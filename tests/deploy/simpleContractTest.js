var contract = require("./contract");

async function createContractTest(){
    // deploy simple contract and create contract
    var simpleContract = await contract.deployContract("simpleContract");

    await contract.actionMethodSend("simpleContract", simpleContract, 'set', 0xffff0000);

    await contract.actionMethodSend("simpleContract", simpleContract, 'del');

    await contract.constMethodCall("simpleContract", simpleContract, 'get');
}

createContractTest();

