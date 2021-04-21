# platon_sol

Generate wasm cross-contract call code using solidity abi file.


## Start

```
platon-sol -abi <solidity-abi-file> -namespace <cpp namespace> -output <output path>
```
**abi:** solidity abi file path

**namespace:** cpp contract namespace

**output:** contract output path

## Support Solidity Type

|   Solidity Type   |
|:-----------------:|
|     uint\<M\>     |
|     int\<M\>      |
|      address      |
|     uint,uint     |
|       bool        |
|    bytes\<M\>     |
|    \<type\>[M]    |
|       bytes       |
|      string       |
|    \<type\>[]     |
|   (T1,T2,...Tn)   |
|      struct       |
