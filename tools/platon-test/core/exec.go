package core

import (
	"context"
	"errors"
	"fmt"
	"github.com/AlayaNetwork/Alaya-Go/common"
	wvm "github.com/AlayaNetwork/Alaya-Go/core/vm"
	"github.com/AlayaNetwork/Alaya-Go/log"
	"github.com/AlayaNetwork/Alaya-Go/params"
	"github.com/PlatONnetwork/wagon/exec"
	"github.com/urfave/cli"
	"io/ioutil"
	"math/big"
	"os"
	"path"
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

const memoryLimit = 16 * 1014 * 1024

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

type testContract struct{}

func (testContract) Address() common.Address {
	return common.Address{}
}

var contractCtx *wvm.Contract

var initGas = uint64(1000000000)

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
	code := make([]byte, 1024*1024)
	size, _ := wasmFile.Read(code)
	// read module
	wasmModule, err := ReadWasmModule(code[:size], false)
	if err != nil {
		fmt.Fprintf(os.Stderr, "wasm file %s could not read module: %v\n", file, err)
		return err
	}

	// create vm
	vm, err := exec.NewVMWithCompiled(wasmModule, memoryLimit)
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
	db := NewMockStateDB()
	contractCtx = wvm.NewContract(&testContract{}, &testContract{}, big.NewInt(0), initGas)

	ct := wvm.Context{
		CanTransfer: func(db wvm.StateDB, address common.Address, b *big.Int) bool {
			return true
		},
		Transfer: func(db wvm.StateDB, address common.Address, address2 common.Address, b *big.Int) {

		},
		GetHash: func(u uint64) common.Hash {
			return common.Hash{}
		},
		GasPrice : big.NewInt(1), Time : big.NewInt(1), BlockNumber: big.NewInt(1),
	}
	evm := wvm.NewEVM(ct, nil, db, &params.ChainConfig{}, wvm.Config{})
	evm.Ctx = context.Background()
	ctx := wvm.NewVMContext(evm, contractCtx, wvm.Config{}, db)

	logger := log.WasmRoot()
	logger.SetHandler(log.LvlFilterHandler(log.LvlDebug,
		log.StreamHandler(os.Stdout, log.FormatFunc(func(r *log.Record) []byte {
			return []byte(r.Msg)
		}))))
	ctx.Log = wvm.NewWasmLogger(wvm.Config{Debug: true}, logger)

	vm.SetHostCtx(ctx)

	// invoke
	exportInvoke := wasmModule.RawModule.Export.Entries["invoke"]
	index := int64(exportInvoke.Index)
	rtrns, err := vm.ExecCode(index)

	// gas result
	fmt.Fprintf(os.Stdin, "gas cost:%d, opcodes:%d\n", initGas - contractCtx.Gas, opCodes)
	if err != nil {
		fmt.Fprintf(os.Stderr, "execute code failed!!! err=%v\n", err)
		return err
	}

	// result
	if res, ok := rtrns.(uint32); ok && 0 != res {
		info := fmt.Sprintf("%s has %d failed assertions\n", file, res)
		fmt.Fprintf(os.Stderr, info)
		return errors.New(info)
	}
	return nil
}
