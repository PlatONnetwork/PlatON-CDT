package wasm

import (
	"crypto/sha256"
	"encoding/hex"
	"fmt"
	"strings"

	"github.com/goombaio/dag"
)

func convertToSolidityType(wasmType string) string {
	wasmType = strings.TrimSpace(wasmType)
	switch {
	case "uint8[]" == wasmType:
		return "bytes"
	case "FixedHash<20>" == wasmType:
		return "address"
	case strings.Contains(wasmType, "[]") || (strings.Contains(wasmType, "]") && strings.Contains(wasmType, "[")):
		index := strings.Index(wasmType, "[")
		return convertToSolidityType(wasmType[:index]) + "[]"
	case "bool" == wasmType:
		return wasmType
	case strings.HasPrefix(wasmType, "int"):
		return "int"
	case strings.HasPrefix(wasmType, "uint"):
		return "uint"
	case strings.Contains(wasmType, "map<"):
		left := strings.Index(wasmType, "<")
		middle := strings.Index(wasmType, ",")
		right := strings.Index(wasmType, ">")
		return "mapping(" + convertToSolidityType(wasmType[left+1:middle]) + "=>" + convertToSolidityType(wasmType[middle+1:right]) + ")"
	case "string" == wasmType:
		return "string"
	default:
		return wasmType
	}
}

func hasDependency(solidityType string) bool {
	switch {
	case "bytes" == solidityType || "bool" == solidityType || "address" == solidityType || "string" == solidityType:
		return false
	case strings.Contains(solidityType, "[]") || strings.Contains(solidityType, "mapping"):
		return true
	case strings.HasPrefix(solidityType, "int") || strings.HasPrefix(solidityType, "uint"):
		return false
	default:
		return true
	}
}

func soliditySliceChildType(solidityType string) string {
	end := strings.LastIndex(solidityType, "[]")
	return strings.TrimSpace(solidityType[:end])
}

func solidityMapChildType(solidityType string) (string, string) {
	mappingHead := "mapping("
	left := strings.Index(solidityType, mappingHead)
	middle := strings.Index(solidityType, "=>")
	right := strings.LastIndex(solidityType, ")")
	firstType := strings.TrimSpace(solidityType[left+len(mappingHead) : middle])
	secondType := strings.TrimSpace(solidityType[middle+2 : right])

	return firstType, secondType
}

func analyzeType(solidityType string, analyze *dag.DAG, parent *dag.Vertex) {
	if hasDependency(solidityType) {
		if _, err := analyze.GetVertex(solidityType); nil == err {
			return
		}

		child := dag.NewVertex(solidityType, nil)
		if err := analyze.AddVertex(child); nil != err {
			panic(err)
		}

		if nil != parent {
			if err := analyze.AddEdge(parent, child); nil != err {
				panic(err)
			}
		}

		// slice
		if strings.HasSuffix(solidityType, "[]") {
			childSolidityType := soliditySliceChildType(solidityType)
			analyzeType(childSolidityType, analyze, child)
		}

		// map
		if strings.HasPrefix(solidityType, "mapping") {
			firstType, secondType := solidityMapChildType(solidityType)
			analyzeType(firstType, analyze, child)
			analyzeType(secondType, analyze, child)
		}
	}
}

func analyzeDependency(contract *Contract) {
	analyze := dag.NewDAG()

	// struct
	for _, oneStruct := range contract.Structs {
		vertexStruct := dag.NewVertex(oneStruct.Name, oneStruct.Fields)
		if err := analyze.AddVertex(vertexStruct); nil != err {
			panic(err)
		}
	}

	for _, oneStruct := range contract.Structs {
		if vertexStruct, err := analyze.GetVertex(oneStruct.Name); nil != err {
			panic(err)
		} else {
			for _, fieldInfo := range oneStruct.Fields {
				if hasDependency(fieldInfo.Type) {
					analyzeType(fieldInfo.Type, analyze, vertexStruct)
				}
			}
		}
	}

	// function
	for _, oneFuncInfo := range contract.Functions {
		for _, oneInput := range oneFuncInfo.Inputs {
			analyzeType(oneInput.TypeName, analyze, nil)
		}
	}

	contract.Analyze = analyze
}

func solidityRLPEncodeFunction(solidityType string) string {
	switch {
	case "bytes" == solidityType:
		return "encodeBytes"
	case strings.Contains(solidityType, "[]"):
		return "encodeList"
	case "bool" == solidityType:
		return "encodeUint"
	case strings.HasPrefix(solidityType, "int"):
		return "encodeInt"
	case strings.HasPrefix(solidityType, "uint"):
		return "encodeUint"
	case "address" == solidityType:
		return "encodeAddress"
	case strings.Contains(solidityType, "mapping"):
		return "encodeList"
	case "string" == solidityType:
		return "encodeString"
	default:
		return "encodeList"
	}
}

func solidityRLPDecodeFunction(solidityType string) string {
	switch {
	case "bytes" == solidityType:
		return "toBytes"
	case strings.Contains(solidityType, "[]"):
		return "decodeList"
	case "bool" == solidityType:
		return "toBoolean"
	case strings.HasPrefix(solidityType, "int"):
		return "toInt"
	case strings.HasPrefix(solidityType, "uint"):
		return "toUint"
	case "address" == solidityType:
		return "toAddress"
	case strings.Contains(solidityType, "mapping"):
		return "decodeList"
	case "string" == solidityType:
		return "toString"
	default:
		return "decodeList"
	}
}

func solidityEncodeListName(solidityListType string) string {
	hash := sha256.New()
	hash.Write([]byte(solidityListType))
	sum := hash.Sum(nil)

	name := hex.EncodeToString(sum[:4])
	return "encode_" + name
}

func solidityDecodeListName(solidityListType string) string {
	hash := sha256.New()
	hash.Write([]byte(solidityListType))
	sum := hash.Sum(nil)

	name := hex.EncodeToString(sum[:4])
	return "decode_" + name
}

func generateStructRLPEncode(oneStruct *StructDef) string {
	oneResult := "\n\tfunction " + solidityEncodeListName(oneStruct.Name) + "(" + oneStruct.Name + " memory self) internal pure returns (bytes memory){"
	for index, fieldInfo := range oneStruct.Fields {
		switch encodefunc := solidityRLPEncodeFunction(fieldInfo.Type); encodefunc {
		case "encodeBytes", "encodeInt", "encodeUint", "encodeAddress", "encodeString":
			oneResult += fmt.Sprintf("\n\t\tbytes memory rlpPara%d", index) + "= RLPEncode." + encodefunc + "(self." + fieldInfo.Name + ");"
		case "encodeList":
			oneRealEncodeFunction := solidityEncodeListName(fieldInfo.Type)
			oneResult += fmt.Sprintf("\n\t\tbytes memory rlpPara%d", index) + " = " + oneRealEncodeFunction + "(self." + fieldInfo.Name + ");"
		}
	}

	length := len(oneStruct.Fields)
	oneResult += fmt.Sprintf("\n\t\tbytes[] memory allRlpPara = new bytes[](%d);", length)
	for i := 0; i < length; i++ {
		oneResult += fmt.Sprintf("\n\t\tallRlpPara[%d] = rlpPara%d;", i, i)
	}

	oneResult += "\n\t\tbytes memory payload = RLPEncode.encodeList(allRlpPara);\n\t\treturn payload;\n\t}\n"

	return oneResult
}

func generateStructRLPDecode(oneStruct *StructDef) string {
	oneResult := "\n\tfunction " + solidityDecodeListName(oneStruct.Name) + "(bytes memory data) internal pure returns (" + oneStruct.Name + " memory){"
	oneResult += "\n\t\t" + oneStruct.Name + " memory result;"
	oneResult += "\n\t\tRLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);"
	oneResult += "\n\t\tRLPReader.RLPItem[] memory allItem = RLPReader.toList(rlpItem);"
	for index, fieldInfo := range oneStruct.Fields {
		switch decodefunc := solidityRLPDecodeFunction(fieldInfo.Type); decodefunc {
		case "toBytes", "toBoolean", "toInt", "toUint", "toAddress", "toString":
			oneResult += fmt.Sprintf("\n\t\tresult."+fieldInfo.Name+" = RLPReader."+decodefunc+"(allItem[%d]);", index)
		case "decodeList":
			oneRealDecodeFunction := solidityDecodeListName(fieldInfo.Type)
			oneResult += fmt.Sprintf("\n\t\tresult."+fieldInfo.Name+" = "+oneRealDecodeFunction+"(allItem[%d]);", index)
		}
	}

	oneResult += "\n\t\treturn result;\n\t}\n"

	return oneResult
}

func generateVectorRLPEncode(oneVector *Vector) string {
	oneRealEncodeFunction := solidityEncodeListName(oneVector.Type)
	oneResult := "\n\tfunction " + oneRealEncodeFunction + "(" + oneVector.Type + " memory self) internal pure returns (bytes memory){"
	oneResult += "\n\t\tuint length = self.length;"
	oneResult += "\n\t\tbytes[] memory allRlpPara = new bytes[](length);"

	index := strings.Index(oneVector.Type, "[]")
	realEncodeType := strings.TrimSpace(oneVector.Type[:index])
	switch encodefunc := solidityRLPEncodeFunction(realEncodeType); encodefunc {
	case "encodeBytes", "encodeInt", "encodeUint", "encodeAddress", "encodeString":
		oneResult += "\n\t\tfor (uint i=0; i<length; i++) {"
		oneResult += "\n\t\t\tallRlpPara[i] = RLPEncode." + encodefunc + "(self[i]);"
		oneResult += "\n\t\t}"
	case "encodeList":
		oneRealEncodeFunction := solidityEncodeListName(realEncodeType)
		oneResult += "\n\t\tfor (uint i=0; i<length; i++) {"
		oneResult += "\n\t\t\tallRlpPara[i] = " + oneRealEncodeFunction + "(self[i]);"
		oneResult += "\n\t\t}"
	}

	oneResult += "\n\t\tbytes memory payload = RLPEncode.encodeList(allRlpPara);\n\t\treturn payload;\n\t}\n"

	return oneResult
}

func generateVectorRLPDecode(oneVector *Vector) string {
	oneRealDecodeFunction := solidityDecodeListName(oneVector.Type)
	oneResult := "\n\tfunction " + oneRealDecodeFunction + "(bytes memory data) internal pure returns (" + oneVector.Type + " memory){"
	oneResult += "\n\t\tRLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);"
	oneResult += "\n\t\tRLPReader.RLPItem[] memory allItem = RLPReader.toList(rlpItem);"
	oneResult += "\n\t\tuint length = RLPReader.numItems(rlpItem);"
	oneResult += "\n\t\t" + oneVector.Type + " memory result = new " + oneVector.Type + "(length);"

	index := strings.Index(oneVector.Type, "[]")
	realDecodeType := strings.TrimSpace(oneVector.Type[:index])

	switch decodefunc := solidityRLPDecodeFunction(realDecodeType); decodefunc {
	case "toBytes", "toBoolean", "toInt", "toUint", "toAddress", "toString":
		oneResult += "\n\t\tfor (uint i=0; i<length; i++) {"
		oneResult += "\n\t\t\tresult[i] = RLPReader." + decodefunc + "(allItem[i]);"
		oneResult += "\n\t\t}"

	case "decodeList":
		oneRealDecodeFunction := solidityDecodeListName(realDecodeType)
		oneResult += "\n\t\tfor (uint i=0; i<length; i++) {"
		oneResult += "\n\t\t\tresult[i] = RLPReader." + oneRealDecodeFunction + "(allItem[i]);"
		oneResult += "\n\t\t}"
	}

	oneResult += "\n\t\treturn result;\n\t}\n"
	return oneResult
}

func generateMapRLPEncode(oneMap *Map) string {

	mappingType := "mapping(" + oneMap.First + "=>" + oneMap.Second + ")"
	oneRealEncodeFunction := solidityEncodeListName(mappingType)
	arrayType := oneMap.First + "[]"
	oneResult := "\n\tfunction " + oneRealEncodeFunction + "(" + mappingType + " memory self, " + arrayType + " memory array) internal pure returns (bytes memory){"
	oneResult += "\n\t\tuint length = array.length;"
	oneResult += "\n\t\tbytes[] memory allRlpPara = new bytes[](length);"

	encodeFirstfunc := solidityRLPEncodeFunction(oneMap.First)
	if "encodeList" == encodeFirstfunc {
		encodeFirstfunc = solidityEncodeListName(oneMap.First)
	} else {
		encodeFirstfunc = "RLPEncode." + encodeFirstfunc
	}

	encodeSecondfunc := solidityRLPEncodeFunction(oneMap.Second)
	if "encodeList" == encodeSecondfunc {
		encodeSecondfunc = solidityEncodeListName(oneMap.Second)
	} else {
		encodeSecondfunc = "RLPEncode." + encodeSecondfunc
	}

	oneResult += "\n\t\tfor (uint i=0; i<length; i++) {"
	oneResult += "\n\t\t\tbytes memory first = " + encodeFirstfunc + "(array[i]);"
	oneResult += "\n\t\t\tbytes memory second = " + encodeSecondfunc + "(self[array[i]]);"
	oneResult += "\n\t\t\tbytes[] memory first2second = new bytes[](2);"
	oneResult += "\n\t\t\tfirst2second[0] = first;"
	oneResult += "\n\t\t\tfirst2second[1] = decond;"
	oneResult += "\n\t\t\tallRlpPara[i] = RLPEncode.encodeList(tfirst2second);"
	oneResult += "\n\t\t}"

	oneResult += "\n\t\tbytes memory payload = RLPEncode.encodeList(allRlpPara);\n\t\treturn payload;\n\t}\n"

	return oneResult
}

func generateMapRLPDecode(oneMap *Map) string {

	mappingType := "mapping(" + oneMap.First + "=>" + oneMap.Second + ")"
	oneRealDecodeFunction := solidityDecodeListName(mappingType)

	oneResult := "\n\tfunction " + oneRealDecodeFunction + "(bytes memory data) internal pure returns (" + mappingType + " memory){"
	oneResult += "\n\t\tRLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);"
	oneResult += "\n\t\tRLPReader.RLPItem[] memory allItem = RLPReader.toList(rlpItem);"
	oneResult += "\n\t\tuint length = RLPReader.numItems(rlpItem);"
	oneResult += "\n\t\t" + mappingType + " memory result;"

	decodeFirstFunc := solidityRLPDecodeFunction(oneMap.First)
	if "decodeList" == decodeFirstFunc {
		decodeFirstFunc = solidityDecodeListName(oneMap.First)
	} else {
		decodeFirstFunc = "RLPReader." + decodeFirstFunc
	}

	decodeSecondFunc := solidityRLPDecodeFunction(oneMap.Second)
	if "decodeList" == decodeSecondFunc {
		decodeSecondFunc = solidityDecodeListName(oneMap.Second)
	} else {
		decodeSecondFunc = "RLPReader." + decodeSecondFunc
	}

	oneResult += "\n\t\tfor (uint i=0; i<length; i++) {"
	oneResult += "\n\t\t\tRLPReader.RLPItem[] memory rlpIFirstSecond = RLPReader.toList(allItem[i]);"
	oneResult += "\n\t\t\t" + oneMap.First + "memory first = " + decodeFirstFunc + "(rlpIFirstSecond[0]);"
	oneResult += "\n\t\t\tresult[first] = " + decodeSecondFunc + "(rlpIFirstSecond[1]);"
	oneResult += "\n\t\t}"

	oneResult += "\n\t\treturn result;\n\t}\n"

	return oneResult
}

func generateOneRLP(oneVertices *dag.Vertex) string {
	var result string
	solidityType := oneVertices.ID

	// slice
	if strings.HasSuffix(solidityType, "[]") {
		oneVector := &Vector{solidityType}
		result += generateVectorRLPEncode(oneVector)
		result += generateVectorRLPDecode(oneVector)
	} else if strings.HasPrefix(solidityType, "mapping") {
		firstType, secondType := solidityMapChildType(solidityType)
		oneMap := &Map{firstType, secondType}
		result += generateMapRLPEncode(oneMap)
		result += generateMapRLPDecode(oneMap)
	} else {

		fields, ok := oneVertices.Value.([]*FieldInfo)
		if !ok {
			panic("invalid type")
		}

		oneStruct := &StructDef{solidityType, fields}
		result += generateStructRLPEncode(oneStruct)
		result += generateStructRLPDecode(oneStruct)
	}

	return result
}

func traverseVertices(analyze *dag.DAG, oneVertices *dag.Vertex, unique *map[string]struct{}, result *string) {
	parentVertices, err := analyze.Predecessors(oneVertices)
	if nil != err {
		panic(err)
	}

	for _, oneParentVertices := range parentVertices {
		if _, ok := (*unique)[oneParentVertices.ID]; ok {
			continue
		}

		*result += generateOneRLP(oneParentVertices)
		(*unique)[oneParentVertices.ID] = struct{}{}
	}

	for _, oneParentVertices := range parentVertices {
		traverseVertices(analyze, oneParentVertices, unique, result)
	}
}

func memoryType(solidityType string) bool {
	switch {
	case "bool" == solidityType || "address" == solidityType:
		return false
	case "bytes" == solidityType || "string" == solidityType:
		return true
	case strings.Contains(solidityType, "[]") || strings.Contains(solidityType, "mapping"):
		return true
	case strings.HasPrefix(solidityType, "int") || strings.HasPrefix(solidityType, "uint"):
		return false
	default:
		return true
	}
}
