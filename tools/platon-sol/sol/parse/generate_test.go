package parse

import (
	"encoding/json"
	"fmt"
	"github.com/goombaio/dag"
	"github.com/stretchr/testify/assert"
	"io/ioutil"
	"platon/abi"
	"testing"
)

type Field struct {
	Name string
	Type string
}
type cppFunction struct {
	Name   string
	Inputs []Field
	Output string
}
type cppStructs struct {
	Name   string
	Fields []Field
}

type cppABI struct {
	Functions []cppFunction
	Structs   []cppStructs
}

func ReadCppAbiFile(path string) (*cppABI, error) {
	buf, err := ioutil.ReadFile(path)
	if err != nil {
		return nil, err
	}
	var abi cppABI
	err = json.Unmarshal(buf, &abi)
	if err != nil {
		return nil, err
	}
	return &abi, nil
}

func ReadSolAbiFile(path string) (*abi.ABI, error) {
	buf, err := ioutil.ReadFile(path)
	if err != nil {
		return nil, err
	}
	var abi abi.ABI
	err = json.Unmarshal(buf, &abi)
	if err != nil {
		return nil, err
	}
	return &abi, nil
}

func TestGenerate(t *testing.T) {
	solAbi, err := ReadSolAbiFile("./test/abi_test.json")
	assert.Nil(t, err)
	cppAbi, err := ReadCppAbiFile("./test/abi_test_cpp.json")
	assert.Nil(t, err)
	structs := createStruct(solAbi)
	funcs := createFunction(solAbi)
	assert.Equal(t, len(cppAbi.Functions), len(funcs))
	assert.Equal(t, len(cppAbi.Structs), len(structs))

	for i, c := range cppAbi.Structs {
		s := structs[i]
		assert.Equal(t, c.Name, s.Name)
		assert.Equal(t, len(c.Fields), len(s.Fields))
		for i := 0; i < len(s.Fields); i++ {
			assert.Equal(t, c.Fields[i].Name, s.Fields[i].Name)
			assert.Equal(t, c.Fields[i].Type, s.Fields[i].Type, fmt.Sprintf("struct:%s, Name:%s", c.Name, c.Fields[i].Name))
		}
	}

	for _, v := range cppAbi.Functions {
		f := funcs[v.Name]
		assert.NotNil(t, f)
		assert.Equal(t, v.Name, f.Name)
		assert.Equal(t, v.Output, f.Output, fmt.Sprintf("return type error function name:%s", v.Name))
		assert.Equal(t, len(v.Inputs), len(f.Inputs))
		for j := 0; j < len(f.Inputs); j++ {
			assert.Equal(t, v.Inputs[j].Name, f.Inputs[j].ArgName, fmt.Sprintf("argument name error function name:%s", v.Name))
			assert.Equal(t, v.Inputs[j].Type, f.Inputs[j].TypeName, fmt.Sprintf("type name error function name:%s", v.Name))
		}
	}
}

func createStruct(abi *abi.ABI) []*StructDef {
	structs := make(map[string]*StructDef)
	structDag := dag.NewDAG()
	for _, m := range abi.Methods {
		generateStructs(m.Inputs, structs, structDag, "")
		generateStructs(m.Inputs, structs, structDag, "")
	}
	return structsDag(structs, structDag)
}

func createFunction(abi *abi.ABI) map[string]*FuncDef {
	funcs := make(map[string]*FuncDef)
	for _, m := range abi.Methods {
		in := generateInputs(m.Inputs)
		out := generateOutputs(m.Outputs)
		funcs[m.Name] = &FuncDef{
			m.Name,
			in,
			out,
		}
	}
	return funcs
}
