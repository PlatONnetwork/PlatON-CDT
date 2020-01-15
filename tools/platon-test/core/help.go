package core

import (
	"fmt"
	"github.com/urfave/cli"
)

func HelpBefore(name string) func(c *cli.Context) error {
	return func(c *cli.Context) error {
		if c.BoolT("help") {
			cli.ShowCommandHelp(c, name)
			return fmt.Errorf("before help")
		}
		return nil
	}
}
