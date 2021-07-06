package wasm

import (
	"bytes"
	"encoding/json"
	"fmt"
	"io"
)

type argument struct {
	Name string
	Type string
}

type arguments []argument

type Method struct {
	Constant bool
	Input    arguments
	Output   string
}

type Struct struct {
	Baseclass []string
	Fields    arguments
}

type ABI struct {
	Structs map[string]*Struct
	Methods map[string]*Method
}

func NewABI(s string) (*ABI, error) {
	return NewABIFromReader(bytes.NewReader([]byte(s)))
}

// MustNewABI returns a parsed ABI contract or panics if fails
func MustNewABI(s string) *ABI {
	a, err := NewABI(s)
	if err != nil {
		panic(err)
	}
	return a
}

// NewABIFromReader returns an ABI object from a reader
func NewABIFromReader(r io.Reader) (*ABI, error) {
	var abi *ABI
	dec := json.NewDecoder(r)
	if err := dec.Decode(&abi); err != nil {
		return nil, err
	}
	return abi, nil
}

// UnmarshalJSON implements json.Unmarshaler interface
func (a *ABI) UnmarshalJSON(data []byte) error {
	var fields []struct {
		Type      string
		Name      string
		Constant  bool
		Baseclass []string
		Input     arguments
		Output    string
		Fields    arguments
		Anonymous bool
	}

	if err := json.Unmarshal(data, &fields); err != nil {
		return err
	}

	a.Methods = make(map[string]*Method)
	a.Structs = make(map[string]*Struct)

	for _, field := range fields {
		switch field.Type {
		case "struct":
			a.Structs[field.Name] = &Struct{
				Baseclass: field.Baseclass,
				Fields:    field.Fields,
			}
		case "Action":
			a.Methods[field.Name] = &Method{
				Constant: field.Constant,
				Input:    field.Input,
				Output:   field.Output,
			}

		case "Event":
			// do nothing

		default:
			return fmt.Errorf("unknown field type '%s'", field.Type)
		}
	}
	return nil
}
