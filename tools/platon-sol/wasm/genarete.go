package wasm

import (
	"bytes"
	"fmt"
	"text/template"

	"github.com/goombaio/dag"
)

type Argument struct {
	TypeName string
	ArgName  string
}

type FuncDef struct {
	Name        string
	Description string
	Inputs      []*Argument
	Output      string
}

func NewFuncDef() *FuncDef {
	return &FuncDef{
		Inputs: make([]*Argument, 0),
	}
}

type FieldInfo struct {
	Name string
	Type string
}

type StructDef struct {
	Name   string
	Fields []*FieldInfo
}

func NewStructDef() *StructDef {
	return &StructDef{
		Fields: make([]*FieldInfo, 0),
	}
}

type Vector struct {
	Type string
}

type Map struct {
	First  string
	Second string
}

type Contract struct {
	Structs   []*StructDef
	Functions []*FuncDef
	Analyze   *dag.DAG
}

func NewContract() *Contract {
	return &Contract{
		Structs:   make([]*StructDef, 0),
		Functions: make([]*FuncDef, 0),
	}
}

func Generate(abi *ABI) (c string, err error) {
	contract := NewContract()

	// structures
	for oneStructName, oneStructInfo := range abi.Structs {
		structDef := NewStructDef()
		structDef.Name = oneStructName
		for index, value := range oneStructInfo.Baseclass {
			structDef.Fields = append(structDef.Fields, &FieldInfo{fmt.Sprintf("baseclass%d", index), convertToSolidityType(value, mapTypeStruct)})
		}

		for _, value := range oneStructInfo.Fields {
			structDef.Fields = append(structDef.Fields, &FieldInfo{value.Name, convertToSolidityType(value.Type, mapTypeStruct)})
		}

		contract.Structs = append(contract.Structs, structDef)
	}

	// functions
	for oneFuncName, oneFuncInfo := range abi.Methods {
		if "init" == oneFuncName {
			continue
		}

		funcDef := NewFuncDef()
		funcDef.Name = oneFuncName
		if oneFuncInfo.Constant {
			funcDef.Description = "internal"
		} else {
			funcDef.Description = "internal"
		}

		funcDef.Output = convertToSolidityType(oneFuncInfo.Output, mapTypeStruct)

		for _, value := range oneFuncInfo.Input {
			funcDef.Inputs = append(funcDef.Inputs, &Argument{convertToSolidityType(value.Type, mapTypeStruct), value.Name})
		}

		contract.Functions = append(contract.Functions, funcDef)
	}

	for _, oneStruct := range mapTypeStruct {
		contract.Structs = append(contract.Structs, oneStruct)
	}

	analyzeDependency(contract)

	return exportTmpl(contract)

}

func exportTmpl(contract *Contract) (string, error) {
	helpers := template.FuncMap{
		"generateRLP": func(analyze *dag.DAG) string {
			var result string
			end := analyze.SinkVertices()
			unique := map[string]struct{}{}

			for _, oneEnd := range end {
				result += generateOneRLP(oneEnd)
				unique[oneEnd.ID] = struct{}{}
			}

			for _, oneEnd := range end {
				traverseVertices(analyze, oneEnd, &unique, &result)
			}

			return result

			// return "generateRLP"
		},

		"generateFuncParameter": func(args []*Argument) string {
			var result string
			for _, oneArg := range args {
				result += ", " + declareSolidityType(oneArg.TypeName) + " " + oneArg.ArgName
			}

			return result
		},
		"generateReturn": func(outputType string) string {
			return declareSolidityType(outputType)
		},
		"generateRLPEncodeFuncInputs": func(args []*Argument) string {
			var result string
			for index, oneArg := range args {
				switch encodefunc := solidityRLPEncodeFunction(oneArg.TypeName); encodefunc {
				case "encodeBytes", "encodeInt", "encodeUint", "encodeAddress", "encodeString":
					result += fmt.Sprintf("\n\t\tbytes memory rlpPara%d = RLPEncode."+encodefunc+"("+oneArg.ArgName+");", index)
				case "encodeList":
					oneRealEncodeFunction := solidityEncodeListName(oneArg.TypeName)
					result += fmt.Sprintf("\n\t\tbytes memory rlpPara%d = "+oneRealEncodeFunction+"("+oneArg.ArgName+");", index)
				}
			}

			length := len(args)
			result += fmt.Sprintf("\n\t\tbytes[] memory allInput = new bytes[](%d);", length+1)
			result += "\n\t\tallInput[0] = rlpMethord;"
			for i := 0; i < length; i++ {
				result += fmt.Sprintf("\n\t\tallInput[%d] = rlpPara%d;", i+1, i)
			}

			result += "\n\t\tbytes memory payload = RLPEncode.encodeList(allInput);"

			return result
		},
		"generateRLPDecodeFuncOutput": func(outputType string) string {
			var (
				result    string
				parameter string
			)

			decodeOutputFunc := solidityRLPDecodeFunction(outputType)
			if "decodeList" == decodeOutputFunc {
				decodeOutputFunc = solidityDecodeListName(outputType)
				parameter = "data"
			} else {
				result += "\n\t\tRLPReader.RLPItem memory rlpItem = RLPReader.toRlpItem(data);"
				parameter = "rlpItem"
				decodeOutputFunc = "RLPReader." + decodeOutputFunc
			}

			result += "\n\t\t" + declareSolidityType(outputType) + " result  = " + decodeOutputFunc + "(" + parameter + ");"

			return result
		},
	}
	tmpl, err := template.New("").Funcs(helpers).Parse(solTemplate)
	if err != nil {
		return "", err
	}
	buffer := bytes.NewBuffer(nil)
	if err := tmpl.Execute(buffer, contract); err != nil {
		return "", err
	}
	return buffer.String(), nil
}
