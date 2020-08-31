#!/usr/bin/env bash

# Switch to the contracts directory
dir=$(cd "$(dirname "$0")";pwd)
cd ${dir}

# Delete old files
if [ -d "${dir}/build" ]; then
    rm -fr "${dir}/build"
fi
mkdir "${dir}/build"

# Compile contract files
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

# Move wasm and abi files
mv *.wasm "${dir}/build/"
mv *.json "${dir}/build/"

echo "All contracts are compiled successfully"