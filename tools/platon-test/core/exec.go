package core

import (
	"fmt"
	"io/ioutil"
	"os"
	"path"

	wasmExec "github.com/PlatONnetwork/wagon/exec"
	"github.com/PlatONnetwork/wagon/wasm"
	"github.com/urfave/cli"
)

var ExecCmd = cli.Command{
	Name:        "exec",
	Description: "exec wasm test file",
	Usage:       "exec wasm test file",
	Before:      HelpBefore("compile"),
	Action:      execTest,
	Flags:       execCmdFlags,
}

var execCmdFlags = []cli.Flag{
	WasmDirFlag,
	WasmFileFlag,
}

func execTest(c *cli.Context) error {
	dir := c.String(WasmDirFlag.Name)
	if dir != "" {
		return ExecDir(dir)
	}
	file := c.String(WasmFileFlag.Name)
	if file == "" {
		cli.ShowCommandHelp(c, "exec")
		return fmt.Errorf("command args error")
	}
	return ExecFile(file)
}

func ExecDir(dir string) error {
	stat, err := os.Stat(dir)
	if err != nil {
		return err
	}

	if !stat.IsDir() {
		return fmt.Errorf("%s is not directory", dir)
	}

	files, err := ioutil.ReadDir(dir)
	var success []string
	var failed []string
	for _, f := range files {
		if f.IsDir() {
			continue
		}
		if path.Ext(f.Name()) == ".wasm" {
			file := path.Join(dir, f.Name())
			if err := ExecFile(file); err != nil {
				failed = append(failed, f.Name())
				//fmt.Fprintln(os.Stderr, "........................ test case " + file + " end")
			} else {
				success = append(success, f.Name())
				//fmt.Fprintln(os.Stdin, "........................ test case "+file+" end")
			}
		}
	}
	if len(success) != 0 {
		fmt.Println("success test case file:")
		for _, name := range success {
			fmt.Printf("\t%s\n", name)
		}
	}

	if len(failed) != 0 {
		fmt.Println("failed test case file:")
		for _, name := range failed {
			fmt.Printf("\t%s\n", name)
		}
	}
	return nil
}

func ExecFile(filePath string) error {
	_, file := path.Split(filePath)
	fmt.Println("test", file)
	wasmFile, err := os.Open(filePath)
	if nil != err {
		fmt.Fprintf(os.Stderr, "open wasm file %s failed!!! err=%v\n", file, err)
		return err
	}
	defer wasmFile.Close()

	// read module
	wasmModule, err := wasm.ReadModule(wasmFile, importer)
	if err != nil {
		fmt.Fprintf(os.Stderr, "wasm file %s could not read module: %v\n", file, err)
		return err
	}

	if wasmModule.Export == nil {
		fmt.Fprintf(os.Stderr, "module %s has no export section\n", file)
		return err
	}

	// create vm
	vm, err := wasmExec.NewVM(wasmModule)
	if err != nil {
		fmt.Fprintf(os.Stderr, "could not create VM: %v\n", err)
		return err
	}

	// set gas
	gasCost := uint64(0)
	opCodes := uint64(0)
	vm.SetUseGas(func(b byte) {
		gasCost += WasmGasCostTable[b]
		opCodes++
	})

	// set context
	ctx := &VMContext{
		Input:   nil,
		CallOut: nil,
		Output:  nil,
		Gas:     &gasCost,
		OpCode:  &opCodes,
		Db:      NewDB(),
	}

	vm.SetHostCtx(ctx)

	// invoke
	exportInvoke := wasmModule.Export.Entries["invoke"]
	index := int64(exportInvoke.Index)
	fidx := wasmModule.Function.Types[int(index)]
	ftype := wasmModule.Types.Entries[int(fidx)]
	result, err := vm.ExecCode(index)

	// gas result
	fmt.Fprintf(os.Stdin, "gas cost:%d, opcodes:%d\n", gasCost, opCodes)
	if err != nil {
		fmt.Fprintf(os.Stderr, "execute code failed!!! err=%v\n", err)
		return err
	}

	// result
	if len(ftype.ReturnTypes) == 0 {
		fmt.Fprintf(os.Stderr, "%s invoke do not have return value\n", file)
		return err
	}

	returnCode, ok := result.(uint32)
	if !ok {
		fmt.Fprintf(os.Stderr, "%s the return value is not uint32\n", file)
		return err
	}

	if 0 != returnCode {
		fmt.Fprintf(os.Stderr, "%s the return value is not 0, test failed!!!\n", file)

		return fmt.Errorf("%s the return value is not 0, test failed!!!\n", file)
	} else {
		fmt.Fprintf(os.Stdin, "%s test success!!!\n", file)
		return nil
	}
}
