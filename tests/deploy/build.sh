#!/usr/bin/env bash

# 切换到 contracts 目录
dir=$(cd "$(dirname "$0")";pwd)
cd ${dir}

# 删除旧文件
if [ -d "${dir}/build" ]; then
    rm -fr "${dir}/build"
fi
mkdir "${dir}/build"

# 编译合约文件
cd "${dir}/contract"
for one_file in `ls`
do   
    platon-cpp ${one_file}
    if [ 0 -ne $? ]; then
        echo "compile ${one_file} failed!!!"
        exit 1
    fi
    echo "${one_file} compiled successfully"
done 

# 移动 wasm 和 abi 文件
mv *.wasm "${dir}/build/"
mv *.json "${dir}/build/"

echo "All contracts are compiled successfully"