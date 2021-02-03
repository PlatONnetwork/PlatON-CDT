
[中文](./README_cn.md)

# js On-chain testing tool

This tool is mainly used to send deployment and transaction transactions and call contract interface transactions on the chain to facilitate unit testing by developers.

## Dependence

The tool is implemented with nodejs, and the node environment needs to be installed.

This tool relies on `client-sdk-js`, `client-sdk-js` uses lerna management tool, `sudo npm install lerna -g` installs lerna globally.

## Instructions for use

-**Install dependencies**

`cd tests/deploy && npm install`

-**Change configuration information**

Refer to the comments to change the deployment account and calling account address in contract.js.

Refer to the comments and change the node information in contract.js to the information of your unit test node.

Currently, the wallet uses the keystore of the node. It is necessary to import the private key and unlock the wallet account before execution. Refer to the comments in contract.js.

E.g:

Import the private key: `web3.platon.personal.importRawKey("16e80ad4079462cc7f9748af2f9cf03e8f7384bed597c086db4f11a98c3b08f0", "123456");`

Unlock wallet account: `web3.platon.personal.unlockAccount("lax1l0sm6ath520sa8tsq499ya8a5j9qkzh2zxgddm", "123456", 99999);`

-**Start Unit Test**

Write your own unit test with reference to the template. After the writing is completed, `npm run test` can start the unit test.