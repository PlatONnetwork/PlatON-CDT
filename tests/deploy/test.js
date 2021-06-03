var contract = require("./contract");
var createContractTest = require("./createContractTest");
var wasmCallContractTest = require("./wasmCallContractTest");

async function contractTest(){
    await createContractTest.createContractTest();
    await wasmCallContractTest.wasmCallContractTest();
}

async function runTest() {
    try{
        await contractTest()
    } catch(e){
        console.log("error:", e.message);
    }
}

runTest();