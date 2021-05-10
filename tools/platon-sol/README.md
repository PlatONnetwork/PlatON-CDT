# platon_sol

Use Solidity abi file to generate wasm cross-contract call code or use wasm abi file to generate solidity cross-contract call code.

## Start

```
platon-sol solidity --solidityAbi <solidity-abi-file> --namespace <cpp namespace> --output <output path>
```
**solidityAbi:** solidity abi file path

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

```
platon-sol wasm --wasmAbi <wasm-abi-file> --wasmAbi <output path>
```
**wasmAbi:** wasm abi file path

**output:** contract output path

## Support Wasm Type

1. All length wasm int types are mapped to solidity int types. All length wasm uint types are mapped to solidity uint types.
2. Pay attention to the size of int and uint assignment. If the value is too large, there may be a risk of overflow on the wasm side.


|   Wasm Type   | Solidity Type |
|:-----------------:| :-----------------:|
|      bool         |bool |
|     int8_t     | int |
|     int16_t      | int |
|     int32_t      | int |
|     int64_t     | int |
|       int         | int |
|       int128_t     | int |
|       int256_t     | int |
|    uint8_t    | uint |
|    uint16_t    | uint |
|      uint32_t       | uint |
|      uint64_t      | uint |
|    uint128_t     | uint |
|     uint256_t     | uint |
|     std::string       | string |
|      platon::Address       | address |
|      std::array<T, N>      | T[N] |
|      std::vector<T>      | T[] |
|      std::map       | struct[] |
|      struct       | struct |
|      class       | struct |
