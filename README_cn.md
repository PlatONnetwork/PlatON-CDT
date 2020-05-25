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

    make -j$(nproc --all)
    sudo make install
    ```

    一些常用选项:
    - `-DTEST=ON` 构建单元测试模块，你需要首先安装go1.13及以上版本（默认不构建单元测试模块）。
    - `-DCMAKE_INSTALL_PREFIX=directory` 指定目录安装cdt相关工具和依赖库（默认`/usr/local`）。
    - `-CDT_BUILD_TYPE=type` 构建类型的有效选项是Debug，Release。Debug模式断言检查会被启用，日志文件会打印调试和错误信息。（默认是Release）。

## 使用

在使用PlatON-CDT之前必须将PlatON-CDT编译生成的执行文件路径加到PATH环境变量中.

``` sh
platon-cpp test.cpp
```

运行指令会在当前目录生成test.wasm和test.abi.json两个文件

## License

GNU General Public License v3.0, see [LICENSE](https://github.com/PlatONnetwork/PlatON-CDT/blob/master/LICENSE).
