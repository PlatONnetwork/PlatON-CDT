package core

import (
	"bytes"
	"fmt"
	"github.com/PlatONnetwork/PlatON-Go/core/vm"
	"github.com/PlatONnetwork/wagon/exec"
	"github.com/PlatONnetwork/wagon/validate"
	"github.com/PlatONnetwork/wagon/wasm"
	"math/big"
	"reflect"
	"time"
)

//
//import (
//	"errors"
//	"fmt"
//	"math/big"
//	"reflect"
//	"time"
//)
//
//var (
//	ErrWASMRlpItemCountTooLarge = errors.New("WASM: itemCount too large for RLP")
//	importer                    = func(name string) (*wasm.Module, error) {
//		switch name {
//		case "env":
//			return NewHostModule(), nil
//		}
//		return nil, fmt.Errorf("module %q unknown", name)
//	}
//)
//

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
//
//func Debug(proc *exec.Process, dst uint32, len uint32) {
//	buf := make([]byte, len)
//	proc.ReadAt(buf, int64(dst))
//	fmt.Printf("%s", string(buf))
//}
//
//func Panic(proc *exec.Process) {
//	panic("test case panic")
//}
//
//func Revert(proc *exec.Process) {
//	proc.Terminate()
//}
//
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

	fmt.Println("debug gas:", "line:", line, "func:", string(buf), "gas:", initGas - contractCtx.Gas)
}

// int64_t platon_nano_time()
func PlatonNanoTime(proc *exec.Process) int64 {
	return time.Now().UnixNano()
}


// uint8_t bigint_binary_operators(uint8_t *left, bool left_negative, uint8_t *right, bool right_negative, uint8_t *result, size_t arr_size, uint32_t operators);
// func $bigint_binary_operators(param $0 i32) (param $1 i32) (param $2 i32) (param $3 i32) (param $4 i32) (param $5 i32) (param $6 i32) (result  i32)
type BinaryOperatorType uint32

const (
	BIGINTADD BinaryOperatorType = 0x01
	BIGINTSUB BinaryOperatorType = 0x02
	BIGINTMUL BinaryOperatorType = 0x04
	BIGINTDIV BinaryOperatorType = 0x08
	BIGINTMOD BinaryOperatorType = 0x10
	BIGINTAND BinaryOperatorType = 0x20
	BIGINTOR  BinaryOperatorType = 0x40
	BIGINTXOR BinaryOperatorType = 0x80
)

func BigintBinaryOperators(proc *exec.Process, left uint32, leftNegative uint32, right uint32, rightNegative uint32,
	result uint32, arrSize uint32, operators uint32) uint32 {
	// read left data
	dataBuf := make([]byte, arrSize)
	_, leftErr := proc.ReadAt(dataBuf, int64(left))
	if nil != leftErr {
		panic(leftErr)
	}

	// fmt.Println("BigintBinaryOperators left: ", dataBuf)

	leftOperand := new(big.Int)
	leftOperand.SetBytes(dataBuf)

	if leftNegative > 0 {
		bigValueTemp := new(big.Int)
		bigValueTemp.Neg(leftOperand)
		leftOperand = bigValueTemp
	}

	// read right data
	_, rightErr := proc.ReadAt(dataBuf, int64(right))
	if nil != rightErr {
		panic(rightErr)
	}

	// fmt.Println("BigintBinaryOperators right: ", dataBuf)

	rightOperand := new(big.Int)
	rightOperand.SetBytes(dataBuf)

	if rightNegative > 0 {
		bigValueTemp := new(big.Int)
		bigValueTemp.Neg(rightOperand)
		rightOperand = bigValueTemp
	}

	// binary operation
	operationResult := new(big.Int)
	switch BinaryOperatorType(operators) {
	case BIGINTADD:
		operationResult.Add(leftOperand, rightOperand)
	case BIGINTSUB:
		operationResult.Sub(leftOperand, rightOperand)
	case BIGINTMUL:
		operationResult.Mul(leftOperand, rightOperand)
	case BIGINTDIV:
		operationResult.Div(leftOperand, rightOperand)
	case BIGINTMOD:
		operationResult.Mod(leftOperand, rightOperand)
	case BIGINTAND:
		operationResult.And(leftOperand, rightOperand)
	case BIGINTOR:
		operationResult.Or(leftOperand, rightOperand)
	case BIGINTXOR:
		operationResult.Xor(leftOperand, rightOperand)
	}

	// fmt.Println(operators)

	// write result
	bytesResult := operationResult.Bytes()

	// fmt.Println("BigintBinaryOperators result: ", bytesResult, operationResult)

	// Clear to zero
	zeroResult := make([]byte, arrSize)
	_, clearErr := proc.WriteAt(zeroResult, int64(result))
	if nil != clearErr {
		panic(clearErr)
	}

	//Set to actual value
	lenResult := len(bytesResult)
	if lenResult > int(arrSize) {
		lenResult = int(arrSize)
	}
	result = result + arrSize - uint32(lenResult)

	_, setErr := proc.WriteAt(bytesResult, int64(result))
	if nil != setErr {
		panic(setErr)
	}

	var returnResult uint32 = 0

	if -1 == operationResult.Sign() {
		returnResult = 1
	}

	if len(bytesResult) > int(arrSize) {
		returnResult |= 0x02
	}

	return returnResult
}

// int bigint_cmp(uint8_t *left, uint8_t left_negative, uint8_t *right, uint8_t right_negative);
// func $bigint_cmp(param $0 i32) (param $1 i32) (param $2 i32) (param $3 i32) (result  i32)
func BigintCmp(proc *exec.Process, left uint32, leftNegative uint32, right uint32, rightNegative uint32, arrSize uint32) int32 {
	// read left data
	dataBuf := make([]byte, arrSize)
	_, leftErr := proc.ReadAt(dataBuf, int64(left))
	if nil != leftErr {
		panic(leftErr)
	}

	leftOperand := new(big.Int)
	leftOperand.SetBytes(dataBuf)

	if leftNegative > 0 {
		bigValueTemp := new(big.Int)
		bigValueTemp.Neg(leftOperand)
		leftOperand = bigValueTemp
	}

	// read right data
	_, rightErr := proc.ReadAt(dataBuf, int64(right))
	if nil != rightErr {
		panic(rightErr)
	}

	rightOperand := new(big.Int)
	rightOperand.SetBytes(dataBuf)

	if rightNegative > 0 {
		bigValueTemp := new(big.Int)
		bigValueTemp.Neg(rightOperand)
		rightOperand = bigValueTemp
	}

	// compare
	result := int32(leftOperand.Cmp(rightOperand))
	return result
}

// uint8_t bigint_sh(uint8_t *origin, uint8_t origin_negative, uint32_t n, uint8_t *result, size_t arr_size, uint32_t direction);
// func $bigint_sh(param $0 i32) (param $1 i32) (param $2 i32) (param $3 i32) (param $4 i32) (param $5 i32) (result i32)
type ShiftDirectionType uint32

const (
	LEFT  ShiftDirectionType = 0x01
	RIGHT ShiftDirectionType = 0x02
)

func BigintSh(proc *exec.Process, origin uint32, originNegative uint32, shiftNum uint32, result uint32, arrSize uint32, direction uint32) uint32 {
	// read origin data
	dataBuf := make([]byte, arrSize)
	_, originErr := proc.ReadAt(dataBuf, int64(origin))
	if nil != originErr {
		panic(originErr)
	}

	originOperand := new(big.Int)
	originOperand.SetBytes(dataBuf)

	if originNegative > 0 {
		bigValueTemp := new(big.Int)
		bigValueTemp.Neg(originOperand)
		originOperand = bigValueTemp
	}

	// shift
	operationResult := new(big.Int)
	switch ShiftDirectionType(direction) {
	case LEFT:
		operationResult.Lsh(originOperand, uint(shiftNum))
	case RIGHT:
		operationResult.Rsh(originOperand, uint(shiftNum))
	}

	// write result
	bytesResult := operationResult.Bytes()

	// Clear to zero
	zeroResult := make([]byte, arrSize)
	_, clearErr := proc.WriteAt(zeroResult, int64(result))
	if nil != clearErr {
		panic(clearErr)
	}

	//Set to actual value
	lenResult := len(bytesResult)
	if lenResult > int(arrSize) {
		lenResult = int(arrSize)
	}
	result = result + arrSize - uint32(lenResult)

	_, setErr := proc.WriteAt(bytesResult, int64(result))
	if nil != setErr {
		panic(setErr)
	}

	var returnResult uint32 = 0

	if -1 == operationResult.Sign() {
		returnResult = 1
	}

	if len(bytesResult) > int(arrSize) {
		returnResult |= 0x02
	}

	return returnResult
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

	// uint8_t bigint_binary_operators(uint8_t *left, bool left_negative, uint8_t *right, bool right_negative, uint8_t *result, size_t arr_size, uint32_t operators);
	// func $bigint_binary_operators(param $0 i32) (param $1 i32) (param $2 i32) (param $3 i32) (param $4 i32) (param $5 i32) (param $6 i32) (result  i32)
	addFuncExport(m,
		wasm.FunctionSig{
			ParamTypes:  []wasm.ValueType{wasm.ValueTypeI32, wasm.ValueTypeI32, wasm.ValueTypeI32, wasm.ValueTypeI32, wasm.ValueTypeI32, wasm.ValueTypeI32, wasm.ValueTypeI32},
			ReturnTypes: []wasm.ValueType{wasm.ValueTypeI32},
		},
		wasm.Function{
			Host: reflect.ValueOf(BigintBinaryOperators),
			Body: &wasm.FunctionBody{},
		},
		wasm.ExportEntry{
			FieldStr: "bigint_binary_operators",
			Kind:     wasm.ExternalFunction,
		},
	)

	// int bigint_cmp(uint8_t *left, uint8_t left_negative, uint8_t *right, uint8_t right_negative, size_t arr_size);
	// func $bigint_cmp(param $0 i32) (param $1 i32) (param $2 i32) (param $3 i32) (param $4 i32) (result i32)
	addFuncExport(m,
		wasm.FunctionSig{
			ParamTypes:  []wasm.ValueType{wasm.ValueTypeI32, wasm.ValueTypeI32, wasm.ValueTypeI32, wasm.ValueTypeI32, wasm.ValueTypeI32},
			ReturnTypes: []wasm.ValueType{wasm.ValueTypeI32},
		},
		wasm.Function{
			Host: reflect.ValueOf(BigintCmp),
			Body: &wasm.FunctionBody{},
		},
		wasm.ExportEntry{
			FieldStr: "bigint_cmp",
			Kind:     wasm.ExternalFunction,
		},
	)

	// uint8_t bigint_sh(uint8_t *origin, uint8_t origin_negative, uint32_t n, uint8_t *result, size_t arr_size, uint32_t direction);
	// func $bigint_sh(param $0 i32) (param $1 i32) (param $2 i32) (param $3 i32) (param $4 i32) (param $5 i32) (result i32)
	addFuncExport(m,
		wasm.FunctionSig{
			ParamTypes:  []wasm.ValueType{wasm.ValueTypeI32, wasm.ValueTypeI32, wasm.ValueTypeI32, wasm.ValueTypeI32, wasm.ValueTypeI32, wasm.ValueTypeI32},
			ReturnTypes: []wasm.ValueType{wasm.ValueTypeI32},
		},
		wasm.Function{
			Host: reflect.ValueOf(BigintSh),
			Body: &wasm.FunctionBody{},
		},
		wasm.ExportEntry{
			FieldStr: "bigint_sh",
			Kind:     wasm.ExternalFunction,
		},
	)
}
