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

# Compile contract files
cd "${dir}/contract"
for one_file in $(ls); do
    result=$(echo ${one_file} | grep ".h")
    if [[ "$result" == "" ]]; then
        platon-cpp ${one_file}
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
