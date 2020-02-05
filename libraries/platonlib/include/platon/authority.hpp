#pragma once
#include "fixedhash.hpp"
#include "storagetype.hpp"

#ifdef __cplusplus
extern "C" {
#endif

void platon_origin(uint8_t addr[20]);
void platon_caller(uint8_t addr[20]);
void platon_address(uint8_t addr[20]);

#ifdef __cplusplus
}
#endif

#define STATIC_OWNER(addr)                                                     \
  static const std::set<platon::Address> kStaticOwner{platon::Address(addr)};

#define IS_STATIC_OWNER(addr)                                   \
  kStaticOwner.find(platon::Address(addr)) != kStaticOwner.end()

namespace platon {

static const size_t address_len = 20;

Address platon_caller() {
  bytes address_bytes;
  address_bytes.resize(address_len);
  ::platon_caller(address_bytes.data());
  return Address(address_bytes);
}

        Address platon_origin_caller() {
  bytes address_bytes;
  address_bytes.resize(address_len);
  ::platon_origin(address_bytes.data());
  return Address(address_bytes);
}

Address platon_contract_address() {
  bytes address_bytes;
  address_bytes.resize(address_len);
  ::platon_address(address_bytes.data());
  return Address(address_bytes);
}

void set_owner(const std::string &address = std::string()) {
  Address platon_addr(address);
  if (address.empty()) {
    platon_addr = platon_caller();
  }

  static const char contract_owner[] = "owner";
  StorageType<contract_owner, Address> owner_addr;
  owner_addr.self() = platon_addr;
}

Address owner() {
  static const char contract_owner[] = "owner";
  StorageType<contract_owner, Address> owner_addr;
  return owner_addr.self();
}

bool is_owner(const std::string &addr = std::string()) {
  Address platon_addr(addr);
  if (addr.empty()) {
    platon_addr = platon_origin_caller();
  }

  static const char contract_owner[] = "owner";
  StorageType<contract_owner, Address> owner_addr;
  return owner_addr.self() == platon_addr;
}

void add_to_whitelist(const std::string &addr) {
  static const char whitelist_name[] = "whitelist";
  Set<whitelist_name, Address> whitelist;
  whitelist.self().emplace(addr);
}

bool in_whitelist(const std::string &addr = std::string()) {
  Address platon_addr(addr);
  if (addr.empty()) {
    platon_addr = platon_origin_caller();
  }

  static const char whitelist_name[] = "whitelist";
  Set<whitelist_name, Address> whitelist;

  if (whitelist.self().find(platon_addr) != whitelist.self().end()) {
    return true;
  }
  return false;
}

} // namespace platon
