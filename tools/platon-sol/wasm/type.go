package wasm

import (
	"crypto/sha256"
	"encoding/hex"
	"fmt"
	"strings"

	"github.com/goombaio/dag"
)

const (
	mapHead   string = "map<"
	tupleHead string = "tuple<"
)

var (
	mapTypeStruct = make(map[string]*StructDef)
)

func splitTupleAndMap(wasmType string) []string {
	wasmType = strings.TrimSpace(wasmType)
	if !strings.HasPrefix(wasmType, mapHead) && !strings.HasPrefix(wasmType, tupleHead) {
		panic("Not a map or tuple type")
	}

	left := strings.Index(wasmType, "<")
	right := strings.LastIndex(wasmType, ">")
	tempWasmType := wasmType[left+1 : right]

	iterms := strings.Split(tempWasmType, ",")
	result := make([]string, 0, len(iterms))

	match := 0
	begin := 0
	next := false
	for i := 0; i < len(iterms); i = i + 1 {
		if 0 != strings.Count(iterms[i], mapHead) || 0 != strings.Count(iterms[i], tupleHead) {
			if !next {
				next = true
				begin = i
			}
			match += strings.Count(iterms[i], mapHead)
			match += strings.Count(iterms[i], tupleHead)
		} else {
			if !next {
				result = append(result, strings.TrimSpace(iterms[i]))
			} else {
				oneDelete := strings.Count(iterms[i], ">")
				match -= oneDelete

				if 0 == match {
					next = false
					oneCom := strings.Join(iterms[begin:i+1], ",")
					result = append(result, strings.TrimSpace(oneCom))
					begin = 0
				}
			}
		}
	}

	return result
}

func splitMap(mapType string) (string, string) {

	result := splitTupleAndMap(mapType)

	if 2 != len(result) {
		panic("Invalid map type")
	}

	return result[0], result[1]
}

func splitTuple(tupleType string) []string {
	return splitTupleAndMap(tupleType)
}

func solidityMappingName(mapType string) string {
	hash := sha256.New()
	hash.Write([]byte(mapType))
	sum := hash.Sum(nil)

	name := hex.EncodeToString(sum[:4])
	return "mapping_" + name
}

func solidityTupleName(tupleType string) string {
	hash := sha256.New()
	hash.Write([]byte(tupleType))
	sum := hash.Sum(nil)

	name := hex.EncodeToString(sum[:4])
	return "tuple_" + name
}

func convertToSolidityType(wasmType string, mapTypeStruct map[string]*StructDef) string {
	wasmType = strings.TrimSpace(wasmType)
	switch {
	case "uint8[]" == wasmType:
		return "bytes"
	case "FixedHash<20>" == wasmType:
		return "address"
	case strings.HasSuffix(wasmType, "]"):
		index := strings.LastIndex(wasmType, "[")
		return convertToSolidityType(wasmType[:index], mapTypeStruct) + wasmType[index:]
	case "bool" == wasmType:
		return wasmType
	case strings.HasPrefix(wasmType, "int"):
		return "int"
	case strings.HasPrefix(wasmType, "uint"):
		return "uint"
	case strings.HasPrefix(wasmType, "map<"):
		result := solidityMappingName(wasmType)
		if _, ok := mapTypeStruct[result]; !ok {
			first, second := splitMap(wasmType)
			realFirst := convertToSolidityType(first, mapTypeStruct)
			realSecond := convertToSolidityType(second, mapTypeStruct)

			fields := make([]*FieldInfo, 2)
			fields[0] = &FieldInfo{"first", realFirst}
			fields[1] = &FieldInfo{"second", realSecond}
			oneStruct := &StructDef{result, fields}

			mapTypeStruct[result] = oneStruct
		}

		return result + "[]"
	case strings.HasPrefix(wasmType, "tuple<"):
		result := solidityTupleName(wasmType)
		if _, ok := mapTypeStruct[result]; !ok {
			iterms := splitTuple(wasmType)
			fields := make([]*FieldInfo, len(iterms))

			for i := 0; i < len(iterms); i += 1 {
				oneItermType := convertToSolidityType(iterms[i], mapTypeStruct)
				fields[i] = &FieldInfo{fmt.Sprintf("tuple_%d", i), oneItermType}
			}

			oneStruct := &StructDef{result, fields}

			mapTypeStruct[result] = oneStruct
		}

		return result
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
	case (strings.Contains(solidityType, "[") && strings.Contains(solidityType, "]")) || strings.Contains(solidityType, "mapping_") || strings.Contains(solidityType, "tuple_"):
		return true
	case strings.HasPrefix(solidityType, "int") || strings.HasPrefix(solidityType, "uint"):
		return false
	default:
		return true
	}
}

func solidityArrayChildType(solidityType string) string {
	end := strings.LastIndex(solidityType, "[")
	return strings.TrimSpace(solidityType[:end])
}

func solidityMapChildType(solidityType string, mapTypeStruct map[string]*StructDef) (string, string) {

	oneStruct, ok := mapTypeStruct[solidityType]
	if !ok {
		panic("Invalid mapping")
	}

	fileds := oneStruct.Fields

	return fileds[0].Type, fileds[1].Type
}

func analyzeType(solidityType string, analyze *dag.DAG, parent *dag.Vertex) {
	if hasDependency(solidityType) {
		if existChild, err := analyze.GetVertex(solidityType); nil == err {
			if nil != parent {
				if err := analyze.AddEdge(parent, existChild); nil != err {
					panic(err)
				}
			}
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

		// array
		if strings.HasSuffix(solidityType, "]") {
			childSolidityType := solidityArrayChildType(solidityType)
			analyzeType(childSolidityType, analyze, child)
			return
		}

		// map
		if strings.HasPrefix(solidityType, "mapping_") {
			firstType, secondType := solidityMapChildType(solidityType, mapTypeStruct)
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
	case strings.Contains(solidityType, "[") && strings.Contains(solidityType, "]"):
		return "encodeList"
	case "bool" == solidityType:
		return "encodeUint"
	case strings.HasPrefix(solidityType, "int"):
		return "encodeInt"
	case strings.HasPrefix(solidityType, "uint"):
		return "encodeUint"
	case "address" == solidityType:
		return "encodeAddress"
	case strings.Contains(solidityType, "mapping_"):
		return "encodeList"
	case strings.Contains(solidityType, "tuple_"):
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
	case strings.Contains(solidityType, "[") && strings.Contains(solidityType, "]"):
		return "decodeList"
	case "bool" == solidityType:
		return "toBoolean"
	case strings.HasPrefix(solidityType, "int"):
		return "toInt"
	case strings.HasPrefix(solidityType, "uint"):
		return "toUint"
	case "address" == solidityType:
		return "toAddress"
	case strings.Contains(solidityType, "mapping_"):
		return "decodeList"
	case strings.Contains(solidityType, "tuple_"):
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
			oneResult += fmt.Sprintf("\n\t\tresult."+fieldInfo.Name+" = "+oneRealDecodeFunction+"(RLPReader.toRlpBytes(allItem[%d]));", index)
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

	index := strings.LastIndex(oneVector.Type, "[")
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
	oneResult += "\n\t\tuint length = allItem.length;"
	if strings.HasSuffix(oneVector.Type, "[]") {
		oneResult += "\n\t\t" + oneVector.Type + " memory result = new " + oneVector.Type + "(length);"
	} else {
		oneResult += "\n\t\t" + oneVector.Type + " memory result;"
	}

	index := strings.LastIndex(oneVector.Type, "[")
	realDecodeType := strings.TrimSpace(oneVector.Type[:index])

	switch decodefunc := solidityRLPDecodeFunction(realDecodeType); decodefunc {
	case "toBytes", "toBoolean", "toInt", "toUint", "toAddress", "toString":
		oneResult += "\n\t\tfor (uint i=0; i<length; i++) {"
		oneResult += "\n\t\t\tresult[i] = RLPReader." + decodefunc + "(allItem[i]);"
		oneResult += "\n\t\t}"

	case "decodeList":
		oneRealDecodeFunction := solidityDecodeListName(realDecodeType)
		oneResult += "\n\t\tfor (uint i=0; i<length; i++) {"
		oneResult += "\n\t\t\tresult[i] = " + oneRealDecodeFunction + "(RLPReader.toRlpBytes(allItem[i]));"
		oneResult += "\n\t\t}"
	}

	oneResult += "\n\t\treturn result;\n\t}\n"
	return oneResult
}

func generateOneRLP(oneVertices *dag.Vertex) string {
	var result string
	solidityType := oneVertices.ID

	// array
	if strings.HasSuffix(solidityType, "]") {
		oneVector := &Vector{solidityType}
		result += generateVectorRLPEncode(oneVector)
		result += generateVectorRLPDecode(oneVector)
	} else if strings.HasPrefix(solidityType, "mapping_") {
		oneStruct, ok := mapTypeStruct[solidityType]
		if !ok {
			panic("Invalid mapping")
		}
		result += generateStructRLPEncode(oneStruct)
		result += generateStructRLPDecode(oneStruct)
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

func declareSolidityType(solidityType string) string {
	switch {
	case "bool" == solidityType || "address" == solidityType:
		return solidityType
	case "bytes" == solidityType || "string" == solidityType:
		return solidityType + " memory"
	case (strings.Contains(solidityType, "[") && strings.Contains(solidityType, "]")) || strings.Contains(solidityType, "mapping_") || strings.Contains(solidityType, "tuple_"):
		return solidityType + " memory"
	case strings.HasPrefix(solidityType, "int") || strings.HasPrefix(solidityType, "uint"):
		return solidityType
	default:
		return solidityType + " memory"
	}
}
