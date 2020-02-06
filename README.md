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
- Tar
- Wget

### Ubuntu 

**Required:** 16.04+

- **Install Dependencies**

``` shell
sudo apt install build-essential cmake libz-dev libtinfo-dev tar wget
```

- **Get Source Code**

```shell
git clone https://github.com/PlatONnetwork/PlatON-CDT.git
cd PlatON-CDT
git checkout feature/wasm
```

- **Build Code**

Unit test modules are not included by default. The build operation is as follows.

``` sh
mkdir build && cd build

cmake ..
make -j4
sudo make install
```

If you want to add a unit test module, first install go1.13 and then do the following:

``` sh
mkdir build && cd build

cmake .. -DTEST=ON
make -j4
sudo make install
```

## Usage

``` bash
platon-cpp test.cpp
```

run command could generate test.wasm and test.abi.json in current dirctoy.

## License

GNU General Public License v3.0, see [LICENSE](https://github.com/PlatONnetwork/PlatON-CDT/blob/master/LICENSE).
