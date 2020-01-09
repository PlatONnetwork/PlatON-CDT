#!/bin/bash

# change directory
work_path=$(dirname $0)
cd ${work_path}

# build platon
go build unit_test_platon.go
