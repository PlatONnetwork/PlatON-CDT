var contract = require("./contract");
var assert = require('assert');


async function wasmCallContractTest(){
    // deploy wasmCall wasm contract
    var wasmCallContract = await contract.deployContract(true, "wasmCall");

    // deploy wasmCaller solidity contract
    var wasmCallerContract = await contract.deployContract(false, "wasmCaller");

    // uint type test
    await contract.actionMethodSend(false, "wasmCaller", wasmCallerContract, 'callSetUint', wasmCallContract, 64);

    setNum = await contract.constMethodCall(false, "wasmCaller", wasmCallerContract, 'callGetUint', wasmCallContract);

    assert.equal(setNum, 64, 'wasmCaller Contract method callGet assertion failed!!!');

    setNum = await contract.constMethodCall(true, "wasmCall", wasmCallContract, 'get_uint');

    assert.equal(setNum, 64, 'wasmCall Contract method get assertion failed!!!');

    // int type test
    await contract.actionMethodSend(false, "wasmCaller", wasmCallerContract, 'callSetInt', wasmCallContract, -12);

    setNum = await contract.constMethodCall(false, "wasmCaller", wasmCallerContract, 'callGetInt', wasmCallContract);

    assert.equal(setNum, -12, 'wasmCaller Contract method callGetInt assertion failed!!!');

    setNum = await contract.constMethodCall(true, "wasmCall", wasmCallContract, 'get_int');

    assert.equal(setNum, -12, 'wasmCall Contract method get_int assertion failed!!!');

    // string type test
    await contract.actionMethodSend(false, "wasmCaller", wasmCallerContract, 'callSetString', wasmCallContract, "jatel");

    var setStr = await contract.constMethodCall(false, "wasmCaller", wasmCallerContract, 'callGetString', wasmCallContract);

    assert.ok(JSON.stringify(setStr) === JSON.stringify("jatel"), 'wasmCaller Contract method callGetString assertion failed!!!');

    setStr = await contract.constMethodCall(true, "wasmCall", wasmCallContract, 'get_string');

    assert.ok(JSON.stringify(setStr) === JSON.stringify("jatel"), 'wasmCall Contract method get_string assertion failed!!!');

    // address type test
    await contract.actionMethodSend(false, "wasmCaller", wasmCallerContract, 'callSetAddress', wasmCallContract, wasmCallerContract);

    var setAddress = await contract.constMethodCall(false, "wasmCaller", wasmCallerContract, 'callGetAddress', wasmCallContract);

    // assert.ok(JSON.stringify(setAddress) === JSON.stringify(wasmCallerContract), 'wasmCaller Contract method callGetString assertion failed!!!');

    setAddress = await contract.constMethodCall(true, "wasmCall", wasmCallContract, 'get_address');

    // assert.ok(JSON.stringify(setAddress) === JSON.stringify(wasmCallerContract), 'wasmCall Contract method get_string assertion failed!!!');

    // derived type test
    await contract.actionMethodSend(false, "wasmCaller", wasmCallerContract, 'callSetInfo', wasmCallContract, [123, "jatel"]);

    var setInfo = await contract.constMethodCall(false, "wasmCaller", wasmCallerContract, 'callGetInfo', wasmCallContract);

    // assert.equal(setInfo[0], 123, 'wasmCall Contract method get_info assertion failed!!!');

    // assert.ok(JSON.stringify(setInfo[1]) === JSON.stringify(wasmCallContract), 'wasmCall Contract method get_info assertion failed!!!');

    setInfo = await contract.constMethodCall(true, "wasmCall", wasmCallContract, 'get_info');

    assert.equal(setInfo[0], 123, 'wasmCall Contract method get_info assertion failed!!!');

    assert.ok(JSON.stringify(setInfo[1]) === JSON.stringify("jatel"), 'wasmCall Contract method get_info assertion failed!!!');

    // type my vector test
    var vector_para =  [[[1, 2, 3]], [[4, 5, "jatel"]], [[["6"], "7", "8"]], [[9, 10, 11, 12, 13]], [[[[14, 15]], [[16, 17]]]], [[[18, "jatel"]], [[19, "jatel"]]], [[[20, [["21"], "22", "23"]]]], [[[24, ["jatel"]]]]]
    await contract.actionMethodSend(false, "wasmCaller", wasmCallerContract, 'callSetMyVector', wasmCallContract, vector_para);

    var vector_result = await contract.constMethodCall(false, "wasmCaller", wasmCallerContract, 'callGetMyVector', wasmCallContract);

    for(var i = 0, len = vector_result.length; i < len; i++){
        console.log(vector_result[i])

        if(vector_result[i] instanceof Array) {
            vector_result[i].forEach(function(item) {
                console.log(item);

                if(item instanceof Array){
                    item.forEach(function(one) {
                        console.log(one);
                    })
                }
            })
        }
    }

    // setInfo = await contract.constMethodCall(true, "wasmCall", wasmCallContract, 'get_my_vector');
}

exports.wasmCallContractTest = wasmCallContractTest;