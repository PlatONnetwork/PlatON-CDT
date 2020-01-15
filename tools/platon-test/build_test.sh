#!/bin/bash

# change directory
work_path=$(dirname $0)
cd ${work_path}

# build platon
go build -o platon-test
