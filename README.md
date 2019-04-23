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

**Required:** [MinGW-W64 GCC-8.1.0](https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win64/Personal%20Builds/mingw-builds/8.1.0/threads-posix/sjlj/x86_64-8.1.0-release-posix-sjlj-rt_v6-rev0.7z)

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

### Singleton Contract

- Init a project

``` sh
platon-init -project example -bare
```

- Build contract

``` sh
cd example
platon-cpp -o example.wasm example.cpp -abigen
```

### CMake Project

- Init CMake project

``` sh
platon-init -project cmake_example
```

- Build contract
  * Linux
  ```
  cd cmake_example/build
  cmake ..
  ```
  * Windows
  >**Required:**
  >+ [MinGW-W64 GCC-8.1.0](https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win64/Personal%20Builds/mingw-builds/8.1.0/threads-posix/sjlj/x86_64-8.1.0-release-posix-sjlj-rt_v6-rev0.7z)
  >+ CMake 3.5 or higher

  ```sh
  cd cmake_example/build
  cmake .. -G "MinGW Makefiles" -DPLATON_CDT_ROOT=<cdt_install_dir>
  ```


## License

GNU General Public License v3.0, see [LICENSE](https://github.com/PlatONnetwork/PlatON-CDT/blob/master/LICENSE).
