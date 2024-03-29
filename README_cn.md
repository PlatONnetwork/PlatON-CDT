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
- Golang 1.14+

### Ubuntu

以下编译步骤在ubuntu 18.04下操作.

- **安装依赖**

```sh
sudo apt install build-essential cmake libz-dev libtinfo-dev tar wget
```

- **获取源码**

```shell
git clone https://github.com/PlatONnetwork/PlatON-CDT.git
cd PlatON-CDT
git checkout feature/wasm
```

- **编译构建**

  - **用脚本构建**

    ``` sh
    ./scripts/build.sh
    cd build
    sudo make install
    ```

  - **cmake 编译构建**

    ``` sh
    mkdir build && cd build
    cmake ..
    make -j
    sudo make install
    ```

    一些常用 cmake 选项:
    - `-DTEST=ON` 构建单元测试模块，你需要首先安装go1.13及以上版本（默认不构建单元测试模块）。
    - `-DCMAKE_INSTALL_PREFIX=directory` 指定目录安装cdt相关工具和依赖库（默认`/usr/local`）。
    - `-DCDT_BUILD_TYPE=type` 构建类型的有效选项是Debug，Release。Debug模式断言检查会被启用，日志文件会打印调试和错误信息。（默认是Release）。

    如果构建单元测试模块，需要在 ./tools/platon-test/node 目录中下载 PlatON-Go，并在下载完成后编译。

    ```shell
    cd tools/platon-test && mkdir node && cd node
    git clone git@github.com:PlatONnetwork/PlatON-Go.git --recursive
    cd PlatON-Go
    git checkout ${BRANCH}
    make all
    ```


## 使用

如果使用 CMAKE_INSTALL_PREFIX 选项指定安装目录，在使用PlatON-CDT之前必须将PlatON-CDT编译生成的执行文件路径加到PATH环境变量中（默认安装在 /usr/local/bin 目录）。

``` sh
platon-cpp test.cpp
```

运行指令会在当前目录生成test.wasm和test.abi.json两个文件

## License

GNU General Public License v3.0, see [LICENSE](https://github.com/PlatONnetwork/PlatON-CDT/blob/master/LICENSE).
