var contract = require("./contract");
var createContractTest = require("./createContractTest");

async function contractTest(){
    await createContractTest.createContractTest();
}

async function runTest() {
    try{
        await contractTest()
    } catch(e){
        console.log("error:", e.message);
    }
}

runTest();