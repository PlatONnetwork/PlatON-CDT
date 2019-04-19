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
sudo apt install build-essential cmake libz-dev libtinfo-dev
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

Windows下编译需要先安装[MinGW-W64 GCC-8.1.0](https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win64/Personal%20Builds/mingw-builds/8.1.0/threads-posix/sjlj/x86_64-8.1.0-release-posix-sjlj-rt_v6-rev0.7z), 且安装路径不能含有空格(即: 不能安装在"Program
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

### 单文件项目

- 初始化项目

``` sh
platon-init -project example -bare
```

- 编译WASM文件

``` sh
cd example
platon-cpp -o example.wasm example.cpp -abigen
```

### CMake项目

- 初始化项目

```sh
platon-init -project cmake_example 
```

- 编译
  
  * Linux
  ```
  cd cmake_example/build
  cmake ..
  ```
  * Windows
  ```sh
  cd cmake_example/build
  cmake .. -G "MinGW Makefiles" -DPLATON_CDT_ROOT=<cdt_install_dir>
  ```
      **编译依赖:**
      * [MinGW-W64 GCC-8.1.0](https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win64/Personal%20Builds/mingw-builds/8.1.0/threads-posix/sjlj/x86_64-8.1.0-release-posix-sjlj-rt_v6-rev0.7z)
      * CMake 3.5 or higher
