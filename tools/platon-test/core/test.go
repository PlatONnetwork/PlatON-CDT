package core

import (
	"fmt"
	"path"

	"github.com/urfave/cli"
)

var TestCmd = cli.Command{
	Name:        "test",
	Description: "compile & exec wasm test file",
	Usage:       "compile & exec wasm test file",
	Before:      HelpBefore("exec"),
	Action:      test,
	Flags:       compileCmdFlags,
}

// var testCmdFlags = []cli.Flag{
// 	WasmDirFlag,
// 	WasmFileFlag,
// 	OutputFlag,
// 	BinPathFlag,
// }

func test(c *cli.Context) error {
	bin := c.String(BinPathFlag.Name)
	output := c.String(OutputFlag.Name)
	dir := c.String(WasmDirFlag.Name)
	define := c.String(DefineMacroFlag.Name)
	undefine := c.String(UndefineMacroFlag.Name)

	if output == "" {
		output = "./"
	}
	if dir != "" {
		return TestDir(bin, dir, output, define, undefine)
	}
	file := c.String(WasmFileFlag.Name)
	if file == "" {
		cli.ShowCommandHelp(c, "test")
		return fmt.Errorf("command args error")
	}
	return TestFile(bin, file, output, define, undefine)
}

func TestDir(bin, dir, output, define, undefine string) error {
	if err := CompileDir(bin, dir, output, define, undefine); err != nil {
		return err
	}

	return ExecDir(output)
}

func TestFile(binPath, filePath, outPath, define, undefine string) error {
	if err := CompileFile(binPath, filePath, outPath, define, undefine); err != nil {
		return err
	}
	_, file := path.Split(filePath)
	file = path.Join(outPath, file+".wasm")
	return ExecFile(file)
}
