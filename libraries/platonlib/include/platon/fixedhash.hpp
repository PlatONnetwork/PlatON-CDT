#pragma once

#include <array>
#include <initializer_list>
#include <string>
#include <vector>
#include "bech32.hpp"
#include "common.h"

namespace platon {

template <unsigned N>
class FixedHash {
 public:
  using iterator = typename std::array<byte, N>::iterator;
  iterator begin() noexcept { return m_data.begin(); }
  iterator end() noexcept { return m_data.end(); }

  using const_iterator = typename std::array<byte, N>::const_iterator;
  const_iterator cbegin() const noexcept { return m_data.cbegin(); }
  const_iterator cend() const noexcept { return m_data.cend(); }

 public:
  /// The size of the container.
  enum { size = N };

  /// Construct an empty hash.
  FixedHash() {}

  template <unsigned M>
  explicit FixedHash(FixedHash<M> const &h) {
    unsigned c = std::min(M, N);
    memcpy(m_data.data(), h.m_data.data(), c);
    for (; c < N; ++c) {
      m_data[c] = 0;
    }
  }

  template <typename T>
  FixedHash(const std::initializer_list<T> &l) {
    m_data.fill(0);
    unsigned i = 0;
    for (auto it = l.begin(); it != l.end() && i < N; ++it, ++i) {
      m_data[i] = *it;
    }
  }

  explicit FixedHash(u128 _u) { toBigEndian(_u, m_data); }

  explicit FixedHash(const byte *h, size_t len) {
    m_data.fill(0);
    size_t real_len = len < N ? len : N;
    memcpy(m_data.data(), h, real_len);
  }

  explicit FixedHash(const bytes &&b) {
    m_data.fill(0);
    unsigned i = 0;
    for (auto it = b.begin(); it != b.end() && i < N; ++it, ++i) {
      m_data[i] = *it;
    }
  }

  explicit FixedHash(const bytes &b) {
    m_data.fill(0);
    unsigned i = 0;
    for (auto it = b.begin(); it != b.end() && i < N; ++it, ++i) {
      m_data[i] = *it;
    }
  }

  explicit FixedHash(const std::string &s, bool isHex = true)
      : FixedHash(isHex ? fromHex(s) : asBytes(s)) {
    static_assert(
        20 != N,
        "Converting hexadecimal strings to addresses is not supported");
  }

  constexpr explicit FixedHash(const std::string_view &s) {
    fromHex<N>(s, m_data);
  }

  std::string toString() const { return "0x" + toHex(m_data); }

  bytes toBytes() const { return bytes(data(), data() + N); }
  byte const *data() const { return m_data.data(); }
  byte *data() { return m_data.data(); }
  bytesRef ref() { return bytesRef(m_data.data(), N); }
  /// @returns true iff this is the empty hash.
  explicit operator bool() const {
    return std::any_of(m_data.begin(), m_data.end(),
                       [](byte _b) { return _b != 0; });
  }

  // The obvious comparison operators.
  bool operator==(FixedHash const &c) const { return m_data == c.m_data; }
  bool operator!=(FixedHash const &c) const { return m_data != c.m_data; }
  bool operator<(FixedHash const &c) const {
    for (unsigned i = 0; i < N; ++i)
      if (m_data[i] < c.m_data[i])
        return true;
      else if (m_data[i] > c.m_data[i])
        return false;
    return false;
  }
  bool operator>=(FixedHash const &c) const { return !operator<(c); }
  bool operator<=(FixedHash const &c) const {
    return operator==(c) || operator<(c);
  }
  bool operator>(FixedHash const &c) const { return !operator<=(c); }

  // The obvious binary operators.
  FixedHash &operator^=(FixedHash const &c) {
    for (unsigned i = 0; i < N; ++i) m_data[i] ^= c.m_data[i];
    return *this;
  }
  FixedHash operator^(FixedHash const &c) const {
    return FixedHash(*this) ^= c;
  }
  FixedHash &operator|=(FixedHash const &c) {
    for (unsigned i = 0; i < N; ++i) m_data[i] |= c.m_data[i];
    return *this;
  }
  FixedHash operator|(FixedHash const &c) const {
    return FixedHash(*this) |= c;
  }
  FixedHash &operator&=(FixedHash const &c) {
    for (unsigned i = 0; i < N; ++i) m_data[i] &= c.m_data[i];
    return *this;
  }
  FixedHash operator&(FixedHash const &c) const {
    return FixedHash(*this) &= c;
  }
  FixedHash operator~() const {
    FixedHash ret;
    for (unsigned i = 0; i < N; ++i) ret[i] = ~m_data[i];
    return ret;
  }

  /// @returns a particular byte from the hash.
  byte &operator[](unsigned _i) { return m_data[_i]; }
  /// @returns a particular byte from the hash.
  byte operator[](unsigned _i) const { return m_data[_i]; }

  // Big-endian increment.
  FixedHash &operator++() {
    for (unsigned i = size(); i > 0 && !++m_data[--i];) {
    }
    return *this;
  }

  /// @returns true if all one-bits in @a _c are set in this object.
  bool contains(FixedHash const &_c) const { return (*this & _c) == _c; }

 private:
  std::array<byte, N> m_data = {};
};

using h256 = FixedHash<32>;
using h128 = FixedHash<16>;
using h64 = FixedHash<8>;
using Address = FixedHash<20>;

/**
 * @brief Do a Bech32 on an Address of type Address to get the string Address.
 *
 * @param address Address of type Address.
 * @param hrp The human-readable part of the address.
 *
 * @return The string address encoded by Bech32.
 *
 */
inline std::string encode(const Address &address, const std::string &hrp) {
  bytes values;
  convertbits<8, 5, true>(values, address.toBytes());
  bytes checksum = create_checksum(hrp, values);
  bytes combined = cat(values, checksum);
  std::string ret = hrp + '1';
  ret.reserve(ret.size() + combined.size());
  for (size_t i = 0; i < combined.size(); ++i) {
    ret += charset[combined[i]];
  }
  return ret;
}

/**
 * @brief Decode the Bech32 encoded string Address to the Address type Address.
 *
 * @param str_address the Bech32 encoded string Address.
 * @param str_hrp The human-readable part of the address.
 *
 * @return The first of a pair represents the Address in the Address format,
 * and the second indicates whether the conversion was successful.
 */
inline std::pair<Address, bool> decode(const std::string &str_address,
                                       const std::string &str_hrp) {
  bool lower = false, upper = false;
  bool ok = true;
  for (size_t i = 0; ok && i < str_address.size(); ++i) {
    unsigned char c = str_address[i];
    if (c < 33 || c > 126) ok = false;
    if (c >= 'a' && c <= 'z') lower = true;
    if (c >= 'A' && c <= 'Z') upper = true;
  }
  if (lower && upper) ok = false;
  size_t pos = str_address.rfind('1');
  if (ok && str_address.size() <= 90 && pos != str_address.npos && pos >= 1 &&
      pos + 7 <= str_address.size()) {
    bytes values;
    values.resize(str_address.size() - 1 - pos);
    for (size_t i = 0; i < str_address.size() - 1 - pos; ++i) {
      unsigned char c = str_address[i + pos + 1];
      if (charset_rev[c] == -1) ok = false;
      values[i] = charset_rev[c];
    }
    if (ok) {
      std::string hrp;
      for (size_t i = 0; i < pos; ++i) {
        hrp += std::tolower(str_address[i]);
      }
      if (hrp != str_hrp) {
        return std::make_pair(Address(), false);
      }
      if (verify_checksum(hrp, values)) {
        bytes conv;
        bool ok_convert = convertbits<5, 8, false>(
            conv, bytes(values.begin(), values.end() - 6));
        if (ok_convert && 20 == conv.size()) {
          return std::make_pair(Address(conv), true);
        }
      }
    }
  }
  return std::make_pair(Address(), false);
}

/**
 * @brief Converts the literal Address of a string to an Address of type
 * Address. The default is the address format of the main network. To get the
 * address format of the test network, define the macro TESTNET.
 *
 * @param str_address the literal Address of a string
 *
 * @return The first of a pair represents the Address in the Address format,
 * and the second indicates whether the conversion was successful.
 */
template <size_t M>
std::pair<Address, bool> make_address(const char (&str_address)[M]) {
  constexpr size_t N = 42;

  // The address string must begin with 0x and be 43 bytes in size
  static_assert(M - 1 == 42, "Incorrect string length");

#ifdef TESTNET
  std::string hrp = "atx";
#else
  std::string hrp = "atp";
#endif

  return decode(str_address, hrp);
}

/**
 * @brief Converts an address in string format to an address of type Address.
 * The default is the address format of the main network.
 * To get the address format of the test network, define the macro TESTNET.
 *
 * @param str_address The address represented by a string
 *
 * @return The first of a pair represents the Address in the Address format,
 * and the second indicates whether the conversion was successful.
 */
inline std::pair<Address, bool> make_address(const std::string &str_address) {
#ifdef TESTNET
  std::string hrp = "atx";
#else
  std::string hrp = "atp";
#endif

  return decode(str_address, hrp);
}

template <>
std::string FixedHash<20>::toString() const {
#ifdef TESTNET
  std::string hrp = "atx";
#else
  std::string hrp = "atp";
#endif
  return encode(*this, hrp);
}
}  // namespace platon
