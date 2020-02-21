#pragma once

#include "chain.hpp"
#include "cstring"

namespace platon {
namespace internal {

inline void platon_throw(const char * msg) {
#ifndef PLATON_NDEBUG
  ::platon_debug((const uint8_t *)msg, strlen(msg));
#endif
  ::platon_panic();
}

}  // namespace internal
}  // namespace platon