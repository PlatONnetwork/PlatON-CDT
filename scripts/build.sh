#!/usr/bin/env bash

set -e

cd $( dirname "${BASH_SOURCE[0]}" )/..

ROOT=`pwd`
BUILD=$ROOT/build
LLVM_DIR=$BUILD/clang_llvm_8.0
CPUS=`nproc --all`

echo $ROOT

if [ -d $LLVM_DIR  ]; then
    # rm cmake cache file
    cd $BUILD
    ls | grep -v "clang_llvm_8.0" | xargs rm -rf

    cmake .. -DLLVM_CONFIG=$LLVM_DIR/bin/llvm-config
    make -j$CPUS
else
    rm -rf build
    # mkdir
    mkdir -p build && cd build

    cmake ..
    make -j$CPUS
fi
