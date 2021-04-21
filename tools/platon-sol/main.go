package main

import (
	"flag"
	"fmt"
	"io/ioutil"
	"os"
	"path"
	"platon/abi"
	"platon/parse"
)


func main() {
	var abiFile string
	var output string
	var namespace string

	flag.StringVar(&abiFile, "abi", "", "solidity abi file path")
	flag.StringVar(&output, "output", "", "wasm contract output path")
	flag.StringVar(&namespace, "namespace", "", "wasm contract cpp namespace, default abi file name")
	flag.Parse()

	stream, err := ioutil.ReadFile(abiFile)
	if err != nil {
		fmt.Println(err)
		os.Exit(0)
	}


	if len(namespace) == 0 {
		namespace = pathToNamespace(abiFile)
	}

	output = createOutputPath(abiFile, output)

	abiTypes, err := abi.NewABI(string(stream))
	if err != nil {
		fmt.Println(err)
		os.Exit(0)
	}

	content, err := parse.Generate(abiTypes, namespace)
	if err != nil {
		fmt.Println(err)
		os.Exit(0)
	}
	if err := writeFile(content, output); err != nil {
		fmt.Println(err)
		os.Exit(0)
	}
	return
}

func pathToNamespace(filepath string) string {
	filenameAll := path.Base(filepath)
	fileSuffix := path.Ext(filepath)
	return filenameAll[0 : len(filenameAll)-len(fileSuffix)]
}

func createOutputPath(abi, output string) string {
	if len(output) == 0 {
		output = "./"
	}
	suffix := path.Ext(output)
	if len(suffix) != 0 {
		return output
	}
	base := pathToNamespace(abi)
	return path.Join(output, base+ ".hpp")
}

func writeFile(content, output string) error {
	return ioutil.WriteFile(output, []byte(content), 0666)
}
