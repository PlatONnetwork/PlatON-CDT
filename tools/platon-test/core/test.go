package core

import (
	"fmt"
	"github.com/urfave/cli"
	"path"
)

var TestCmd = cli.Command{
	Name:        "test",
	Description: "compile & exec wasm test file",
	Usage:       "compile & exec wasm test file",
	Before:      HelpBefore("exec"),
	Action:      test,
	Flags:       compileCmdFlags,
}

var testCmdFlags = []cli.Flag{
	WasmDirFlag,
	WasmFileFlag,
	OutputFlag,
	BinPathFlag,
}

func test(c *cli.Context) error {
	bin := c.String(BinPathFlag.Name)
	output := c.String(OutputFlag.Name)
	dir := c.String(WasmDirFlag.Name)
	if output == "" {
		output = "./"
	}
	if dir != "" {
		return TestDir(bin, dir, output)
	}
	file := c.String(WasmFileFlag.Name)
	if file == "" {
		cli.ShowCommandHelp(c, "test")
		return fmt.Errorf("command args error")
	}
	return TestFile(bin, file, output)
}

func TestDir(bin, dir, output string) error {
	if err := CompileDir(bin, dir, output); err != nil {
		return err
	}

	return ExecDir(output)
}

func TestFile(binPath, filePath, outPath string) error {
	if err := CompileFile(binPath, filePath, outPath); err != nil {
		return err
	}
	_, file := path.Split(filePath)
	file = path.Join(outPath, file+".wasm")
	return ExecFile(file)
}
