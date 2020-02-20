//
// Created by yangzhou on 2/20/20.
//

#pragma once

#include "chain.hpp"

namespace platon {
namespace internal {
void assert_inner(bool cond, const char *msg) {
  if (!cond) {
#ifdef NDEBUG
    platon_debug((const uint8_t *)msg, strlen(msg));
#endif
    platon_revert();
  }
}
}  // namespace internal
}  // namespace platon