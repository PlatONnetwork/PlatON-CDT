package main

import (
	"bufio"
	"errors"
	"fmt"
	"io"
	"io/ioutil"
	"os"
	"os/exec"
	"path"
	"path/filepath"
	"strings"

	wasmExec "github.com/go-interpreter/wagon/exec"
	"github.com/go-interpreter/wagon/wasm"
)

func main() {
	// get test file
	testFile := os.Args[1]

	// get current path
	dirPath, err := getCurrentPath()
	if nil != err {
		fmt.Printf("could not get current path: %v\n", err)
		os.Exit(1)
	}

	// get all test file
	unitTestFileOrPath := filepath.Join(dirPath, testFile)
	unitTestStat, err := os.Stat(unitTestFileOrPath)
	if nil != err {
		fmt.Println("Invalid parameter:", testFile)
		os.Exit(1)
	}

	// directory
	if unitTestStat.IsDir() {
		// statistics
		successTestFile := make([]string, 0)
		failedTestFile := make([]string, 0)

		testFiles, err := ioutil.ReadDir(unitTestFileOrPath)
		if nil != err {
			fmt.Println("read directory ./unit failed!!")
			os.Exit(1)
		}

		// compile test file
		for _, oneTestFile := range testFiles {
			oneTestFileName := oneTestFile.Name()
			if !strings.HasSuffix(oneTestFileName, "_test.cpp") {
				continue
			}

			// invoke
			boolSuccess := oneFileTest(dirPath, unitTestFileOrPath, oneTestFileName)
			if boolSuccess {
				successTestFile = append(successTestFile, oneTestFileName)
			} else {
				failedTestFile = append(failedTestFile, oneTestFileName)
			}
		}

		// report
		fmt.Println("###################platon_test###################")

		// success
		successNum := len(successTestFile)
		if 0 != successNum {
			fmt.Printf("success test case file:")
			for _, oneSuccess := range successTestFile {
				fmt.Printf(" %s", oneSuccess)
			}
			fmt.Printf("\n")
		}

		// failed
		failedNum := len(failedTestFile)
		if 0 != failedNum {
			fmt.Printf("failed test case file:")
			for _, oneFailed := range failedTestFile {
				fmt.Printf(" %s", oneFailed)
			}
			fmt.Printf("\n")
		}

		// total
		totalNum := successNum + failedNum
		successRate := float64(successNum) / float64(totalNum)
		fmt.Printf("success number:%d, failed number:%d, total number:%d, success rate:%%%.3f\n",
			successNum, failedNum, totalNum, successRate*100)
	} else {
		oneFileTest(dirPath, path.Dir(unitTestFileOrPath), path.Base(unitTestFileOrPath))
	}
}

func oneFileTest(dirPath, unitTestFileOrPath, oneTestFileName string) bool {
	fmt.Println("........................ test case " + oneTestFileName + " begin")
	compileToolPath := filepath.Join(dirPath, "platon-cpp")
	oneTestFilePath := filepath.Join(unitTestFileOrPath, oneTestFileName)
	cmd := compileToolPath + " " + oneTestFilePath
	bool_result := execCommand(compileToolPath, []string{oneTestFilePath})
	if !bool_result {
		fmt.Printf("run command %s failed!!!\n", cmd)
		fmt.Println("........................ test case " + oneTestFileName + " end")
		return false
	}

	oneTargetWasmFileName := strings.Replace(oneTestFileName, ".cpp", ".wasm", -1)
	testWasmFiles, err := ioutil.ReadDir(dirPath)
	if nil != err {
		fmt.Println("read directory ./ failed!!")
		os.Exit(1)
	}

	boolFind := false
	for _, oneTestWasmFile := range testWasmFiles {
		oneTestWasmFileName := oneTestWasmFile.Name()
		if oneTargetWasmFileName == oneTestWasmFileName {
			boolFind = true
			break
		}
	}

	if !boolFind {
		fmt.Printf("compile %s file failed!!!\n", oneTestFileName)
		fmt.Println("........................ test case " + oneTestFileName + " end")
		return false
	}
	fmt.Printf("compile %s file success!!!\n", oneTestFileName)
	oneTargetWasmFilePath := filepath.Join(dirPath, oneTargetWasmFileName)
	defer os.Remove(oneTargetWasmFilePath)

	// open wasm file
	wasmFile, err := os.Open(oneTargetWasmFilePath)
	if nil != err {
		fmt.Printf("open wasm file %s failed!!! err=%v\n", oneTargetWasmFileName, err)
		fmt.Println("........................ test case " + oneTestFileName + " end")
		return false
	}
	defer wasmFile.Close()

	// read module
	wasmModule, err := wasm.ReadModule(wasmFile, importer)
	if err != nil {
		fmt.Printf("wasm file %s could not read module: %v\n", oneTargetWasmFileName, err)
		fmt.Println("........................ test case " + oneTestFileName + " end")
		return false
	}

	if wasmModule.Export == nil {
		fmt.Printf("module %s has no export section\n", oneTargetWasmFileName)
		fmt.Println("........................ test case " + oneTestFileName + " end")
		return false
	}

	// create vm
	vm, err := wasmExec.NewVM(wasmModule)
	if err != nil {
		fmt.Printf("could not create VM: %v\n", err)
		fmt.Println("........................ test case " + oneTestFileName + " end")
		return false
	}

	// invoke
	exportInvoke := wasmModule.Export.Entries["invoke"]
	index := int64(exportInvoke.Index)
	fidx := wasmModule.Function.Types[int(index)]
	ftype := wasmModule.Types.Entries[int(fidx)]

	result, err := vm.ExecCode(index)
	if err != nil {
		fmt.Printf("execute code failed!!! err=%v\n", err)
		fmt.Println("........................ test case " + oneTestFileName + " end")
		return false
	}

	// result
	if len(ftype.ReturnTypes) == 0 {
		fmt.Printf("%s invoke do not have return value\n", oneTargetWasmFileName)
		fmt.Println("........................ test case " + oneTestFileName + " end")
		return false
	}

	returnCode, boolRight := result.(uint32)
	if !boolRight {
		fmt.Printf("%s the return value is not uint32\n", oneTargetWasmFileName)
		fmt.Println("........................ test case " + oneTestFileName + " end")
		return false
	}

	fmt.Printf("run %s result is ", oneTargetWasmFileName)
	fmt.Printf("%[1]v (%[1]T)\n", result)

	if 0 != returnCode {
		fmt.Printf("%s the return value is not 0, test failed!!!\n", oneTestFileName)
		fmt.Println("........................ test case " + oneTestFileName + " end")
		return false
	} else {
		fmt.Printf("%s test success!!!\n", oneTestFileName)
		fmt.Println("........................ test case " + oneTestFileName + " end")
		return true
	}
}

func execCommand(command_name string, params []string) bool {
	cmd := exec.Command(command_name, params...)

	stdout, err := cmd.StdoutPipe()

	if err != nil {
		fmt.Println(err)
		return false
	}

	cmd.Start()
	reader := bufio.NewReader(stdout)

	for {
		line, err2 := reader.ReadString('\n')
		if err2 != nil || io.EOF == err2 {
			break
		}
		fmt.Println(line)
	}

	cmd.Wait()
	return true
}

func getCurrentPath() (string, error) {
	file, err := exec.LookPath(os.Args[0])
	if err != nil {
		return "", err
	}
	path, err := filepath.Abs(file)
	if err != nil {
		return "", err
	}
	i := strings.LastIndex(path, "/")
	if i < 0 {
		i = strings.LastIndex(path, "\\")
	}
	if i < 0 {
		return "", errors.New(`error: Can't find "/" or "\".`)
	}
	return string(path[0 : i+1]), nil
}
