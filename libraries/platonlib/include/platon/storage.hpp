//
// Created by zhou.yang on 2018/10/22.
//
#pragma once

#include <string>
#include <vector>
#include "RLP.h"
#include "chain.hpp"
#include "common.h"
#include "print.hpp"
#include "rlp_extend.hpp"
#include "rlp_size.hpp"

const uint8_t value_prefix = 0xfe;

namespace platon {
/**
 * @brief Set the State object
 *
 * @tparam KEY Key type
 * @tparam VALUE Value type
 * @param key Key
 * @param value Value
 */
template <typename KEY, typename VALUE>
inline void set_state(const KEY &key, const VALUE &value) {
  RLPStream state_stream;
  auto key_size = pack_size(key);
  state_stream.reserve(key_size);

  state_stream << key;
  const bytesRef vect_key = state_stream.out();

  RLPStream value_stream;
  auto value_size = pack_size(value);
  std::vector<byte> vect_value(sizeof(value_prefix), value_prefix);
  value_stream.appendPrefix(vect_value);
  value_stream.reserve(vect_value.size() + value_size);
  value_stream << value;
  ::platon_set_state(vect_key.data(), vect_key.size(),
                     value_stream.out().data(), value_stream.out().size());
}
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
inline size_t get_state(const KEY &key, VALUE &value) {
  RLPStream state_stream;
  state_stream << key;
  const bytesRef vect_key = state_stream.out();
  size_t len = ::platon_get_state_length(vect_key.data(), vect_key.size());
  if (len == 0) {
    return 0;
  }
  std::vector<byte> result;
  result.resize(len);
  int32_t ret = ::platon_get_state(vect_key.data(), vect_key.size(),
                                   result.data(), result.size());
  if (-1 == ret) {
    return 0;
  }

  fetch(RLP(result.data() + sizeof(value_prefix),
            result.size() - sizeof(value_prefix)),
        value);
  return len;
}

/**
 * @brief delete State Object
 *
 * @tparam KEY Key type
 * @param key Key
 */
template <typename KEY>
inline void del_state(const KEY &key) {
  RLPStream state_stream;
  state_stream << key;
  const bytesRef vect_key = state_stream.out();
  byte del = 0;
  ::platon_set_state(vect_key.data(), vect_key.size(), (const byte *)&del, 0);
}

/**
 * @brief has State Object
 *
 * @tparam KEY Key type
 * @param key Key
 * @return true has state, false does not exist
 */
template <typename KEY>
inline bool has_state(const KEY &key) {
  RLPStream state_stream;
  state_stream << key;
  const bytesRef vect_key = state_stream.out();
  size_t len = ::platon_get_state_length(vect_key.data(), vect_key.size());
  return len != 0;
}

}  // namespace platon
