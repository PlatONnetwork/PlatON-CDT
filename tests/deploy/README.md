
[中文](./README_cn.md)
# js On-chain testing tool

This tool is mainly used to send deployment and transaction transactions and call contract interface transactions on the chain, which is convenient for developers to perform unit testing.

## Dependence

The tool is implemented with nodejs, and the node environment needs to be installed.

`client-sdk-js` uses the lerna management tool, execute the `sudo npm install lerna -g` command to install lerna globally.

## Instructions for use

-**Install dependencies**

`cd tests/deploy && npm install`

-**Change configuration information**

Change the configuration in config.json and set it to the configuration of your own node.

```json
{
    "deployFrom": "lax1ghvqzvmpxwarcqhkmd6d5f3lrzept59wvj0feg", // sender address of deployment contract transaction
    "deployFromPrivateKey": "ad296b67d8e9bd226189efd9ae2f637645360e784c725bf03d1a1a0fe5d45ec6", // deploy the private key of the contract transaction sender
    "deployFromPassword": "123456", // Deploy the contract transaction sender wallet password
    "from": "lax1gutkc403f9fh57xa8skelk43znyjjgp0wn3q4d", // call the wallet password of the contract transaction sender
    "fromPrivateKey": "e53e05953b3d1992b7d3d27a6ef9554c797088d68eed114d1c8bfe2a80ff29b5", // Call the wallet password of the contract transaction sender
    "fromPassword": "12345", // Call the wallet password of the contract transaction sender
    "gas": 4500000,
    "gasPrice": 50000000004,
    "server": "http://10.1.1.10:36789" // The node opens the rpc service information
}
```

-**Add your own wasm contract and unit test js file**

Put the wasm contract file to be tested under the `contract` directory

Add your own `{contract_name}Test.js` unit test js module, write the test function `async function {contract_name}Test(){...}`, and export the test function `exports.{contract_name}Test = {contract_name} Test;`.

Import the new unit test module `var {contract_name}Test = require("./{contract_name}Test"); in the `test.js` file, and add your own unit test function execution to the `contractTest` function Logic `await {contract_name}Test.{contract_name}Test();`.

-**Start Unit Test**

Execute `./build.sh` to compile the contract

Write your own unit test with reference to the template. After the writing is completed, `npm run test` can start the unit test.