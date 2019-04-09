# PlatON-CDT (Contract Development Toolkit)

PlatON-CDT is a toolchain for WebAssembly(WASM) and set of tools to faciliate
contract writing for the PlatON platform.

## Build

#### Required

- GCC 5.4+ or Clang 4.0+
- cmake 3.5+

### Ubuntu 

**Required:** 16.04+

- **Install Dependencies**

``` shell
sudo apt install libz-dev libtinfo-dev
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
mkdir build
cmake .. -DCMAKE_INSTALL_PREFIX=/you/path
make && make install
```

## Usage

- **Initial A Project**

``` sh
platon-init -project example -path /you/path
```

- **Build**

``` sh
cd example
platon-cpp -o example.wasm example.cpp -abigen
```
