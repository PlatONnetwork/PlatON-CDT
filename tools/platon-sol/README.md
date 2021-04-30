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

|   Wasm Type   |
|:-----------------:|
|      bool         |
|     int8_t     |
|     int16_t      |
|     int32_t      |
|     int64_t     |
|       int         |
|       int128_t     |
|       int256_t     |
|    uint8_t    |
|    uint16_t    |
|      uint32_t       |
|      uint64_t      |
|    uint128_t     |
|     uint256_t     |
|     std::string       |
|      platon::Address       |
|      std::array       |
|      std::vector       |
|      std::map       |
|      struct       |
|      class       |
