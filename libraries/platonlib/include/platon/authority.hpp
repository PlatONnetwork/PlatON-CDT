#pragma once
#include "fixedhash.hpp"
#include "storagetype.hpp"
#include "chain.hpp"

namespace platon {

static const size_t address_len = 20;
static const char sys_whitelist_name[] = "$platon$whitelist";
static const char sys_owner_name[] = "$platon$owner";

/**
 * @brief Get the address of the transaction initiator
 *
 * @return The address of the transaction initiator
 */
Address platon_caller() {
  bytes address_bytes;
  address_bytes.resize(address_len);
  ::platon_caller(address_bytes.data());
  return Address(address_bytes);
}


/**
 * @brief Get the address of the original transaction initiator
 *
 * @return The address of the original transaction initiator
 */

Address platon_origin() {
  bytes address_bytes;
  address_bytes.resize(address_len);
  ::platon_origin(address_bytes.data());
  return Address(address_bytes);
}


/**
 * @brief Get the address of contract
 *
 * @return The address of contract
 */

Address platon_address() {
  bytes address_bytes;
  address_bytes.resize(address_len);
  ::platon_address(address_bytes.data());
  return Address(address_bytes);
}

/**
 * @brief Set the address as contracts owner
 *
 * @param address Accounts address, if address empty that
 *                set caller as contract owner
 */
void set_owner(const std::string &address = std::string()) {
  Address platon_addr(address);
  if (address.empty()) {
    platon_addr = platon_caller();
  }

  StorageType<sys_owner_name, Address> owner_addr;
  owner_addr.self() = platon_addr;
}

/**
 * @brief Get the owner of contact
 *
 * @return The address of contract
 */
Address owner() {
  StorageType<sys_owner_name, Address> owner_addr;
  return owner_addr.self();
}

/**
 * @brief Whether the input address is owner of contract
 *
 * @param addr Accounts address, if `addr` empty that set
 *             `addr` to origin caller
 * @return true if the input address is owner, false otherwise
 */
bool is_owner(const std::string &addr = std::string()) {
  Address platon_addr(addr);
  if (addr.empty()) {
    platon_addr = platon_origin();
  }

  StorageType<sys_owner_name, Address> owner_addr;
  return owner_addr.self() == platon_addr;
}

/**
 * @brief Persist storage whitelist implement
 *
 * @tparam Name Whitelist name, in the same contract, the name should be unique
 */
template <const char *Name>
class WhiteList {
 public:
  /**
   * @brief Construct a new whitlist.
   */
  WhiteList() : whitelist_() {}

  /**
   * @brief Add the address to whitelist
   *
   * @param addr Accounts address
   */
  void Add(const std::string &addr) { Add(Address(addr)); }

  /**
   * @brief Add the address to whitelist
   *
   * @param addr Accounts address
   */
  void Add(const Address &addr) { whitelist_.self().insert(addr); }

  /**
   * @brief Delete the address from whitelist
   *
   * @param addr Accounts address
   */
  void Delete(const std::string &addr) { Delete(Address(addr)); }

  /**
   * @brief Delete the address from whitelist
   *
   * @param addr Accounts address
   */
  void Delete(const Address &addr) { whitelist_.self().erase(addr); }

  /**
   * @brief Whether the address exists in whitelist
   *
   * @param addr Accounts address
   * @return true if exists, false otherwise
   */
  bool Exists(const std::string &addr) { return Exists(Address(addr)); }

  /**
   * @brief Whether the address exists in whitelist
   *
   * @param addr Accounts address
   * @return true if exists, false otherwise
   */
  bool Exists(const Address &addr) {
    return whitelist_.self().find(addr) != whitelist_.self().end();
  }

 private:
  Set<Name, Address> whitelist_;
};

/**
 * @brief System default whitelist
 */
using SysWhitelist = WhiteList<sys_whitelist_name>;

}  // namespace platon
