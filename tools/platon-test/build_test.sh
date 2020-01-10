#!/bin/bash

# change directory
work_path=$(dirname $0)
cd ${work_path}

# build platon
go build platon.go vm.go

mv platon platon-test
