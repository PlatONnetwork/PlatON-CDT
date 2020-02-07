#pragma once
#include <stdint.h>
#include "authority.hpp"
#include "chain.hpp"
#include "cross_call.hpp"
#include "fixedhash.hpp"

namespace platon {

template <typename value_type, typename gas_type>
bool platon_migrate_contract(Address &addr, const bytes &init_args,
                             value_type value, gas_type gas) {
  // value and gas
  bytes value_bytes = value_to_bytes(value);
  bytes gas_bytes = value_to_bytes(gas);

  // call platon_migrate
  return ::platon_migrate(addr.data(), init_args.data(), init_args.size(),
                          value_bytes.data(), value_bytes.size(),
                          gas_bytes.data(), gas_bytes.size()) == 0;
}

}  // namespace platon
