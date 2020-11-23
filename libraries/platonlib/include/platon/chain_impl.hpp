#pragma once

#include "RLP.h"
#include "chain.hpp"
#include "exchange.hpp"
#include "fixedhash.hpp"
#include "rlp_extend.hpp"

namespace platon {

/**
 * @brief Get the value of gas price
 *
 * @return The value of gas price
 */
u128 platon_gas_price() {
  byte gas_price[16];
  size_t len = ::platon_gas_price(gas_price);
  return fromBigEndian<u128>(bytesRef(gas_price, len));
}

/**
 * @brief Get the unix timestamp of the current block
 *
 * @return The unix timestamp of the current block (second)
 */
int64_t platon_unix_timestamp() { return platon_timestamp() / 1000; }

/**
 * @brief Gets the block hash based on the block height
 *
 * @param num Height of block
 * @return Hash of block
 */
h256 platon_block_hash(int64_t num) {
  byte hash[32];
  ::platon_block_hash(num, hash);
  return h256(hash, sizeof(hash));
}

/**
 * @brief Gets the Hash of miner nodes
 *
 * @return Hash of miner nodes
 */
Address platon_coinbase() {
  byte hash[20];
  ::platon_coinbase(hash);
  return Address(hash, sizeof(hash));
}

/**
 * @brief Get the balance based on the address
 *
 * @param addr address
 * @return The balance of the address
 */
Energon platon_balance(const Address &addr) {
  byte balance[16];
  size_t len = ::platon_balance(addr.data(), balance);
  return Energon(fromBigEndian<u128>(bytesRef(balance, len)));
}

/**
 * @brief Transfer the amount of Energon to address
 *
 * @param addr Accounts address
 * @param amount The amount of Energon
 * @return true if transfer success, false otherwise
 */
bool platon_transfer(const Address &addr, const Energon &amount);

/**
 * @brief Get the value of the current transaction value field
 *
 * @return The value of the current transaction value field
 */
u128 platon_call_value() {
  byte val[16];
  size_t len = ::platon_call_value(val);
  return fromBigEndian<u128>(bytesRef(val, len));
}

/**
 * @brief Sh3 algorithm
 *
 * @param data Binary data
 * @return The Hash of the data
 */
template <typename T>
h256 platon_sha3(const T &data) {
  byte hash[32];
  ::platon_sha3(data.data(), data.size(), hash, sizeof(hash));
  return h256(hash, sizeof(hash));
}

/**
 * @brief It returns the address from the given signature by calculating a
 * recovery function of ECDSA
 *
 * @param hash The hash value after sha3 hash of the raw data
 * @param signature signature data
 * @param result the signature address
 * @return Getting the signature address returns 0 on success and -1 on failure
 */
template <typename T>
int32_t platon_ecrecover(const h256 &hash, const T &signature,
                         Address &result) {
  int32_t ret = ::platon_ecrecover(hash.data(), signature.data(),
                                   signature.size(), result.data());
  return ret;
}

/**
 * @brief ripemd160 algorithm
 *
 * @param data Binary data
 * @param result The Hash of the data
 */
void platon_ripemd160(const bytes &data, byte result[20]) {
  ::platon_ripemd160(data.data(), data.size(), result);
}

/**
 * @brief sha256 algorithm
 *
 * @param data Binary data
 * @param result The Hash of the data
 */
void platon_sha256(const bytes &data, byte result[32]) {
  ::platon_sha256(data.data(), data.size(), result);
}

/**
 * @brief Set the return value
 *
 * @tparam T Value type
 */
template <typename T>
void platon_return(const T &t);

/**
 * @brief Get the input value
 *
 * @return The input value
 */
bytes get_input();

/**
 * @brief Print the debug info in log
 *
 * @param data The debug info
 */
void platon_debug(const std::string &data) {
  ::platon_debug(reinterpret_cast<const uint8_t *>(data.c_str()), data.size());
}

/**
 * @brief Destory contract
 *
 * @param addr Address of the contract
 * @return true if destroy successfully, false otherwise
 */
bool platon_destroy(const Address &addr) {
  return ::platon_destroy(addr.data()) == 0;
}

/**
 * @brief Migrate contract
 *
 * @param value_type The value type
 * @param gas_type The gas type
 * @param addr The address of new contract
 * @param init_args The input arguments
 * @param value Transfer amount
 * @param gas Pay amount of gas for this transaction
 * @return true if migration successfully, false otherwise
 */
template <typename args_type, typename value_type, typename gas_type>
bool platon_migrate_contract(Address &addr, const args_type &init_args,
                             value_type value, gas_type gas);

/**
 * @brief Cross contract call
 *
 * @param value_type The value type
 * @param gas_type The gas type
 * @param addr Address of contract
 * @param params The input arguments
 * @param value Transfer amount
 * @param gas Pay amount of gas for this transaction
 * @return true if call successfully, false otherwise
 */
template <typename value_type, typename gas_type>
bool platon_call(const Address &addr, const bytes &paras,
                 const value_type &value, const gas_type &gas);

/**
 * @brief Cross contract delegation call
 *
 * tparam gas_type The gas type
 * @param addr Address of contract
 * @param args The input arguments
 * @param call_cost Pay amount of gas for this transaction, big-endian bytes
 * @return 0 if call successfully
 */
template <typename gas_type>
bool platon_delegate_call(const Address &addr, const bytes &paras,
                          const gas_type &gas);

/**
 * @brief Get the value of call output
 *
 * @tparam T The output value type
 * @return The value of call output
 */
template <typename T>
void get_call_output(T &t);

/**
 * @brief Get the address of original caller
 *
 * @return The address of original caller
 */
Address platon_origin();

/**
 * @brief Get the address of caller
 *
 * @return The address of caller
 */
Address platon_caller();

/**
 * @brief Get the address of contract
 *
 * @return The address of contract
 */
Address platon_address();

/**
 * @brief Set the State object
 *
 * @tparam KEY Key type
 * @tparam VALUE Value type
 * @param key Key
 * @param value Value
 */
template <typename KEY, typename VALUE>
void set_state(const KEY &key, const VALUE &value);

/**
 * @brief Get the State object
 *
 * @tparam KEY Key type
 * @tparam VALUE Value type
 * @param key Key
 * @param value Value
 * @return size_t Get the length of the data
 */
template <typename KEY, typename VALUE>
size_t get_state(const KEY &key, VALUE &value);
}  // namespace platon
