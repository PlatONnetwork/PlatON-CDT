var fs_extra = require("fs-extra");
var fs = require("fs")
var path = require("path");
var Web3 = require('web3')

// configuration
var config = require('./config');

// The directory where the wasm and abi files are located.
let contractDirectory = path.join(__dirname, "./build/");

// deploy address
var deployFrom = config["deployFrom"];

// send transaction address
var from = config["from"];

// gas  and  gasPrice
var gas = config["gas"];
var gasPrice = config["gasPrice"];

// web3 object
var provider = new Web3.providers.HttpProvider(
    config["server"],
    { keepAlive: false }
);
var web3 = new Web3(provider);

// Gas consumption statistics
var gasJson = [];

//import private key and unlock account.
async function importAccount() {
    allAcount = await web3.platon.personal.getAccounts()

    if (! allAcount.includes(deployFrom)) {
        await web3.platon.personal.importRawKey(config["deployFromPrivateKey"], config["deployFromPassword"]);
    } 
    await web3.platon.personal.unlockAccount(deployFrom, config["deployFromPassword"], 99999);
    

    if (! allAcount.includes(from)) {
        await web3.platon.personal.importRawKey(config["fromPrivateKey"], config["fromPassword"]);
    } 
    await web3.platon.personal.unlockAccount(from, config["fromPassword"], 99999);
}

function changeDeploy(addr) { deployFrom = addr; }

function changeFrom(addr) { from = addr; }

async function deployContract(wasm, contractName, ...args) {
    try {
        await importAccount();
        // find contractName.wasm and contractName.abi.json exist in build dictory
        var binPath;
        var abiPath;

        if (wasm) {
            binPath = path.join(contractDirectory, contractName + ".wasm");
            abiPath = path.join(contractDirectory, contractName + ".abi.json");
        } else {
            binPath = path.join(contractDirectory, contractName + ".bin");
            abiPath = path.join(contractDirectory, contractName + ".abi");
        }

        var checkFile = fs.existsSync(binPath) && fs.existsSync(abiPath);
        if (!checkFile) {
            throw new Error("contract does not compile, not allow to deploy");
        }

        // create contract object
        var abi = JSON.parse(await fs_extra.readFileSync(abiPath, "utf8"));
        var contract;
        if (wasm) {
            contract = new web3.platon.Contract(abi, deployFrom, { vmType: 1 });
        } else {
            contract = new web3.platon.Contract(abi, deployFrom, { vmType: 0 });
        }

        // deploy parameters
        var bin;
        if(wasm){
            bin = (await fs_extra.readFile(binPath)).toString("hex");
        }else{
            bin = await fs_extra.readFileSync(binPath, "utf8");
            bin = "0x" + bin;
        }
        var params = {};
        params.data = contract.deploy({
            data: bin,
            arguments: args
        }).encodeABI();
        params.from = deployFrom;
        params.gas = gas;
        params.gasPrice = gasPrice;

        // deploy contract
        const receipt = await web3.platon.sendTransaction(params);
        console.log("deploy: ", receipt);
        if (receipt.status !== undefined && !receipt.status) {
            throw new Error("Failed to deploy wasm contract");
        }
        gasJson.push({ 'coantract': contractName, 'method': 'deploy', 'gasUsed': receipt.gasUsed });
        return receipt.contractAddress;
    } catch (error) {
        console.trace("error: ", error);
    }
}


async function actionMethodSend(wasm, contractName, contractAddress, method, ...args) {
    try {
        await importAccount();

        // find contractName.abi.json  exist in dictory
        var abiPath;
        if (wasm) {
            abiPath = path.join(contractDirectory, contractName + ".abi.json");
        } else {
            abiPath = path.join(contractDirectory, contractName + ".abi");
        }

        if (!fs.existsSync(abiPath)) {
            throw new Error("contract does not compile, not allow to deploy");
        }

        // ACTION method
        var abi = JSON.parse(await fs_extra.readFileSync(abiPath, "utf8"));
        var contract;
        if (wasm) {
            contract = new web3.platon.Contract(abi, contractAddress, { vmType: 1 });
        } else {
            contract = new web3.platon.Contract(abi, contractAddress, { vmType: 0 });
        }
        var sendResult = await contract.methods[method].apply(this, args).send({ from: from, gas: gas });
        console.log("send: ", sendResult);
        gasJson.push({ 'coantract': contractName, 'method': method, 'gasUsed': sendResult.gasUsed });
        return sendResult;
    } catch (error) {
        console.trace("error: ", error);
    }
}

async function constMethodCall(wasm, contractName, contractAddress, method, ...args) {
    try {
        await importAccount();

        // find contractName.abi.json exist in dictory
        var abiPath;
        if (wasm) {
            abiPath = path.join(contractDirectory, contractName + ".abi.json");
        } else {
            abiPath = path.join(contractDirectory, contractName + ".abi");
        }
        if (!fs.existsSync(abiPath)) {
            throw new Error("contract does not compile, not allow to deploy");
        }

        // ACTION method
        var abi = JSON.parse(await fs_extra.readFileSync(abiPath, "utf8"));
        var contract;
        if (wasm) {
            contract = new web3.platon.Contract(abi, contractAddress, { vmType: 1 });
        } else {
            contract = new web3.platon.Contract(abi, contractAddress, { vmType: 0 });
        }
        var callResult = await contract.methods[method].apply(this, args).call();
        console.log("call: ", callResult);
        return callResult;
    } catch (error) {
        console.trace("error: ", error);
    }
}


module.exports = {
    // Configuration parameter
    deployFrom: deployFrom,
    from: from,
    web3: web3,

    // account
    changeDeploy: changeDeploy,
    changeFrom: changeFrom,

    // function
    deployContract: deployContract,
    actionMethodSend: actionMethodSend,
    constMethodCall: constMethodCall,

    // Gas consumption statistics
    gasJson: gasJson
}


