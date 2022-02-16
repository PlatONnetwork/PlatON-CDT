#!/usr/bin/env bash

set -e

cd $( dirname "${BASH_SOURCE[0]}" )/..

ROOT=`pwd`
BUILD=$ROOT/build
CPUS=`grep -c ^processor /proc/cpuinfo 2>/dev/null || sysctl -n hw.ncpu`

echo $ROOT

if [ -d $BUILD ]; then
    rm -rf build
fi

# mkdir and build
git checkout feature/wasm
mkdir -p build && cd build && cmake .. && make -j$CPUS

