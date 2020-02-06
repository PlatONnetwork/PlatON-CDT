![logo](./docs/images/platon-cdt-logo.png)

[English](./README.md)

# PlatON-CDT (Contract Development Toolkit)

**PlatON-CDT**是WebAssembly(WASM)工具链和**PlatON**平台智能合约开发工具集.

## 编译

### 编译要求

- GCC 5.4+ 或 Clang 4.0+
- CMake 3.5+
- Git
- Tar
- Wget

### Ubuntu

以下编译步骤在ubuntu 16.04下操作.

- 安装依赖

```sh
sudo apt install build-essential cmake libz-dev libtinfo-dev tar wget
```

- 获取源码

```shell
git clone https://github.com/PlatONnetwork/PlatON-CDT.git
cd PlatON-CDT
git checkout feature/wasm
```

- 执行编译

默认情况下不带单元测试模块。构建操作如下

``` sh
mkdir build && cd build

cmake .. 
make -j4
sudo make install
```

如果你要加上单元测试模块，首先安装go1.13， 然后进行如下操作：

``` sh
mkdir build && cd build

cmake .. -DTEST=ON
make -j4
sudo make install
```

## 使用

在使用PlatON-CDT之前必须将PlatON-CDT编译生成的执行文件路径加到PATH环境变量中.

``` sh
platon-cpp test.cpp
```

运行指令会在当前目录生成test.wasm和test.abi.json两个文件

## License

GNU General Public License v3.0, see [LICENSE](https://github.com/PlatONnetwork/PlatON-CDT/blob/master/LICENSE).
