package core

import (
	"bytes"
	"fmt"
	"reflect"
	"time"

	"github.com/PlatONnetwork/PlatON-Go/core/vm"
	"github.com/PlatONnetwork/wagon/exec"
	"github.com/PlatONnetwork/wagon/validate"
	"github.com/PlatONnetwork/wagon/wasm"
)

//
func addFuncExport(m *wasm.Module, sig wasm.FunctionSig, function wasm.Function, export wasm.ExportEntry) {
	typesLen := len(m.Types.Entries)
	m.Types.Entries = append(m.Types.Entries, sig)
	function.Sig = &m.Types.Entries[typesLen]
	funcLen := len(m.FunctionIndexSpace)
	m.FunctionIndexSpace = append(m.FunctionIndexSpace, function)
	export.Index = uint32(funcLen)
	m.Export.Entries[export.FieldStr] = export
}

func SetState(proc *exec.Process, key uint32, keyLen uint32, val uint32, valLen uint32) {
	//db := proc.HostCtx().(*VMContext).Db
	//keyBuf := make([]byte, keyLen)
	//proc.ReadAt(keyBuf, int64(key))
	//if valLen == 0 {
	//	db.Delete(string(keyBuf))
	//	return
	//}
	//valBuf := make([]byte, valLen)
	//proc.ReadAt(valBuf, int64(val))
	//db.Set(string(keyBuf), valBuf)
}

func DebugGas(proc *exec.Process, line uint32, dst uint32, len uint32) {
	buf := make([]byte, len)
	proc.ReadAt(buf, int64(dst))

	fmt.Println("debug gas:", "line:", line, "func:", string(buf), "gas:", initGas-contractCtx.Gas)
}

// int64_t platon_nano_time()
func PlatonNanoTime(proc *exec.Process) int64 {
	return time.Now().UnixNano()
}

func ReadWasmModule(Code []byte, verify bool) (*exec.CompiledModule, error) {
	host := vm.NewHostModule()
	AddHostModule(host)
	m, err := wasm.ReadModule(bytes.NewReader(Code), func(name string) (*wasm.Module, error) {
		switch name {
		case "env":
			return host, nil
		}
		return nil, fmt.Errorf("module %q unknown", name)
	})

	if err != nil {
		return nil, err
	}

	if verify {
		err = validate.VerifyModule(m)
		if err != nil {
			return nil, err
		}
	}

	compiled, err := exec.CompileModule(m)

	if err != nil {
		return nil, err
	}

	return compiled, nil
}

func AddHostModule(m *wasm.Module) {
	addFuncExport(m,
		wasm.FunctionSig{
			ParamTypes: []wasm.ValueType{wasm.ValueTypeI32, wasm.ValueTypeI32, wasm.ValueTypeI32},
		},
		wasm.Function{
			Host: reflect.ValueOf(DebugGas),
			Body: &wasm.FunctionBody{},
		},
		wasm.ExportEntry{
			FieldStr: "platon_debug_gas",
			Kind:     wasm.ExternalFunction,
		},
	)

	addFuncExport(m,
		wasm.FunctionSig{
			ReturnTypes: []wasm.ValueType{wasm.ValueTypeI64},
		},
		wasm.Function{
			Host: reflect.ValueOf(PlatonNanoTime),
			Body: &wasm.FunctionBody{},
		},
		wasm.ExportEntry{
			FieldStr: "platon_nano_time",
			Kind:     wasm.ExternalFunction,
		},
	)
}
