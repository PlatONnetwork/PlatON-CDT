#  js 链上测试工具

此工具主要用于在向链上发送部署合交易和调用合约接口交易，方便开发人员进行单元测试。

## 依赖

工具是用 nodejs 实现，需要安装 node 环境。

`client-sdk-js` 使用了 lerna 管理工具，执行`sudo npm install lerna -g` 命令全局安装 lerna。

## 使用说明

- **安装依赖项**

`cd tests/deploy && npm install`

- **更改配置信息**

更改 config.json 里面的配置，设置为自己节点的配置。

```json
{
    "deployFrom" : "lax1ghvqzvmpxwarcqhkmd6d5f3lrzept59wvj0feg",                                    // 部署合约交易发送者地址
    "deployFromPrivateKey" : "ad296b67d8e9bd226189efd9ae2f637645360e784c725bf03d1a1a0fe5d45ec6",    // 部署合约交易发送者私钥
    "deployFromPassword" : "123456",                                                                // 部署合约交易发送者钱包密码
    "from" : "lax1gutkc403f9fh57xa8skelk43znyjjgp0wn3q4d",                                          // 调用合约交易发送者钱包密码
    "fromPrivateKey" : "e53e05953b3d1992b7d3d27a6ef9554c797088d68eed114d1c8bfe2a80ff29b5",          // 调用合约交易发送者钱包密码
    "fromPassword" : "12345",                                                                       // 调用合约交易发送者钱包密码
    "gas": 4500000,
    "gasPrice" : 50000000004,
    "server": "http://10.1.1.10:36789"                                                              // 节点开启 rpc 服务信息
}
```

- **增加自己的 wasm 合约 和 单元测试 js 文件**

将要测试的 wasm 合约文件放到 `contract` 目录下面

增加自己的 `{contract_name}Test.js` 单元测试 js 模块， 编写测试函数 `async function {contract_name}Test(){...}`, 并导出测试函数 `exports.{contract_name}Test = {contract_name}Test;`。

在 `test.js` 文件导入新增的单元测试模块 `var {contract_name}Test = require("./{contract_name}Test");`，并在 `contractTest` 函数里面加上自己的单元测试函数执行逻辑 `await {contract_name}Test.{contract_name}Test();`。

- **启动单元测试**

执行 `./build.sh` 编译合约

参考模板编写自己的单元测试，编写完成之后， `npm run test` 即可启动单元测试。