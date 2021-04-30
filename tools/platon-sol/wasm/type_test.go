package wasm

import (
	"io/ioutil"
	"testing"

	"gotest.tools/assert"
)

func TestSplitMap(t *testing.T) {
	first, second := splitMap("map<map<map<int32,int32>, int>, int16>")
	assert.Equal(t, first, "map<map<int32,int32>, int>")
	assert.Equal(t, second, "int16")

	first, second = splitMap("map<int16,map<int32,int32>>")
	assert.Equal(t, first, "int16")
	assert.Equal(t, second, "map<int32,int32>")

	first, second = splitMap("map<int16,int32[][2]>")
	assert.Equal(t, first, "int16")
	assert.Equal(t, second, "int32[][2]")

	first, second = splitMap("map<int32,int32>")
	assert.Equal(t, first, "int32")
	assert.Equal(t, second, "int32")
}

func TestConvertToSolidityType(t *testing.T) {
	mapTypeStruct := make(map[string]*StructDef)

	cases := []struct {
		Input  string
		result string
	}{
		{"uint8[][]", "bytes[]"},
		{"int32[5][]", "int[][]"},
		{"uint16[5][5]", "uint[][]"},
		{"uint8[][5]", "bytes[]"},
		{"map<int32,int32>[10][]", solidityMappingName("map<int32,int32>") + "[][][]"},
		{"map<int32,int32>[][10]", solidityMappingName("map<int32,int32>") + "[][][]"},
		{"map<int32,int32>[10][10]", solidityMappingName("map<int32,int32>") + "[][][]"},
		{"map<int32,int32>[][]", solidityMappingName("map<int32,int32>") + "[][][]"},
		{"map<int32,FixedHash<20>[]>[]", solidityMappingName("map<int32,FixedHash<20>[]>") + "[][]"},
	}

	for _, oneType := range cases {
		result := convertToSolidityType(oneType.Input, mapTypeStruct)
		assert.Equal(t, result, oneType.result)
	}
}

func TestExportInfo(t *testing.T) {
	stream, err := ioutil.ReadFile("./test/contract.abi.json")
	assert.NilError(t, err)

	abiTypes, err := NewABI(string(stream))
	assert.NilError(t, err)

	_, err = Generate(abiTypes)
	assert.NilError(t, err)
}
