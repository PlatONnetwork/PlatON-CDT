#!/usr/bin/env bash

# Switch to the contracts directory
dir=$(
    cd "$(dirname "$0")"
    pwd
)
cd ${dir}

# Delete old files
if [ -d "${dir}/build" ]; then
    rm -fr "${dir}/build"
fi
mkdir "${dir}/build"

# get solc-static-linux
if [ ! -f "${dir}/solc-static-linux" ]; then
    wget https://github.com/ethereum/solidity/releases/download/v0.5.17/solc-static-linux
    if [ 0 -ne $? ]; then
            echo "wget solc-static-linux failed!!!"
            exit 1
    fi
    chmod a+x solc-static-linux
fi

# Compile contract files
cd "${dir}/contract"
for one_file in $(ls); do
    result=$(echo ${one_file} | grep ".cpp")
    if [ ! -z  "$result" ]; then
        platon-cpp ${one_file}
        if [ 0 -ne $? ]; then
            echo "compile ${one_file} failed!!!"
            exit 1
        fi
        echo "${one_file} compiled successfully"
    fi
    result=$(echo ${one_file} | grep ".sol")
    if [ ! -z  "$result" ]; then
        ${dir}/solc-static-linux ${one_file} --bin --abi --opcodes --optimize --overwrite -o ${dir}/build
        if [ 0 -ne $? ]; then
            echo "compile ${one_file} failed!!!"
            exit 1
        fi
        echo "${one_file} compiled successfully"
    fi
done

# Move wasm and abi files
mv *.wasm "${dir}/build/"
mv *.json "${dir}/build/"

echo "All contracts are compiled successfully"