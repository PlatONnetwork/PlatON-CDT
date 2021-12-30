package core

import (
	"encoding/hex"
	"fmt"
	"github.com/AlayaNetwork/Alaya-Go/common"
	"github.com/AlayaNetwork/Alaya-Go/common/hexutil"
	"github.com/AlayaNetwork/Alaya-Go/core"
	"github.com/AlayaNetwork/Alaya-Go/core/snapshotdb"
	"github.com/AlayaNetwork/Alaya-Go/core/types"
	wvm "github.com/AlayaNetwork/Alaya-Go/core/vm"
	"github.com/AlayaNetwork/Alaya-Go/crypto"
	"github.com/AlayaNetwork/Alaya-Go/params"
	"github.com/urfave/cli"
	"io/ioutil"
	"math/big"
	"os"
	"path"
	"strings"
	"time"
)

var ExecSolCmd = cli.Command{
	Name:        "sol",
	Description: "exec solidity test file",
	Usage:       "exec solidity test file",
	Before:      HelpBefore("compile"),
	Action:      execSolTest,
	Flags:       execSolCmdFlags,
}

var execSolCmdFlags = []cli.Flag{
	SolFileFlag,
}

func execSolTest(c *cli.Context) error {
	file := c.String(SolFileFlag.Name)
	if file == "" {
		cli.ShowCommandHelp(c, "sol")
		return fmt.Errorf("command args error")
	}
	return ExecSolFile(file)
}

var (
	from   = common.BigToAddress(big.NewInt(102))
	header = &types.Header{
		Number: big.NewInt(1),
		Time:   uint64(time.Now().Unix()),
		Extra:  make([]byte, 33),
	}
	gasLimit       = uint64(100000000)
	gasPrice       = big.NewInt(100000)
	invokeSig      = hexutil.MustDecode("0xcab7f521") //invoke()
	defalutBalance = big.NewInt(int64(gasLimit) * gasPrice.Int64()*10)
	statedb        = NewMockStateDB()
)

func ExecSolFile(filePath string) error {
	_, file := path.Split(filePath)
	fmt.Println("test", file)
	sol, err := ioutil.ReadFile(filePath)
	if nil != err {
		fmt.Fprintf(os.Stderr, "open solidity file %s failed!!! err=%v\n", file, err)
		return err
	}
	code, err := hex.DecodeString(strings.TrimSpace(string(sol)))
	if err != nil {
		fmt.Fprintf(os.Stderr, "solidity file %s decode failed!!! err=%v\n", file, err)
		return err
	}

	statedb.AddBalance(from, defalutBalance)

	address, err := deploySol(code, statedb)
	if err != nil {
		fmt.Fprintf(os.Stderr, "solidity file %s deploy failed!!! err=%v\n", file, err)
		return err
	}

	msg := types.NewMessage(from, &address, 1, big.NewInt(0), gasLimit, gasPrice, invokeSig, false)

	context := core.NewEVMContext(msg, header, nil)
	context.GetHash = func(u uint64) common.Hash {
		return common.Hash{}
	}

	// Create a new environment which holds all relevant information
	// about the transaction and calling mechanisms.
	vmenv := wvm.NewEVM(context, snapshotdb.Instance(), statedb, params.TestChainConfig, wvm.Config{})

	gp := new(core.GasPool).AddGas(gasLimit)
	// Apply the transaction to the current state (included in the env)
	result, err := core.ApplyMessage(vmenv, msg, gp)
	if err != nil {
		fmt.Fprintf(os.Stderr, "solidity file %s exec failed!!! err=%v\n", file, err)
		return err
	}
	fmt.Fprintf(os.Stdout, "gas cost:%d, return:[%s]\n", result.UsedGas, hex.EncodeToString(result.ReturnData))
	return nil
}

func deploySol(code []byte, statedb *MockStateDB) (common.Address, error) {
	msg := types.NewMessage(from, nil, 0, big.NewInt(0), gasLimit, gasPrice, code, false)

	context := core.NewEVMContext(msg, header, nil)
	context.GetHash = func(u uint64) common.Hash {
		return common.Hash{}
	}

	// Create a new environment which holds all relevant information
	// about the transaction and calling mechanisms.
	vmenv := wvm.NewEVM(context, snapshotdb.Instance(), statedb, params.TestChainConfig, wvm.Config{})

	gp := new(core.GasPool).AddGas(gasLimit)
	_, err := core.ApplyMessage(vmenv, msg, gp)
	if err != nil {
		return common.Address{}, err
	}

	return crypto.CreateAddress(from, 0), nil
}
