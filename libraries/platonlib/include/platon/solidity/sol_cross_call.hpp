#pragma once
#include "platon/chain_impl.hpp"
#include "platon/cross_call.hpp"
#include "platon/solidity/decode.hpp"
#include "platon/solidity/encode.hpp"
#include "platon/solidity/encode_size.hpp"
namespace platon {
namespace solidity {
namespace abi {

/**
 * @brief Construct the parameters of the call across contracts
 *
 * @param method The method name of the invoked contract
 * @param args The parameters corresponding to the contract method
 *
 * @return Parameter byte array
 */
template <typename... Args>
inline bytes sol_call_args(const std::string &method, const Args &... args) {
  h256 id = platon_sha3(method);
  if (sizeof...(args) != 0) {
    std::tuple<Args...> tuple_args = std::make_tuple(args...);
    SolEncodeSize sol_size;
    sol_size.Encode(tuple_args);
    size_t size = sol_size.Size();
    SolEncode sol(size + 4, 4);
    sol.Encode(tuple_args);
    memcpy(sol.Out().out().begin(), id.data(), 4);
    return sol.Out().out().toBytes();
  }
  bytes out(4);
  memcpy(out.data(), id.data(), 4);
  return out;
}

/**
 * @brief Normal cross-contract invocation
 *
 * @param addr The contract address to be invoked
 * @param value The amount transferred to the contract
 * @param gas The called contract method estimates the gas consumed
 * @param method The method name of the invoked contract
 * @param args The parameters corresponding to the contract method
 *
 * @return The contract method returns whether the execution was
 * successful
 *
 * Example:
 *
 * @code
 *
  auto address_pair =
 make_address("lax10jc0t4ndqarj4q6ujl3g3ycmufgc77epxg02lt"); bool result =
 sol_call(address_pair.first,
  uint32_t(100), uint32_t(100), "add", 1,2,3);
  if(!result){
    platon_throw("cross call fail");
  }
 * @endcode
 */
template <typename value_type, typename gas_type, typename... Args>
inline bool sol_call(const Address &addr, const value_type &value,
                     const gas_type &gas, const std::string &method,
                     const Args &... args) {
  bytes paras = sol_call_args(method, args...);
  bytes value_bytes = value_to_bytes(value);
  bytes gas_bytes = value_to_bytes(gas);
  int32_t result =
      ::platon_call(addr.data(), paras.data(), paras.size(), value_bytes.data(),
                    value_bytes.size(), gas_bytes.data(), gas_bytes.size());
  return 0 == result;
}

/**
 * @brief Gets the return value of calling contract methods across contracts
 *
 * @param t The value returned by the contract method
 *
 * @return void
 */
template <typename T>
inline void get_sol_call_output(T &t) {
  bytes result;
  size_t len = ::platon_get_call_output_length();
  result.resize(len);
  ::platon_get_call_output(result.data());
  Decode(t, bytesConstRef(result.data(), result.size()));
}

/**
 * @brief Normal cross-contract invocation
 *
 * @param addr The contract address to be invoked
 * @param value The amount transferred to the contract
 * @param gas The called contract method estimates the gas consumed
 * @param method The method name of the invoked contract
 * @param args The parameters corresponding to the contract method
 *
 * @return The contract method returns the value and whether the execution was
 * successful
 *
 * Example:
 *
 * @code
 *
  auto address_pair =
 make_address("lax10jc0t4ndqarj4q6ujl3g3ycmufgc77epxg02lt"); auto result =
 sol_call_with_return_value<int>(address_pair.first,
  uint32_t(100), uint32_t(100), "add", 1,2,3);
  if(!result.second){
    platon_throw("cross call fail");
  }
 * @endcode
 */
template <typename return_type, typename value_type, typename gas_type,
          typename... Args>
inline auto sol_call_with_return_value(const Address &addr,
                                       const value_type &value,
                                       const gas_type &gas,
                                       const std::string &method,
                                       const Args &... args) {
  bool result = sol_call(addr, value, gas, method, args...);
  if (!result) {
    return std::pair<return_type, bool>(return_type(), false);
  }

  return_type return_value;
  get_sol_call_output(return_value);
  return std::pair<return_type, bool>(return_value, true);
}
}  // namespace abi
}  // namespace solidity
}  // namespace platon
