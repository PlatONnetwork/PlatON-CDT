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

namespace platon {

static const size_t address_len = 20;
static const char sys_whitelist_name[] = "$platon$whitelist";
static const char sys_owner_name[] = "$platon$owner";

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

  StorageType<sys_owner_name, Address> owner_addr;
  owner_addr.self() = platon_addr;
}

Address owner() {
  StorageType<sys_owner_name, Address> owner_addr;
  return owner_addr.self();
}

bool is_owner(const std::string &addr = std::string()) {
  Address platon_addr(addr);
  if (addr.empty()) {
    platon_addr = platon_origin_caller();
  }

  StorageType<sys_owner_name, Address> owner_addr;
  return owner_addr.self() == platon_addr;
}

template <const char *Name>
class WhiteList {
 public:
  WhiteList() : whitelist_() {}

  void Add(const std::string &addr) { Add(Address(addr)); }
  void Add(const Address &addr) { whitelist_.self().insert(addr); }

  void Delete(const std::string &addr) { Delete(Address(addr)); }
  void Delete(const Address &addr) { whitelist_.self().erase(addr); }

  bool Exists(const std::string &addr) { return Exists(Address(addr)); }
  bool Exists(const Address &addr) {
    return whitelist_.self().find(addr) != whitelist_.self().end();
  }

 private:
  Set<Name, Address> whitelist_;
};

using SysWhitelist = WhiteList<sys_whitelist_name>;

}  // namespace platon
