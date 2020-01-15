package core

import (
	"fmt"
	"github.com/urfave/cli"
	"io/ioutil"
	"os"
	"os/exec"
	"path"
	"strings"
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
}

func compile(c *cli.Context) error {
	bin := c.String(BinPathFlag.Name)
	output := c.String(OutputFlag.Name)
	dir := c.String(WasmDirFlag.Name)
	if output == "" {
		output = "./"
	}
	if dir != "" {
		return CompileDir(bin, dir, output)
	}
	file := c.String(WasmFileFlag.Name)
	if file == "" {
		cli.ShowCommandHelp(c, "compile")
		return fmt.Errorf("command args error")
	}
	return CompileFile(bin, file, output)
}

func CompileDir(bin, dir , output string) error {
	stat, err := os.Stat(dir)
	if err != nil {
		return err
	}

	if !stat.IsDir() {
		return fmt.Errorf("%s is not directory", dir)
	}

	files , err := ioutil.ReadDir(dir)
	for _, f := range files {
		if f.IsDir() {
			continue
		}
		if strings.HasSuffix(f.Name(), "_test.cpp") {
			CompileFile(bin, path.Join(dir, f.Name()), output)
		}
	}
	return nil
}

func CompileFile(binPath, filePath, outPath string) error {
	_, file := path.Split(filePath)
	output := path.Join(outPath, file+".wasm")
	bin := path.Join(binPath, "platon-cpp")
	//command := fmt.Sprintf("/home/yangzhou/platon/yangzhou/PlatON-CDT/build/bin/platon-cpp -std=c++1z %s -o %s", filePath, output)
	cmd := exec.Command(bin, []string{
		"-std=c++1z",
		filePath,
		"-o",
		output,
	}...)
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

