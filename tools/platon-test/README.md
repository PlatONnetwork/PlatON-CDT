# PlatON-Test


## Introduction

PlatON-Test is C++ Test tools

### compile

```
platon-test compile --dir {test file dir} | --file {test file}  --output {output dir} --bin {platon-cpp path}
```

### exec

```
platon-test exec --dir {wasm test file dir} | --file {wasm test file} 
```

### exec

compile and execution test file

```
platon-test compile --dir {test file dir} | --file {test file}  --output {output dir} --bin {platon-cpp path}
```

### sol

execution solidity test file

**solidity format**

```
pragma solidity ^0.8.2;
contract Test {
    // no constructor
    // testcase entry function
    function invoke() public pure returns (int) {
        case1();
        case2();
        return 0;
    }
    // tset case
    function case1() internal pure returns (int) {
        return 1;
    }
    function case2() internal pure returns (int) {
        return 1;
    }
}
```

```
platon-test  sol --file {test file}
```