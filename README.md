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

**Required:** 18.04

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

  - **Build with script**

    ``` sh
    ./scripts/build.sh
    cd build
    sudo make install
    ```

  - **cmake build**

    ``` sh
    mkdir build && cd build

    make -j$(nproc --all)
    sudo make install
    ```

    Some common options:
    - `-DTEST=ON` To build a unit test module, you need to install go1.13 and above first (the unit test module is not built by default).
    - `-DCMAKE_INSTALL_PREFIX=directory` Specify the directory to install cdt related tools and dependent libraries (default `/ usr / local`).
    - `-CDT_BUILD_TYPE=type` The valid options for the build type are Debug and Release. Debug mode assertion checking will be enabled, and the log file will print debugging and error information. (The default is Release).

## Usage

``` bash
platon-cpp test.cpp
```

run command could generate test.wasm and test.abi.json in current dirctoy.

## License

GNU General Public License v3.0, see [LICENSE](https://github.com/PlatONnetwork/PlatON-CDT/blob/master/LICENSE).
