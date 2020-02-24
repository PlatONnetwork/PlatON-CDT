#pragma once
#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/include/std_tuple.hpp>
#include <boost/mp11/tuple.hpp>
#include <tuple>

#include "RLP.h"
#include "boost/fusion/algorithm/iteration/for_each.hpp"
#include "boost/preprocessor/seq/for_each.hpp"
#include "chain.hpp"
#include "common.h"
#include "container/string.hpp"
#include "fixedhash.hpp"
#include "rlp_extend.hpp"

namespace platon {

template <typename... Args>
inline bytes cross_call_args(const container::string &method,
                             const Args &... args) {
  RLPStream stream;
  std::tuple<Args...> tuple_args = std::make_tuple(args...);
  size_t num = sizeof...(Args);
  stream.appendList(num + 1);
  stream << method;
  boost::fusion::for_each(tuple_args, [&](const auto &i) { stream << i; });
  return stream.out();
}

template <typename T>
inline bytes value_to_bytes(T value) {
  unsigned byte_count = bytesRequired(value);
  bytes result;
  result.resize(byte_count);
  byte *b = &result.back();
  for (; value; value >>= 8) *(b--) = (byte)value;
  return result;
}

template <typename value_type, typename gas_type>
inline bool platon_call(const Address &addr, const bytes &paras,
                        const value_type &value, const gas_type &gas) {
  bytes value_bytes = value_to_bytes(value);
  bytes gas_bytes = value_to_bytes(gas);
  return ::platon_call(addr.data(), paras.data(), paras.size(),
                       value_bytes.data(), value_bytes.size(), gas_bytes.data(),
                       gas_bytes.size()) == 0;
}

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

template <typename T>
inline void get_call_output(T &t) {
  bytes result;
  size_t len = ::platon_get_call_output_length();
  result.resize(len);
  ::platon_get_call_output(result.data());
  fetch(RLP(result), t);
}

}  // namespace platon
