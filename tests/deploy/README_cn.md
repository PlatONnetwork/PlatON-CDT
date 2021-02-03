#  js 链上测试工具

此工具主要用于在向链上发送部署合交易和调用合约接口交易，方便开发人员进行单元测试。

## 依赖

工具是用 nodejs 实现，需要安装 node 环境。

此工具依赖 `client-sdk-js`， `client-sdk-js` 使用了 lerna 管理工具，`sudo npm install lerna -g` 全局安装 lerna。

## 使用说明

- **安装依赖项**

`cd tests/deploy && npm install`

- **更改配置信息**

参考注释，更改 contract.js 里面的部署账户和调用账户地址。

参考注释，更改 contract.js 里面的节点信息为自己单元测试节点的信息。

目前钱包使用节点的 keystore， 需要执行之前导入私钥和解锁钱包账户，参考 contract.js 里面的注释。

例如：

导入私钥： `web3.platon.personal.importRawKey("16e80ad4079462cc7f9748af2f9cf03e8f7384bed597c086db4f11a98c3b08f0", "123456");`

解锁钱包账户： `web3.platon.personal.unlockAccount("lax1l0sm6ath520sa8tsq499ya8a5j9qkzh2zxgddm", "123456", 99999);`

- **启动单元测试**

参考模板编写自己的单元测试，编写完成之后， `npm run test` 即可启动单元测试。