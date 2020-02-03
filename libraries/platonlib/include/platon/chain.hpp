#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Return the value of gas price
uint64_t platon_gas_price();

// Return the height of the current block
uint64_t platon_block_number();

// Return the value of gas price limit
uint64_t platon_gas_limit();

// Return the value of gas
uint64_t platon_gas();

// Return the timestamp of the current block
int64_t platon_timestamp();

// Return the value of the caller's nonce
uint64_t platon_caller_nonce();


// Args:
//      num: Height of block
//      hash: Hash of block
// Gets the block hash based on the block height
void platon_block_hash(int64_t num,  uint8_t hash[32]);

// Args:
//      addr: Hash of miner nodes
// Gets the Hash of miner nodes
void platon_coinbase(uint8_t addr[20]);

// Args:
//      addr: address
//      balance: The big end represents the balance of 256
// Get the balance based on the address
uint8_t platon_balance(const uint8_t  addr[20], uint8_t balance[32]);

// Args:
//      addr: address
//      amount: Pointer to the amount array
//      len: The length of the amount array
// You need to convert the amount into your own array of big-end representations.
// Transfer the amount to the address.
int32_t platon_transfer(const uint8_t to[20], const uint8_t *amount, size_t len);

//Return  The value of the current transaction value field
uint8_t platon_call_value(uint8_t val[32]);

// Sh3 algorithm
void platon_sha3(const uint8_t *src, size_t srcLen, uint8_t *dest, size_t destLen);

#ifdef __cplusplus
}
#endif