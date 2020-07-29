var fs_extra = require("fs-extra");
var fs = require("fs")
var path = require("path");
var Web3 = require('web3')

// The directory where the wasm and abi files are located.
let contractDirectory = path.join(__dirname, "./build/");

// deploy address
var deployFrom = "lax1l0sm6ath520sa8tsq499ya8a5j9qkzh2zxgddm";

// send transaction address
var from = "lax1l0sm6ath520sa8tsq499ya8a5j9qkzh2zxgddm";

// configuration
var gas = 2000000;
var gasPrice = 50000000004;

// web3 object
var provider = new Web3.providers.HttpProvider(
    `http://10.1.1.10:36789`,
    { keepAlive: false }
);
var web3 = new Web3(provider);

// Gas consumption statistics
var gasJson = [];

//import private key and unlock account.
// The private key only needs to be imported once. 
// After importing, comment out the code for importing the private key, and don’t comment out the code for the unlocked wallet.
// web3.platon.personal.importRawKey("16e80ad4079462cc7f9748af2f9cf03e8f7384bed597c086db4f11a98c3b08f0", "123456");
web3.platon.personal.unlockAccount("lax1l0sm6ath520sa8tsq499ya8a5j9qkzh2zxgddm", "123456", 99999);

async function deployContract(contractName, ...args) {
    try {
        // find contractName.wasm and contractName.abi.json exist in build dictory
        var binPath = path.join(contractDirectory, contractName + ".wasm");
        var abiPath = path.join(contractDirectory, contractName + ".abi.json");
        var checkFile = fs.existsSync(binPath) && fs.existsSync(abiPath);
        if(!checkFile) {
            throw new Error("contract does not compile, not allow to deploy");
        }

        // create contract object
        var abi = JSON.parse(await fs_extra.readFileSync(abiPath, "utf8"));
        var contract = new web3.platon.Contract(abi, deployFrom, { vmType: 1 }); 

        // deploy parameters
        var bin = (await fs_extra.readFile(binPath)).toString("hex");
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
        gasJson.push({'coantract': contractName, 'method': 'deploy', 'gasUsed': receipt.gasUsed});
        return receipt.contractAddress;
    } catch(error) {
        console.trace("error: ", error);
    }
}


async function actionMethodSend(contractName, contractAddress, method, ...args){
    try {
        // find contractName.abi.json  exist in dictory
        var abiPath = path.join(contractDirectory, contractName + ".abi.json");
        if(!fs.existsSync(abiPath)){
            throw new Error("contract does not compile, not allow to deploy");
        }

        // ACTION method
        var abi = JSON.parse(await fs_extra.readFileSync(abiPath, "utf8"));
        var contract = new web3.platon.Contract(abi, contractAddress, {vmType: 1 });
        var sendResult = await contract.methods[method].apply(this, args).send({from: from, gas: gas});
        console.log("send: ",sendResult);
        gasJson.push({'coantract': contractName, 'method': method, 'gasUsed': sendResult.gasUsed});
        return sendResult;
    } catch(error) {
        console.trace("error: ", error);
    }
}

async function constMethodCall(contractName, contractAddress, method, ...args){
    try {
        // find contractName.abi.json exist in dictory
        var abiPath = path.join(contractDirectory, contractName + ".abi.json");
        if(!fs.existsSync(abiPath)){
            throw new Error("contract does not compile, not allow to deploy");
        }

        // ACTION method
        var abi = JSON.parse(await fs_extra.readFileSync(abiPath, "utf8"));
        var contract = new web3.platon.Contract(abi, contractAddress, {vmType: 1 });
        var callResult = await contract.methods[method].apply(this, args).call();
        console.log("call: ",callResult);
        return callResult;
    } catch(error) {
        console.trace("error: ", error);
    }
}


module.exports = {
    // Configuration parameter
    deployFrom: deployFrom,
    form: from,
    web3: web3,

    // function
    deployContract: deployContract, 
    actionMethodSend: actionMethodSend,
    constMethodCall: constMethodCall,

    // Gas consumption statistics
    gasJson:gasJson
}


