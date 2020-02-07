#pragma once
#include <string>
#include <vector>

#include "chain.hpp"
#include "common.h"

namespace platon {

inline std::vector<byte> get_input(void) {
  std::vector<byte> result;
  size_t len = ::platon_get_input_length();
  result.resize(len);
  ::platon_get_input(result.data());
  return result;
}

inline void platon_throw(const std::string msg) {
  size_t len = msg.length();
  ::platon_debug((uint8_t *)msg.c_str(), len);
  ::platon_panic();
}

}  // namespace platon
