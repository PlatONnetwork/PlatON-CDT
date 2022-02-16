#pragma once
#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/include/std_tuple.hpp>
#include <boost/mp11/tuple.hpp>
#include <map>
#include <tuple>
#include "boost/fusion/algorithm/iteration/for_each.hpp"
#include "boost/preprocessor/seq/for_each.hpp"

#include "RLP.h"
#include "chain.hpp"
#include "common.h"
#include "fixedhash.hpp"
#include "name.hpp"
#include "rlp_extend.hpp"

namespace platon {

/**
 * @brief Construct the parameters of the call across contracts
 *
 * @param method The method name of the invoked contract
 * @param args The parameters corresponding to the contract method
 *
 * @return Parameter byte array
 */
template <typename... Args>
inline bytes cross_call_args(const std::string &method,
                                     const Args &... args) {
  uint64_t t_method = Name(method).value;
  RLPStream stream;
  std::tuple<Args...> tuple_args = std::make_tuple(args...);
  size_t num = sizeof...(Args);
  stream.appendList(num + 1);
  RLPSize rlps;
  rlps << t_method;
  boost::fusion::for_each(tuple_args, [&](const auto &i) { rlps << i; });
  stream.reserve(rlps.size());
  stream << t_method;
  boost::fusion::for_each(tuple_args, [&](const auto &i) { stream << i; });
  return stream.out().toBytes();
}

/**
 * @brief Converts the data to a big-end representation byte array
 *
 * @param value Data values to be converted
 *
 * @return A byte array of data
 */
template <typename T>
inline bytes value_to_bytes(T value) {
  unsigned byte_count = bytesRequired(value);
  bytes result;
  result.resize(byte_count);
  byte *b = &result.back();
  for (; value; value >>= 8) *(b--) = (byte)value;
  return result;
}

/**
 * @brief Normal cross-contract invocation
 *
 * @param addr The contract address to be invoked
 * @param paras A contract parameter constructed using the function
 * cross_call_args
 * @param gas The called contract method estimates the gas consumed
 * @param value The amount transferred to the contract
 *
 * @return The call succeeds or fails
 */
template <typename value_type, typename gas_type>
inline bool platon_call(const Address &addr, const bytes &paras,
                        const value_type &value, const gas_type &gas) {
  bytes value_bytes = value_to_bytes(value);
  bytes gas_bytes = value_to_bytes(gas);
  return ::platon_call(addr.data(), paras.data(), paras.size(),
                       value_bytes.data(), value_bytes.size(), gas_bytes.data(),
                       gas_bytes.size()) == 0;
}

/**
 * @brief The proxy is invoked across contracts
 *
 * @param addr The contract address to be invoked
 * @param paras A contract parameter constructed using the function
 * cross_call_args
 * @param gas The called contract method estimates the gas consumed
 *
 * @return The call succeeds or fails
 */
template <typename gas_type>
inline bool platon_delegate_call(const Address &addr, const bytes &paras,
                                 const gas_type &gas) {
  bytes gas_bytes = value_to_bytes(gas);
  return ::platon_delegate_call(addr.data(), paras.data(), paras.size(),
                                gas_bytes.data(), gas_bytes.size()) == 0;
}

// template<typename gas_type>
// int32_t platon_static_call(const std::string &str_address, const bytes paras,
// const gas_type &gas) {
//     Address contrace_address(str_address);
//     bytes gas_bytes = value_to_bytes(gas);
//     return ::platon_static_call(contrace_address.data(), paras.data(),
//     paras.size(), gas_bytes.data(), gas_bytes.size());
// }

/**
 * @brief Gets the return value of calling contract methods across contracts
 *
 * @param t The value returned by the contract method
 *
 * @return void
 */
template <typename T>
inline void get_call_output(T &t) {
  bytes result;
  size_t len = ::platon_get_call_output_length();
  result.resize(len);
  ::platon_get_call_output(result.data());
  fetch(RLP(result), t);
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
 platon_call(address_pair.first,
  uint32_t(100), uint32_t(100), "add", 1,2,3);
  if(!result){
    platon_throw("cross call fail");
  }
 * @endcode
 */
template <typename value_type, typename gas_type, typename... Args>
inline bool platon_call(const Address &addr, const value_type &value,
                        const gas_type &gas, const std::string &method,
                        const Args &... args) {
  bytes paras = cross_call_args(method, args...);
  bytes value_bytes = value_to_bytes(value);
  bytes gas_bytes = value_to_bytes(gas);
  int32_t result =
      ::platon_call(addr.data(), paras.data(), paras.size(), value_bytes.data(),
                    value_bytes.size(), gas_bytes.data(), gas_bytes.size());
  return 0 == result;
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
 platon_call_with_return_value<int>(address_pair.first,
  uint32_t(100), uint32_t(100), "add", 1,2,3);
  if(!result.second){
    platon_throw("cross call fail");
  }
 * @endcode
 */
template <typename return_type, typename value_type, typename gas_type,
          typename... Args>
inline auto platon_call_with_return_value(const Address &addr,
                                          const value_type &value,
                                          const gas_type &gas,
                                          const std::string &method,
                                          const Args &... args) {
  bool result = platon_call(addr, value, gas, method, args...);
  if (!result) {
    return std::pair<return_type, bool>(return_type(), false);
  }

  return_type return_value;
  get_call_output(return_value);
  return std::pair<return_type, bool>(return_value, true);
}

/**
 * @brief The proxy is invoked across contracts
 *
 * @param addr The contract address to be invoked
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
  auto address_pair =make_address("lax10jc0t4ndqarj4q6ujl3g3ycmufgc77epxg02lt");
  bool result = platon_delegate_call<int>(address_pair.first, uint32_t(100),
 "add", 1,2,3); if(!result){ platon_throw("cross call fail");
  }

 * @endcode
 */
template <typename gas_type, typename... Args>
inline bool platon_delegate_call(const Address &addr, const gas_type &gas,
                                 const std::string &method,
                                 const Args &... args) {
  bytes paras = cross_call_args(method, args...);
  bytes gas_bytes = value_to_bytes(gas);
  int32_t result =
      ::platon_delegate_call(addr.data(), paras.data(), paras.size(),
                             gas_bytes.data(), gas_bytes.size());
  return 0 == result;
}

/**
 * @brief The proxy is invoked across contracts
 *
 * @param addr The contract address to be invoked
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
 platon_delegate_call_with_return_value<int>(address_pair.first,
  uint32_t(100), "add", 1,2,3);
  if(!result.secnod){
    platon_throw("cross call fail");
  }

 * @endcode
 */
template <typename return_type, typename gas_type, typename... Args>
inline auto platon_delegate_call_with_return_value(const Address &addr,
                                                   const gas_type &gas,
                                                   const std::string &method,
                                                   const Args &... args) {
  bool result = platon_delegate_call(addr, gas, method, args...);
  if (!result) {
    return std::pair<return_type, bool>(return_type(), false);
  }

  return_type return_value;
  get_call_output(return_value);
  return std::pair<return_type, bool>(return_value, true);
}

}  // namespace platon
