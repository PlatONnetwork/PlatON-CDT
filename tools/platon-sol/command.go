package main

import (
	"errors"
	"fmt"
	"io/ioutil"
	"path"
	"platon/sol/abi"
	"platon/sol/parse"
	"platon/wasm"

	"github.com/urfave/cli"
)

func HelpBefore(name string) func(c *cli.Context) error {
	return func(c *cli.Context) error {
		if c.BoolT("help") {
			cli.ShowCommandHelp(c, name)
			return fmt.Errorf("before help")
		}
		return nil
	}
}

var CallWasmCmd = cli.Command{
	Name:        "wasm",
	Description: "Call wasm across contracts",
	Usage:       "Generate cross-contract call solidity library WASM based on wasm abi file",
	Before:      HelpBefore("wasm"),
	Action:      callWasm,
	Flags:       callWasmFlags,
}

func callWasm(c *cli.Context) error {
	wasmAbiFile := c.String(wasmAbiFileFlag.Name)
	output := c.String(wasmOutputFlag.Name)

	if 0 == len(wasmAbiFile) {
		return errors.New("Need a valid wasm abi file path")
	}

	stream, err := ioutil.ReadFile(wasmAbiFile)
	if err != nil {
		return err
	}

	output = createOutputPath(wasmAbiFile, output, true)

	abiTypes, err := wasm.NewABI(string(stream))
	if err != nil {
		return err
	}

	content, err := wasm.Generate(abiTypes)
	if err != nil {
		return err
	}

	if err := writeFile(content, output); err != nil {
		return err
	}

	return nil
}

var CallSolidityCmd = cli.Command{
	Name:        "solidity",
	Description: "Call solidity across contracts",
	Usage:       "Generate wasm contract header files that call solidity across contracts",
	Before:      HelpBefore("solidity"),
	Action:      callSolidity,
	Flags:       callSolidityFlags,
}

func callSolidity(c *cli.Context) error {
	solidityAbiFile := c.String(solidityAbiFileFlag.Name)
	output := c.String(SolidityOutputFlag.Name)
	namespace := c.String(namespaceFlag.Name)

	stream, err := ioutil.ReadFile(solidityAbiFile)
	if err != nil {
		return err
	}

	if len(namespace) == 0 {
		namespace = pathToNamespace(solidityAbiFile)
	}

	output = createOutputPath(solidityAbiFile, output, false)

	abiTypes, err := abi.NewABI(string(stream))
	if err != nil {
		return err
	}

	content, err := parse.Generate(abiTypes, namespace)
	if err != nil {
		return err
	}
	if err := writeFile(content, output); err != nil {
		return err
	}

	return nil
}

func pathToNamespace(filepath string) string {
	filenameAll := path.Base(filepath)
	fileSuffix := path.Ext(filepath)
	return filenameAll[0 : len(filenameAll)-len(fileSuffix)]
}

func createOutputPath(abi, output string, isWasm bool) string {
	if len(output) == 0 {
		output = "./"
	}
	suffix := path.Ext(output)
	if len(suffix) != 0 {
		return output
	}
	base := pathToNamespace(abi)

	if isWasm {
		return path.Join(output, base+".sol")
	}
	return path.Join(output, base+".hpp")
}

func writeFile(content, output string) error {
	return ioutil.WriteFile(output, []byte(content), 0666)
}
