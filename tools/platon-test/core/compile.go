package core

import (
	"fmt"
	"io/ioutil"
	"os"
	"os/exec"
	"path"
	"strings"

	"github.com/urfave/cli"
)

var CompileCmd = cli.Command{
	Name:        "compile",
	Description: "compile wasm test file",
	Usage:       "compile wasm test file",
	Before:      HelpBefore("compile"),
	Action:      compile,
	Flags:       compileCmdFlags,
}

var compileCmdFlags = []cli.Flag{
	WasmDirFlag,
	WasmFileFlag,
	OutputFlag,
	BinPathFlag,
	DefineMacroFlag,
	UndefineMacroFlag,
}

func compile(c *cli.Context) error {
	bin := c.String(BinPathFlag.Name)
	output := c.String(OutputFlag.Name)
	dir := c.String(WasmDirFlag.Name)
	define := c.String(DefineMacroFlag.Name)
	undefine := c.String(UndefineMacroFlag.Name)
	if output == "" {
		output = "./"
	}
	if dir != "" {
		return CompileDir(bin, dir, output, define, undefine)
	}
	file := c.String(WasmFileFlag.Name)
	if file == "" {
		cli.ShowCommandHelp(c, "compile")
		return fmt.Errorf("command args error")
	}
	return CompileFile(bin, file, output, define, undefine)
}

func CompileDir(bin, dir, output, define, undefine string) error {
	stat, err := os.Stat(dir)
	if err != nil {
		return err
	}

	if !stat.IsDir() {
		return fmt.Errorf("%s is not directory", dir)
	}

	files, err := ioutil.ReadDir(dir)
	for _, f := range files {
		if f.IsDir() {
			continue
		}
		if strings.HasSuffix(f.Name(), "_test.cpp") {
			CompileFile(bin, path.Join(dir, f.Name()), output, define, undefine)
		}
	}
	return nil
}

func CompileFile(binPath, filePath, outPath, define, undefine string) error {
	_, file := path.Split(filePath)
	output := path.Join(outPath, file+".wasm")
	bin := path.Join(binPath, "platon-cpp")
	//command := fmt.Sprintf("/home/yangzhou/platon/yangzhou/PlatON-CDT/build/bin/platon-cpp -std=c++1z %s -o %s", filePath, output)

	args := []string{}

	if "" != define {
		args = append(args, "-D", define)
	}

	if "" != undefine {
		args = append(args, "-U", undefine)
	}

	args = append(args, filePath, "-o", output)
	cmd := exec.Command(bin, args...)
	out, err := cmd.CombinedOutput()
	if err != nil {
		fmt.Println(string(out))
		return err
	}
	if strings.Contains(string(out), "error:") {
		return fmt.Errorf(string(out))
	}
	return nil
}
