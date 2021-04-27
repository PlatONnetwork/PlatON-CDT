package main

import (
	"fmt"
	"os"

	"github.com/urfave/cli"
)

var (
	app = cli.App{Usage: "Calling tools across virtual machines"}
)

func init() {
	app.Commands = []cli.Command{
		CallWasmCmd,
		CallSolidityCmd,
	}
}

func main() {
	err := app.Run(os.Args)
	if err != nil {
		fmt.Println(err)
		os.Exit(1)
	}
}
