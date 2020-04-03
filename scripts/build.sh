#!/usr/bin/env bash

set -e

cd $( dirname "${BASH_SOURCE[0]}" )/..

ROOT=`pwd`
BUILD=$ROOT/build
LLVM_DIR=$ROOT/depends/clang_llvm_8.0
CPUS=`nproc --all`

echo $ROOT

if [ -d $BUILD ]; then
    rm -rf build
fi

# mkdir
mkdir -p build && cd build

if [ -d $LLVM_DIR  ]; then
    cmake .. -DLLVM_CONFIG=$LLVM_DIR/bin/llvm-config
    make -j$CPUS
else
    cmake ..
    make -j$CPUS
fi
