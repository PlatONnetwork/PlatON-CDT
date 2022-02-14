package main

import (
	"fmt"
	"github.com/urfave/cli"
	"os"
	"platon/core"
)
var (
	app    = cli.App{Usage: "wasm library test tools"}
)

func init()  {
	app.Commands = []cli.Command{
		core.CompileCmd,
		core.ExecCmd,
		core.TestCmd,
		core.ExecSolCmd,
	}
}
func main() {
	err :=app.Run(os.Args)
	if err != nil {
		fmt.Println(err)
	}
}

