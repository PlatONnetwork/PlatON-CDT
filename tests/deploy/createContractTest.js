var contract = require("./contract");
var assert = require('assert');

async function createContractTest(){
    // deploy simple contract and create contract
    var simpleContract = await contract.deployContract("simpleContract");
    var createContract = await contract.deployContract("createContract");
    var simpleLength = await contract.constMethodCall("createContract", createContract, 'get_contract_length', simpleContract);

    // test simple contract
    await contract.actionMethodSend("simpleContract", simpleContract, 'set_address', createContract);
    var simpleCreateAddress = await contract.constMethodCall("simpleContract", simpleContract, 'get_address');
    assert.ok(JSON.stringify(simpleCreateAddress) === JSON.stringify(createContract), 'simple Contract method set_address assertion failed!!!');

    // test create contract
    await contract.actionMethodSend("createContract", createContract, 'set_simple_address', simpleContract);
    var createSimpleAddress = await contract.constMethodCall("createContract", createContract, 'get_simple_address');
    assert.ok(JSON.stringify(createSimpleAddress) === JSON.stringify(simpleContract), 'create Contract method set_simple_address assertion failed!!!');

    // deploy simple contract
    await contract.actionMethodSend("createContract", createContract, 'deploy_contract', simpleContract);
    var deploySimple = await contract.constMethodCall("createContract", createContract, 'get_deploy_address');

    // get deploy simple contract code length
    var deployLength = await contract.constMethodCall("createContract", createContract, 'get_contract_length', deploySimple);
    assert.equal(deployLength, simpleLength, 'createContract Contract method deploy_contract assertion failed!!!');

    // test deploy simple contract
    await contract.actionMethodSend("simpleContract", deploySimple, 'set', 7);
    var deploySimpleGet = await contract.constMethodCall("simpleContract", deploySimple, 'get');
    assert.equal(deploySimpleGet, 7, 'deploySimple Contract method get assertion failed!!!');

    // clone simple contract
    await contract.actionMethodSend("createContract", createContract, 'clone_contract', simpleContract);
    var cloneSimple = await contract.constMethodCall("createContract", createContract, 'get_clone_address');

    // get clone simple contract code length
    var cloneLength = await contract.constMethodCall("createContract", createContract, 'get_contract_length', cloneSimple);
    assert.equal(cloneLength, simpleLength, 'createContract Contract method deploy_contract assertion failed!!!');

    // test clone simple contract
    await contract.actionMethodSend("simpleContract", cloneSimple, 'set', 7);
    var cloneSimpleGet = await contract.constMethodCall("simpleContract", cloneSimple, 'get');
    assert.equal(cloneSimpleGet, 7, 'cloneSimple Contract method get assertion failed!!!');

    // migrate test
    var mograteCreateContract = await contract.deployContract("createContract");
    await contract.actionMethodSend("createContract", mograteCreateContract, 'set_simple_address', simpleContract);
    await contract.actionMethodSend("createContract", mograteCreateContract, 'migrate', simpleContract);
    simpleCreateAddress = await contract.constMethodCall("simpleContract", simpleContract, 'get_address');

    // test migrate simple contract
    await contract.actionMethodSend("simpleContract", simpleCreateAddress, 'set', 7);
    var migrateSimpleGet = await contract.constMethodCall("simpleContract", simpleCreateAddress, 'get');
    assert.equal(migrateSimpleGet, 7, 'migrateSimpleGet Contract method get assertion failed!!!');

    // migrate clone test
    var mograteCloneCreateContract = await contract.deployContract("createContract");
    await contract.actionMethodSend("createContract", mograteCloneCreateContract, 'set_simple_address', simpleContract);
    await contract.actionMethodSend("createContract", mograteCloneCreateContract, 'migrate_clone', simpleContract);
    var simpleCloneCreateAddress = await contract.constMethodCall("simpleContract", simpleContract, 'get_address');

    // test migrate clone simple contract
    await contract.actionMethodSend("simpleContract", simpleCloneCreateAddress, 'set', 7);
    var migrateCloneSimpleGet = await contract.constMethodCall("simpleContract", simpleCloneCreateAddress, 'get');
    assert.equal(migrateCloneSimpleGet, 7, 'migrateCloneSimpleGet Contract method get assertion failed!!!');

}

exports.createContractTest = createContractTest;