#pragma once

#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/include/std_tuple.hpp>
#include "boost/fusion/algorithm/iteration/for_each.hpp"
#include "boost/preprocessor/seq/for_each.hpp"

#include <boost/mp11/tuple.hpp>
#include <tuple>
#include <type_traits>
#include "RLP.h"
#include "chain.hpp"
#include "name.hpp"
#include "panic.hpp"
#include "rlp_extend.hpp"

namespace platon {

inline byte* get_input(size_t& len) {
  len = ::platon_get_input_length();
  byte* result = (byte*)malloc(len * sizeof(byte));
  ::platon_get_input(result);
  return result;
}

template <typename... Args>
void get_para(RLP& rlp, std::tuple<Args...>& t) {
  int vect_index = 1;
  boost::fusion::for_each(t, [&](auto& i) {
    fetch(rlp[vect_index], i);
    vect_index++;
  });
}

template <typename T>
void platon_return(const T& t) {
  RLPStream rlp_stream;
  rlp_stream << t;
  const bytesRef result = rlp_stream.out();
  ::platon_return(result.data(), result.size());
}

/**
 * Unpack the received action and execute the correponding action handler
 *
 * @tparam T - The contract class that has the correponding action handler, this
 * contract should be derived from ontio::contract
 * @tparam Q - The namespace of the action handler function
 * @tparam Args - The arguments that the action handler accepts, i.e. members of
 * the action
 * @param obj - The contract object that has the correponding action handler
 * @param func - The action handler
 * @return true
 */
template <typename T, typename R, typename... Args>
void execute_action(RLP& rlp, R (T::*func)(Args...)) {
  std::tuple<std::decay_t<Args>...> args;
  get_para(rlp, args);

  T inst;

  auto f2 = [&](auto... a) {
    R&& t = ((&inst)->*func)(a...);
    platon_return<R>(t);
  };

  boost::mp11::tuple_apply(f2, args);
}

template <typename T, typename... Args>
void execute_action(RLP& rlp, void (T::*func)(Args...)) {
  std::tuple<std::decay_t<Args>...> args;
  get_para(rlp, args);

  T inst;

  auto f2 = [&](auto... a) { ((&inst)->*func)(a...); };

  boost::mp11::tuple_apply(f2, args);
}

// Helper macro for EOSIO_DISPATCH_INTERNAL
#define PLATON_DISPATCH_INTERNAL(r, OP, elem)                        \
  else if (method == platon::name_value(BOOST_PP_STRINGIZE(elem))) { \
    platon::execute_action(rlp, &OP::elem);                          \
  }

// Helper macro for PLATON_DISPATCH
#define PLATON_DISPATCH_HELPER(TYPE, MEMBERS) \
  BOOST_PP_SEQ_FOR_EACH(PLATON_DISPATCH_INTERNAL, TYPE, MEMBERS)

/**
 * @addtogroup dispatcher
 * Convenient macro to create contract apply handler
 *
 * @note To be able to use this macro, the contract needs to be derived from
 * platon::contract
 * @param TYPE - The class name of the contract
 * @param MEMBERS - The sequence of available actions supported by this contract
 *
 * Example:
 * @code
 * PLATON_DISPATCH( hello,
 * (init)(set_message)(change_message)(delete_message)(get_message) )
 * @endcode
 */

#define PLATON_DISPATCH(TYPE, MEMBERS)                       \
  extern "C" {                                               \
  void __wasm_call_ctors();                                  \
  void invoke(void) {                                        \
    __wasm_call_ctors();                                     \
    size_t len = 0;                                          \
    auto input = platon::get_input(len);                     \
    platon::RLP rlp(input, len);                             \
    uint64_t method = 0;                                     \
    fetch(rlp[0], method);                                   \
    if (0 == method) {                                       \
      platon::internal::platon_throw("valid method\n");      \
    }                                                        \
    PLATON_DISPATCH_HELPER(TYPE, MEMBERS)                    \
    else {                                                   \
      platon::internal::platon_throw("no method to call\n"); \
    }                                                        \
  }                                                          \
  }

}  // namespace platon
