# platon-cpp compiler

## build
``` bash
mkdir build
cd build
wget http://releases.llvm.org/9.0.0/clang+llvm-9.0.0-x86_64-linux-gnu-ubuntu-18.04.tar.xz
tar xavf clang+llvm-9.0.0-x86_64-linux-gnu-ubuntu-18.04.tar.xz
cmake platon-cpp-src-path -DLLVM_CONFIG=clang+llvm-9.0.0-x86_64-linux-gnu-ubuntu-18.04/bin/llvm-config
make -j4
```

## usage
``` bash
platon-cpp test.cpp
```
run command could generate test.wasm and test.abi.json in current dirctoy.