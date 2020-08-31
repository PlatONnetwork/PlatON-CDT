#pragma once
#include "chain.hpp"
#include "cross_call.hpp"
#include "fixedhash.hpp"

namespace platon {

bytes platon_contract_code(const Address &contract_address) {
  bytes code;
  size_t code_length = platon_contract_code_length(contract_address.data());
  if (code_length == 0) return code;
  code.resize(code_length);
  ::platon_contract_code(contract_address.data(), code.data(), code_length);
  return code;
}

template <typename value_type, typename gas_type, typename... Args>
std::pair<Address, bool> platon_create_contract(const bytes &code,
                                                value_type value, gas_type gas,
                                                const Args &... init_args) {
  // value and gas
  bytes value_bytes = value_to_bytes(value);
  bytes gas_bytes = value_to_bytes(gas);

  // deploy args
  std::vector<byte> magic_number = {0x00, 0x61, 0x73, 0x6d};
  bytes init_rlp = cross_call_args("init", init_args...);
  RLPSize rlps;
  rlps << code << init_rlp;
  RLPStream stream;
  stream.appendPrefix(magic_number);
  stream.reserve(rlps.size() + 4);
  stream.appendList(2);
  stream << code << init_rlp;
  bytesRef result = stream.out();

  // deploy contract
  Address return_address;
  bool success =
      platon_deploy(return_address.data(), result.data(), result.size(),
                    value_bytes.data(), value_bytes.size(), gas_bytes.data(),
                    gas_bytes.size()) == 0;

  return std::make_pair(return_address, success);
}

template <typename value_type, typename gas_type, typename... Args>
std::pair<Address, bool> platon_create_contract(const Address &address,
                                                value_type value, gas_type gas,
                                                const Args &... init_args) {
  // value and gas
  bytes value_bytes = value_to_bytes(value);
  bytes gas_bytes = value_to_bytes(gas);

  // init args
  bytes init_rlp = cross_call_args("init", init_args...);

  // clone contract
  Address return_address;
  bool success =
      platon_clone(address.data(), return_address.data(), init_rlp.data(),
                   init_rlp.size(), value_bytes.data(), value_bytes.size(),
                   gas_bytes.data(), gas_bytes.size()) == 0;
  return std::make_pair(return_address, success);
}

}  // namespace platon
