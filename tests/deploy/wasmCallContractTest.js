var contract = require("./contract");
var assert = require('assert');

async function wasmCallContractTest(){
    // deploy simple contract and create contract
    var wasmCallContract = await contract.deployContract(true, "wasmCall");

    await contract.actionMethodSend(true, "wasmCall", wasmCallContract, 'set', 0xffff0000);

    var setNum = await contract.constMethodCall(true, "wasmCall", wasmCallContract, 'get');

    assert.equal(setNum, 0xffff0000, 'wasmCall Contract method get assertion failed!!!');

    var wasmCallerContract = await contract.deployContract(false, "wasmCaller");

    await contract.actionMethodSend(false, "wasmCaller", wasmCallerContract, 'callSet', wasmCallContract, 64);

    setNum = await contract.constMethodCall(false, "wasmCaller", wasmCallerContract, 'callGet', wasmCallContract);

    assert.equal(setNum, 64, 'wasmCaller Contract method callGet assertion failed!!!');

    setNum = await contract.constMethodCall(true, "wasmCall", wasmCallContract, 'get');

    assert.equal(setNum, 64, 'wasmCall Contract method get assertion failed!!!');

    await contract.actionMethodSend(false, "wasmCaller", wasmCallerContract, 'callSetInt', wasmCallContract, -12);

    setNum = await contract.constMethodCall(false, "wasmCaller", wasmCallerContract, 'callGetInt', wasmCallContract);

    assert.equal(setNum, -12, 'wasmCaller Contract method callGetInt assertion failed!!!');

    setNum = await contract.constMethodCall(true, "wasmCall", wasmCallContract, 'get_int');

    assert.equal(setNum, -12, 'wasmCall Contract method get_int assertion failed!!!');

    await contract.actionMethodSend(false, "wasmCaller", wasmCallerContract, 'callSetString', wasmCallContract, "jatel");

    var setStr = await contract.constMethodCall(false, "wasmCaller", wasmCallerContract, 'callGetString', wasmCallContract);

    assert.ok(JSON.stringify(setStr) === JSON.stringify("jatel"), 'wasmCaller Contract method callGetString assertion failed!!!');

    setStr = await contract.constMethodCall(true, "wasmCall", wasmCallContract, 'get_string');

    assert.ok(JSON.stringify(setStr) === JSON.stringify("jatel"), 'wasmCall Contract method get_string assertion failed!!!');

    await contract.actionMethodSend(false, "wasmCaller", wasmCallerContract, 'callSetInfo', wasmCallContract, 456);

    setStr = await contract.constMethodCall(false, "wasmCaller", wasmCallerContract, 'callGetInfo', wasmCallContract);

    assert.ok(JSON.stringify(setStr) === JSON.stringify(wasmCallContract), 'wasmCaller Contract method callGetInfo assertion failed!!!');

    var setInfo = await contract.constMethodCall(true, "wasmCall", wasmCallContract, 'get_info');

    assert.equal(setInfo[0], 456, 'wasmCall Contract method get_info assertion failed!!!');

    assert.ok(JSON.stringify(setInfo[1]) === JSON.stringify(wasmCallContract), 'wasmCall Contract method get_info assertion failed!!!');

    await contract.actionMethodSend(false, "wasmCaller", wasmCallerContract, 'set_my_vector', wasmCallContract, 64, "jatel");

    setStr = await contract.constMethodCall(false, "wasmCaller", wasmCallerContract, 'get_my_vector_string', wasmCallContract);

    assert.ok(JSON.stringify(setStr) === JSON.stringify("jatel"), 'wasmCaller Contract method get_my_vector_string assertion failed!!!');

    setNum = await contract.constMethodCall(false, "wasmCaller", wasmCallerContract, 'get_my_vector_uint', wasmCallContract);

    assert.equal(setNum, 65, 'wasmCaller Contract method get_my_vector_uint assertion failed!!!');

    setNum = await contract.constMethodCall(false, "wasmCaller", wasmCallerContract, 'get_my_vector_int', wasmCallContract);

    assert.equal(setNum, 66, 'wasmCaller Contract method get_my_vector_int assertion failed!!!');
}

exports.wasmCallContractTest = wasmCallContractTest;