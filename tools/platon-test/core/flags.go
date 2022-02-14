package core

import "github.com/urfave/cli"

var WasmDirFlag = cli.StringFlag{
	Name:  "dir",
	Usage: "wasm test directory",
}

var WasmFileFlag = cli.StringFlag{
	Name:  "file",
	Usage: "wasm test file",
}

var SolFileFlag = cli.StringFlag{
	Name:  "file",
	Usage: "solidity test file",
}

var OutputFlag = cli.StringFlag{
	Name:  "output",
	Usage: "compile wasm test directory",
}

var BinPathFlag = cli.StringFlag{
	Name:  "bin",
	Usage: "bin path",
}

var DefineMacroFlag = cli.StringFlag{
	Name:  "define",
	Usage: "Define conditional compilation macros.",
}

var UndefineMacroFlag = cli.StringFlag{
	Name:  "undefine",
	Usage: "Undefine conditional compilation macros.",
}
