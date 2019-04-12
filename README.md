![logo](./docs/images/platon-cdt-logo.png)

[中文](./README_cn.md)

# PlatON-CDT (Contract Development Toolkit)

PlatON-CDT is a toolchain for WebAssembly(WASM) and set of tools to faciliate
contract writing for the PlatON platform.

## Build

### Required

- GCC 5.4+ or Clang 4.0+
- CMake 3.5+
- Git
- Python

### Ubuntu 

**Required:** 16.04+

- **Install Dependencies**

``` shell
sudo apt install build-essential cmake libz-dev libtinfo-dev
```

- **Get Source Code**

```shell
git clone https://github.com/PlatONnetwork/PlatON-CDT.git
cd PlatON-CDT
git submodule update --init --recursive
```
- **Build Code**

``` sh
cd PlatON-CDT
mkdir build && cd build
cmake .. 
make && make install
```

### Windows

**Required:** MinGW

**NOTES:** _MinGW and CMake must be installed in a directory without space._

- **Get Source Code**

```shell
git clone https://github.com/PlatONnetwork/PlatON-CDT.git
cd PlatON-CDT
git submodule update --init --recursive
```
- **Build Code**

``` sh
cd PlatON-CDT
mkdir build && cd build
cmake -G "MinGW Makefiles" .. -DCMAKE_INSTALL_PREFIX="C:/platon.cdt" -DCMAKE_MAKE_PROGRAM=mingw32-make
mingw32-make && mingw32-make install
```

## Usage

- **Initial A Project**

``` sh
platon-init -project example
```

- **Build**

``` sh
cd example
platon-cpp -o example.wasm example.cpp -abigen
```
