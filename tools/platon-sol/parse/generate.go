package parse

import (
	"bytes"
	"fmt"
	"github.com/goombaio/dag"
	"github.com/pkg/errors"
	"platon/abi"
	"strings"
	"text/template"
)

var (
	headers = []string{
		`#include <array>`,
		`#include <tuple>`,
		`#include <vector>`,
		`#include "platon/bigint.hpp"`,
		`#include "platon/solidity/sol_serialize.hpp"`,
		`#include "platon/solidity/sol_cross_call.hpp"`,
	}
)

type Argument struct {
	TypeName string
	ArgName  string
}

type FuncDef struct {
	Name   string
	Inputs []Argument
	Output string
}

type StructMember struct {
	Type string
	Name string
}

type StructDef struct {
	Name   string
	Fields []StructMember //type + name;
}

type Contract struct {
	Namespace string
	Includes  []string
	Structs   []*StructDef
	Functions []*FuncDef
}

func Generate(abi *abi.ABI, namespace string) (c string, err error) {
	var method string
	defer func() {
		if r := recover(); r != nil {
			err = errors.WithMessagef(errors.Errorf("%v", r), "generate function [%s] error", method)
		}
	}()
	contract := &Contract{
		Namespace: namespace,
		Includes:  headers,
	}
	structDag := dag.NewDAG()
	structs := make(map[string]*StructDef)
	for _, m := range abi.Methods {
		method = m.Name
		generateStructs(m.Inputs, structs, structDag, "")
		generateStructs(m.Outputs, structs, structDag, "")
		in := generateInputs(m.Inputs)
		out := generateOutputs(m.Outputs)
		contract.Functions = append(contract.Functions, &FuncDef{
			Name:   m.Name,
			Inputs: in,
			Output: out,
		})
	}
	contract.Structs = structsDag(structs, structDag)

	return exportTmpl(contract)
}

func exportTmpl(contract *Contract) (string, error) {
	helpers := template.FuncMap{
		"generateFuncInputs": func(args []Argument) string {
			var inputArgs []string
			for _, i := range args {
				inputArgs = append(inputArgs, fmt.Sprintf("const %s& %s", i.TypeName, i.ArgName))
			}
			if len(inputArgs) == 0 {
				return ""
			}
			return "," + strings.Join(inputArgs, ",")
		},
		"generateInputs": func(args []Argument) string {
			var inputArgs []string
			for _, i := range args {
				inputArgs = append(inputArgs, i.ArgName)
			}
			if len(inputArgs) == 0 {
				return ""
			}
			return "," + strings.Join(inputArgs, ",")
		},
		"generateSerialize": func(s *StructDef) string {
			fields := ""
			for _, f := range s.Fields {
				fields += fmt.Sprintf("(%s)", f.Name)
			}
			return s.Name + "," + fields
		},
	}
	tmpl, err := template.New("").Funcs(helpers).Parse(cppTemplate)
	if err != nil {
		return "", err
	}
	buffer := bytes.NewBuffer(nil)
	if err := tmpl.Execute(buffer, contract); err != nil {
		return "", err
	}
	return buffer.String(), nil
}

func structsDag(structs map[string]*StructDef, structDag *dag.DAG) []*StructDef {
	defs := make([]*StructDef, len(structs))
	removeVertex := func(tailVertex *dag.Vertex, headVertex *dag.Vertex) {
		structDag.DeleteEdge(tailVertex, headVertex)
		for _, p := range headVertex.Parents.Values() {
			if p == tailVertex {
				headVertex.Parents.Remove(tailVertex)
			}
		}
	}
	tail := defs
	for structDag.Order() != 0 {
		m := structDag.SourceVertices()
		for _, k := range m {
			children, _ := structDag.Successors(k)
			for _, c := range children {
				removeVertex(k, c)
			}
			if len(k.ID) != 0 {
				tail[len(tail)-1] = structs[k.ID]
				tail = tail[:len(tail)-1]
			}
			structDag.DeleteVertex(k)
		}
	}
	return defs
}

func generateInputs(inputs *abi.Type) []Argument {
	defer func() {
		if r := recover(); r != nil {
			panic(errors.Errorf("%v", r))
		}
	}()
	if len(inputs.TupleElems()) != 0 && inputs.Kind() != abi.KindTuple {
		panic("unsupport abi format")
	}
	var args []Argument
	for _, t := range inputs.TupleElems() {
		if len(t.Name) == 0 {
			panic("argument name is empty")
		}
		args = append(args, Argument{
			encodeElem(t.Elem),
			t.Name,
		})
	}
	return args
}

func generateOutputs(outputs *abi.Type) string {
	var args []string
	for _, t := range outputs.TupleElems() {
		args = append(args, encodeElem(t.Elem))
	}
	switch len(args) {
	case 0:
		return "void"
	case 1:
		return args[0]
	default:
		res := fmt.Sprintf("std::tuple<%s>", strings.Join(args, ","))

		return res
	}
}

func generateStructs(s *abi.Type, structs map[string]*StructDef, structDag *dag.DAG, parent string) {
	defer func() {
		if r := recover(); r != nil {
			panic(errors.Errorf("%v", r))
		}
	}()
	switch s.Kind() {
	case abi.KindTuple:
		if len(s.InternalType()) == 0 {
			for _, t := range s.TupleElems() {
				generateStructs(t.Elem, structs, structDag, parent)
			}
		} else {
			def := &StructDef{
				Name: s.InternalType(),
			}

			for _, t := range s.TupleElems() {
				def.Fields = append(def.Fields, StructMember{
					Type: encodeStructMember(t.Elem, structs, structDag, s.InternalType()),
					Name: t.Name,
				})
			}
			structs[s.InternalType()] = def
			child := dag.NewVertex(s.InternalType(), "")
			parent := dag.NewVertex(parent, "")
			if c, _ := structDag.GetVertex(child.ID); c == nil {
				structDag.AddVertex(child)
			} else {
				child = c
			}
			if c, _ := structDag.GetVertex(parent.ID); c == nil {
				structDag.AddVertex(parent)
			} else {
				parent = c
			}
			structDag.AddEdge(parent, child)
		}
	case abi.KindSlice:
		generateStructs(s.Elem(), structs, structDag, parent)
	case abi.KindArray:
		generateStructs(s.Elem(), structs, structDag, parent)
	}
}

func encodeStructMember(s *abi.Type, structs map[string]*StructDef, structDag *dag.DAG, parent string) string {
	var args string
	switch s.Kind() {
	case abi.KindBool:
		args = "bool"
	case abi.KindUInt:
		args = mustSupportIntSize(s.Size(), abi.KindUInt)
	case abi.KindInt:
		args = mustSupportIntSize(s.Size(), abi.KindInt)
	case abi.KindString:
		args = "std::string"
	case abi.KindArray:
		args = fmt.Sprintf("std::array<%s,%d>", encodeElem(s.Elem()), s.Size())
		generateStructs(s.Elem(), structs, structDag, parent)
	case abi.KindAddress:
		args = "platon::Address"
	case abi.KindBytes:
		args = "std::vector<uint8_t>"
	case abi.KindFixedBytes:
		args = fmt.Sprintf("std::array<uint8_t,%d>", s.Size())
	case abi.KindSlice:
		args = fmt.Sprintf("std::vector<%s>", encodeElem(s.Elem()))
		generateStructs(s.Elem(), structs, structDag, parent)
	case abi.KindTuple:
		if s.InternalType() != "" {
			args = s.InternalType()
		}

		generateStructs(s, structs, structDag, parent)
	case abi.KindFunction, abi.KindFixedPoint:
		panic("unsupport function fixedpoint")
	}
	return args
}

func mustSupportIntSize(size int, kind abi.Kind) string {
	switch size {
	case 8, 16, 32, 64, 128, 256:
		switch kind {
		case abi.KindUInt:
			if size == 128 {
				return "__uint128_t"
			}
			if size == 256 {
				return "std::uint256_t"
			}
			return fmt.Sprintf("uint%d_t", size)
		case abi.KindInt:
			if size == 128 {
				return "__int128_t"
			}
			if size == 256 {
				return "std::int256_t"
			}
			return fmt.Sprintf("int%d_t", size)
		}
	}
	panic(fmt.Sprintf("unsupport int bits %d", size))
}

func encodeElem(inputs *abi.Type) string {
	var args string

	switch inputs.Kind() {
	case abi.KindBool:
		args = "bool"
	case abi.KindUInt:

		args = mustSupportIntSize(inputs.Size(), abi.KindUInt)
	case abi.KindInt:
		args = mustSupportIntSize(inputs.Size(), abi.KindInt)
	case abi.KindString:
		args = "std::string"
	case abi.KindArray:
		args = fmt.Sprintf("std::array<%s, %d>", encodeElem(inputs.Elem()), inputs.Size())
	case abi.KindAddress:
		args = "platon::Address"
	case abi.KindBytes:
		args = "std::vector<uint8_t>"
	case abi.KindFixedBytes:
		args = fmt.Sprintf("std::array<uint8_t,%d>", inputs.Size())
	case abi.KindSlice:
		args = fmt.Sprintf("std::vector<%s>", encodeElem(inputs.Elem()))

	case abi.KindTuple:
		if inputs.InternalType() != "" {
			args = inputs.InternalType()
		}

	case abi.KindFunction, abi.KindFixedPoint:
		panic("unsupport function fixedpoint")
	}
	return args
}
