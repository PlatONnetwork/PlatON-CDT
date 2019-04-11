![logo](./docs/images/platon-cdt-logo.png)

[English](./README.md)

# PlatON-CDT (Contract Development Toolkit)

**PlatON-CDT**是WebAssembly(WASM)工具链和**PlatON**平台智能合约开发工具集.

## 编译

### 编译要求

- GCC 5.4+ 或 Clang 4.0+
- CMake 3.5+
- Git
- Python

### Ubuntu

以下编译步骤在ubuntu 16.04下操作.

- 安装依赖

```sh
sudo apt install libz-dev libtinfo-dev
```

- 获取源码

```shell
git clone https://github.com/PlatONnetwork/PlatON-CDT.git
cd PlatON-CDT
git submodule update --init --recursive
```

- 执行编译

``` sh
cd PlatON-CDT
mkdir build && cd build
cmake .. 
make && make install
```

### Windows

Windows下编译需要先安装MinGW, 且安装路径不能含有空格(即: 不能安装在"Program
Files"或"Program Files(x86)目录"), 否则编译失败.

- 获取源码

```shell
git clone https://github.com/PlatONnetwork/PlatON-CDT.git
cd PlatON-CDT
git submodule update --init --recursive
```

- 执行编译

``` sh
cd PlatON-CDT
mkdir build && cd build
cmake -G "MinGW Makefiles" .. -DCMAKE_INSTALL_PREFIX="C:/platon.cdt" -DCMAKE_MAKE_PROGRAM=mingw32-make
mingw32-make && mingw32-make install
```

## 使用

在使用PlatON-CDT之前必须将PlatON-CDT编译生成的执行文件路径加到PATH环境变量中.

- 初始化项目

``` sh
platon-init -project example 
```

- 编译WASM文件

``` sh
cd example
platon-cpp -o example.wasm example.cpp -abigen
```
