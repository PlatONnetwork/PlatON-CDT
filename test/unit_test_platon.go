package main

import (
	"bufio"
	"errors"
	"fmt"
	"io"
	"io/ioutil"
	"os"
	"os/exec"
	"path/filepath"
	"strings"

	wasm_exec "github.com/go-interpreter/wagon/exec"
	"github.com/go-interpreter/wagon/wasm"
)

func main() {
	// get current path
	dir_path, err := get_current_path()
	if nil != err {
		fmt.Printf("could not get current path: %v\n", err)
		os.Exit(1)
	}

	// get all test file
	unit_test_path := filepath.Join(dir_path, "unit")
	test_files, err := ioutil.ReadDir(unit_test_path)
	if nil != err {
		fmt.Println("read directory ./unit failed!!")
		os.Exit(1)
	}

	// statistics
	success_test_file := make([]string, 0)
	failed_test_file := make([]string, 0)

	// compile test file
	for _, one_test_file := range test_files {
		one_test_file_name := one_test_file.Name()
		if !strings.HasSuffix(one_test_file_name, ".cpp") {
			continue
		}

		// invoke
		bool_success := one_file_test(dir_path, one_test_file_name)
		if bool_success {
			success_test_file = append(success_test_file, one_test_file_name)
		} else {
			failed_test_file = append(failed_test_file, one_test_file_name)
		}
	}

	// report
	fmt.Println("###################platon_test###################")

	// success
	success_num := len(success_test_file)
	if 0 != success_num {
		fmt.Printf("success test case file:")
		for _, one_success := range success_test_file {
			fmt.Printf(" %s", one_success)
		}
		fmt.Printf("\n")

	}

	// failed
	failed_num := len(failed_test_file)
	if 0 != failed_num {
		fmt.Printf("failed test case file:")
		for _, one_failed := range failed_test_file {
			fmt.Printf(" %s", one_failed)
		}
		fmt.Printf("\n")
	}

	// total
	total_num := success_num + failed_num
	success_rate := float64(success_num) / float64(total_num)
	fmt.Printf("success number:%d, failed number:%d, total number:%d, success rate:%%%.3f\n",
		success_num, failed_num, total_num, success_rate*100)
}

func one_file_test(dir_path, one_test_file_name string) bool {
	fmt.Println("........................ test case " + one_test_file_name + " begin")
	compile_tool_path := filepath.Join(dir_path, "../build/bin/platon-cpp")
	one_test_file_path := filepath.Join(dir_path, "./unit", one_test_file_name)
	cmd := compile_tool_path + " " + one_test_file_path
	bool_result := exec_command(compile_tool_path, []string{one_test_file_path})
	if !bool_result {
		fmt.Printf("run command %s failed!!!\n", cmd)
		fmt.Println("........................ test case " + one_test_file_name + " end")
		return false
	}

	one_target_wasm_file_name := strings.Replace(one_test_file_name, ".cpp", ".wasm", -1)
	test_wasm_files, err := ioutil.ReadDir("./")
	if nil != err {
		fmt.Println("read directory ./ failed!!")
		os.Exit(1)
	}

	bool_find := false
	for _, one_test_wasm_file := range test_wasm_files {
		one_test_wasm_file_name := one_test_wasm_file.Name()
		if one_target_wasm_file_name == one_test_wasm_file_name {
			bool_find = true
			break
		}
	}

	if !bool_find {
		fmt.Printf("compile %s file failed!!!\n", one_test_file_name)
		fmt.Println("........................ test case " + one_test_file_name + " end")
		return false
	}
	fmt.Printf("compile %s file success!!!\n", one_test_file_name)
	one_target_wasm_file_path := filepath.Join(dir_path, one_target_wasm_file_name)
	defer os.Remove(one_target_wasm_file_path)

	// open wasm file
	wasm_file, err := os.Open(one_target_wasm_file_path)
	if nil != err {
		fmt.Printf("open wasm file %s failed!!! err=%v\n", one_target_wasm_file_name, err)
		fmt.Println("........................ test case " + one_test_file_name + " end")
		return false
	}
	defer wasm_file.Close()

	// read module
	wasm_module, err := wasm.ReadModule(wasm_file, nil)
	if err != nil {
		fmt.Printf("wasm file %s could not read module: %v\n", one_target_wasm_file_name, err)
		fmt.Println("........................ test case " + one_test_file_name + " end")
		return false
	}

	if wasm_module.Export == nil {
		fmt.Printf("module %s has no export section\n", one_target_wasm_file_name)
		fmt.Println("........................ test case " + one_test_file_name + " end")
		return false
	}

	// create vm
	vm, err := wasm_exec.NewVM(wasm_module)
	if err != nil {
		fmt.Printf("could not create VM: %v\n", err)
		fmt.Println("........................ test case " + one_test_file_name + " end")
		return false
	}

	// invoke
	export_invoke := wasm_module.Export.Entries["invoke"]
	index := int64(export_invoke.Index)
	fidx := wasm_module.Function.Types[int(index)]
	ftype := wasm_module.Types.Entries[int(fidx)]

	result, err := vm.ExecCode(index)
	if err != nil {
		fmt.Printf("execute code failed!!! err=%v\n", err)
		fmt.Println("........................ test case " + one_test_file_name + " end")
		return false
	}

	// result
	if len(ftype.ReturnTypes) == 0 {
		fmt.Printf("%s invoke do not have return value\n", one_target_wasm_file_name)
		fmt.Println("........................ test case " + one_test_file_name + " end")
		return false
	}

	return_code, bool_right := result.(uint32)
	if !bool_right {
		fmt.Printf("%s the return value is not uint32\n", one_target_wasm_file_name)
		fmt.Println("........................ test case " + one_test_file_name + " end")
		return false
	}

	fmt.Printf("run %s result is ", one_target_wasm_file_name)
	fmt.Printf("%[1]v (%[1]T)\n", result)

	if 0 != return_code {
		fmt.Printf("%s the return value is not 0, test failed!!!\n", one_test_file_name)
		fmt.Println("........................ test case " + one_test_file_name + " end")
		return false
	} else {
		fmt.Printf("%s test success!!!\n", one_test_file_name)
		fmt.Println("........................ test case " + one_test_file_name + " end")
		return true
	}
}

func exec_command(command_name string, params []string) bool {
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

func get_current_path() (string, error) {
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
