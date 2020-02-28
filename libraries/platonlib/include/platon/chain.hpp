#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get the value of gas price
 *
 * @param gas_price The big-endian represents the gas price
 * @return the length of gas price
 */
uint8_t platon_gas_price(uint8_t gas_price[16]);

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
void platon_block_hash(int64_t num, uint8_t hash[32]);

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
 * @param balance The big end represents the balance of 128
 * @return the length of balance
 */
uint8_t platon_balance(const uint8_t addr[20], uint8_t balance[16]);

/**
 * @brief Transfer the amount to the address
 * You need to convert the amount into your own array of big-end
 * representations.
 *
 * @param addr address
 * @param amount Pointer to the amount array
 * @param len The length of the amount array
 * @return The call succeeds or fails
 */
int32_t platon_transfer(const uint8_t to[20], const uint8_t *amount,
                        size_t len);

/**
 * @brief Get The value of the current transaction value field
 *
 * @param val The big end represents the value of 128
 * @return The length of value
 */
uint8_t platon_call_value(uint8_t val[16]);

/**
 * @brief Sh3 algorithm
 *
 * @param src Hash input pointer to the byte array
 * @param srcLen Hash input the length of the byte array
 * @param dest Hash output pointer to byte array
 * @param destLen The maximum length of a hash output byte array
 * @return void
 */
void platon_sha3(const uint8_t *src, size_t srcLen, uint8_t *dest,
                 size_t destLen);

/**
 * @brief Set the return value
 *
 * @param value The point to the data
 * @param len The data size
 */
void platon_return(const uint8_t *value, const size_t len);

/**
 * @brief Get the input params length
 */
size_t platon_get_input_length(void);

/**
 * @brief Get the input value
 *
 * @param value The output value
 */
void platon_get_input(uint8_t *value);

/**
 * @brief Print the debug info in log
 *
 * @param dst The point to the debug info
 * @param len The length of debug info
 */
void platon_debug(const uint8_t *dst, size_t len);

/**
 * @brief Terminate transaction, deduct all gas given by transaction
 */
void platon_panic(void);

/**
 * @brief Terminate the transaction and deduct the gas consumed
 */
void platon_revert(void);

/**
 * @brief Destory contract
 *
 * @param to Address of the contract
 * @return 0 if destroy successfully
 */
int32_t platon_destroy(const uint8_t to[20]);

/**
 * @brief Migrate contract
 *
 * @param new_addr The address of new contract
 * @param args The input arguments
 * @param args_len The length of input arguments
 * @param value Transfer amount, big-endian bytes
 * @param value_len The The length of amount bytes
 * @param call_cost Pay amount of gas for this transaction, big-endian bytes
 * @param call_cost_len The length of gas bytes
 * @return 0 if migration successfully
 */
int32_t platon_migrate(uint8_t new_addr[20], const uint8_t *args,
                       size_t args_len, const uint8_t *value, size_t value_len,
                       const uint8_t *call_cost, size_t call_cost_len);

/**
 * @brief Cross contract call
 *
 * @param to Address of contract
 * @param args The input arguments
 * @param args_len The length of input arguments
 * @param value Transfer amount, big-endian bytes
 * @param value_len The The length of amount bytes
 * @param call_cost Pay amount of gas for this transaction, big-endian bytes
 * @param call_cost_len The length of gas bytes
 * @return 0 if call successfully
 */
int32_t platon_call(const uint8_t to[20], const uint8_t *args, size_t args_len,
                    const uint8_t *value, size_t value_len,
                    const uint8_t *call_cost, size_t call_cost_len);
/**
 * @brief Cross contract delegation call
 *
 * @param to Address of contract
 * @param args The input arguments
 * @param args_len The length of input arguments
 * @param call_cost Pay amount of gas for this transaction, big-endian bytes
 * @param call_cost_len The length of gas bytes
 * @return 0 if call successfully
 */
int32_t platon_delegate_call(const uint8_t to[20], const uint8_t *args,
                             size_t args_len, const uint8_t *call_cost,
                             size_t call_cost_len);

/**
 * @brief Get the length of call ouput
 */
size_t platon_get_call_output_length();

/**
 * @brief Get the value of call ouput
 *
 * @param value The call output value
 */
void platon_get_call_output(uint8_t *value);

/**
 * @brief Post event to VM
 *
 * @param topic The topic of event
 * @param topic_len The length of topic
 * @param args The arguments
 * @param args_len The length of arguments
 */
void platon_event(const uint8_t *topic, size_t topic_len, const uint8_t *args,
                  size_t args_len);

/**
 * @brief Get the address of original caller
 *
 * @param addr Address
 */
void platon_origin(uint8_t addr[20]);

/**
 * @brief Get the address of caller
 *
 * @param addr Address
 */
void platon_caller(uint8_t addr[20]);

/**
 * @brief Get the address of contract
 *
 * @param addr Address
 */
void platon_address(uint8_t addr[20]);

/**
 * @brief Set the state object
 *
 * @param key Key
 * @param Klen The length of key
 * @param value Value
 * @param vlen The length of value
 */
void platon_set_state(const uint8_t *key, size_t klen, const uint8_t *value,
                      size_t vlen);

/**
 * @brief Get the length of state object
 *
 * @param key Key
 * @param klen The length of key
 * @return The length of state object
 */
size_t platon_get_state_length(const uint8_t *key, size_t klen);

/**
 * @brief Get the state object
 *
 * @param key Key
 * @param klen The length of Key
 * @param value Value
 * @param vlen The length of value
 * @return The length of value
 */
size_t platon_get_state(const uint8_t *key, size_t klen, uint8_t *value,
                        size_t vlen);

#ifdef __cplusplus
}
#endif
