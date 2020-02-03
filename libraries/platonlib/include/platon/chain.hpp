#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get the value of gas price
 * 
 * @param void
 * @return the value of gas price
 */
uint64_t platon_gas_price();

/**
 * @brief Get the height of the current block
 * 
 * @param void
 * @return the height of the current block
 */
uint64_t platon_block_number();

/**
 * @brief Get the value of gas price limit
 * 
 * @param void
 * @return the value of gas price limit
 */
uint64_t platon_gas_limit();

/**
 * @brief Get the value of gas
 * 
 * @param void
 * @return the value of gas
 */
uint64_t platon_gas();

/**
 * @brief Get the timestamp of the current block
 * 
 * @param void
 * @return the timestamp of the current block
 */
int64_t platon_timestamp();

/**
 * @brief Get the value of the caller's nonce
 * 
 * @param void
 * @return the value of the caller's nonce
 */
uint64_t platon_caller_nonce();

/**
 * @brief Gets the block hash based on the block height
 *
 * @param num Height of block
 * @param hash Hash of block
 * @return void
 */
void platon_block_hash(int64_t num,  uint8_t hash[32]);

/**
 * @brief Gets the Hash of miner nodes
 *
 * @param addr Hash of miner nodes
 * @return void
 */
void platon_coinbase(uint8_t addr[20]);

/**
 * @brief Get the balance based on the address
 *
 * @param addr address
 * @param balance The big end represents the balance of 256
 * @return the length of balance
 */
uint8_t platon_balance(const uint8_t  addr[20], uint8_t balance[32]);

/**
 * @brief Transfer the amount to the address
 * You need to convert the amount into your own array of big-end representations.
 *
 * @param addr address
 * @param amount Pointer to the amount array
 * @param len The length of the amount array
 * @return The call succeeds or fails
 */
int32_t platon_transfer(const uint8_t to[20], const uint8_t *amount, size_t len);

/**
 * @brief Get The value of the current transaction value field
 * 
 * @param val The big end represents the value of 256
 * @return The length of value
 */
uint8_t platon_call_value(uint8_t val[32]);

/**
 * @brief Sh3 algorithm
 * 
 * @param src Hash input pointer to the byte array
 * @param srcLen Hash input the length of the byte array
 * @param dest Hash output pointer to byte array
 * @param destLen The maximum length of a hash output byte array
 * @return void
 */
void platon_sha3(const uint8_t *src, size_t srcLen, uint8_t *dest, size_t destLen);

#ifdef __cplusplus
}
#endif