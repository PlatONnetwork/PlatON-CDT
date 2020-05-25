#!/usr/bin/env bash

set -e

cd $( dirname "${BASH_SOURCE[0]}" )/..

ROOT=`pwd`
BUILD=$ROOT/build
CPUS=`nproc --all`

echo $ROOT

if [ -d $BUILD ]; then
    rm -rf build
fi

# mkdir and build
mkdir -p build && cd build && cmake .. && make -j$CPUS

