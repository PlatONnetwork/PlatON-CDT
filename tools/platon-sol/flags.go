package main

import "github.com/urfave/cli"

var callWasmFlags = []cli.Flag{
	wasmAbiFileFlag,
	wasmOutputFlag,
}

var callSolidityFlags = []cli.Flag{
	solidityAbiFileFlag,
	SolidityOutputFlag,
	namespaceFlag,
}

var solidityAbiFileFlag = cli.StringFlag{
	Name:  "solidityAbi",
	Usage: "solidity abi file path",
}

var wasmAbiFileFlag = cli.StringFlag{
	Name:  "wasmAbi",
	Usage: "wasm abi file path",
}

var wasmOutputFlag = cli.StringFlag{
	Name:  "output",
	Usage: "wasm contract output directory",
}

var SolidityOutputFlag = cli.StringFlag{
	Name:  "output",
	Usage: "solidity contract output directory",
}

var namespaceFlag = cli.StringFlag{
	Name:  "namespace",
	Usage: "wasm contract cpp namespace, default abi file name",
}
