//
// Created by zhou.yang on 2018/10/19.
//
#pragma once
#include <algorithm>
#include <iterator>
#include <string>
#include <type_traits>
#include <vector>
#include "vector_ref.h"

namespace platon {
using u128 = __uint128_t;
using bigint = u128;

typedef uint8_t byte;

using bytes = std::vector<byte>;
using bytesRef = vector_ref<byte>;
using bytesConstRef = vector_ref<byte const>;

/**
 * @brief Convert hex character to decimal integer.
 * @param _i A hex character.
 * @return -1 if `_i` is not a hex character, otherwise returns the
 * corresponding decimal integer.
 */
inline int fromHexChar(char _i) noexcept {
  if (_i >= '0' && _i <= '9') return _i - '0';
  if (_i >= 'a' && _i <= 'f') return _i - 'a' + 10;
  if (_i >= 'A' && _i <= 'F') return _i - 'A' + 10;
  return -1;
}

/**
 * @brief Convert to bytes represented by hex string s.
 * @param _s Hex string.
 * @return The bytes represented by the hexadecimal string s.
 */
inline bytes fromHex(std::string const& _s) {
  unsigned s = (_s.size() >= 2 && _s[0] == '0' && _s[1] == 'x') ? 2 : 0;
  std::vector<uint8_t> ret;
  ret.reserve((_s.size() - s + 1) / 2);

  if (_s.size() % 2) {
    int h = fromHexChar(_s[s++]);
    if (h != -1)
      ret.push_back(h);
    else
      return bytes();
  }
  for (unsigned i = s; i < _s.size(); i += 2) {
    int h = fromHexChar(_s[i]);
    int l = fromHexChar(_s[i + 1]);
    if (h != -1 && l != -1)
      ret.push_back((byte)(h * 16 + l));
    else
      return bytes();
  }
  return ret;
}

/**
 * @brief Converts a string to a byte array containing the string's (byte) data.
 * @param _b A string.
 * @return Byte array of stringsByte array of strings.
 */
inline bytes asBytes(std::string const& _b) {
  return bytes((byte const*)_b.data(), (byte const*)(_b.data() + _b.size()));
}

/**
 * @brief Convert a series of bytes to the corresponding hex string.
 * @param _it  Iterator start position.
 * @param _end Iterator end position.
 * @param _prefix Prefix string.
 * @return Hex string.
 */
template <class Iterator>
inline std::string toHex(Iterator _it, Iterator _end,
                         std::string const& _prefix) {
  typedef std::iterator_traits<Iterator> traits;
  static_assert(sizeof(typename traits::value_type) == 1,
                "toHex needs byte-sized element type");

  static char const* hexdigits = "0123456789abcdef";
  size_t off = _prefix.size();
  std::string hex(std::distance(_it, _end) * 2 + off, '0');
  hex.replace(0, off, _prefix);
  for (; _it != _end; _it++) {
    hex[off++] = hexdigits[(*_it >> 4) & 0x0f];
    hex[off++] = hexdigits[*_it & 0x0f];
  }
  return hex;
}

/**
 * @brief Convert a series of bytes to the corresponding hex string.
 * @param data A series of bytes.
 * @return Hex string.
 * @example toHex("A\x69") == "4169"
 */
template <class T>
inline std::string toHex(T const& _data) {
  return toHex(_data.begin(), _data.end(), "");
}

/**
 * @brief Calculate the number of bytes required for storage `_i`.
 * @param _i Integer to be calculated.
 * @return Number of bytes.
 */
template <class T>
inline unsigned bytesRequired(T _i) {
  static_assert(
      !std::numeric_limits<T>::is_signed,
      "only unsigned types or bigint supported");  // bigint does not carry sign
                                                   // bit on shift
  unsigned i = 0;
  for (; _i != 0; ++i, _i >>= 8) {
  }
  return i;
}

/**
 * @brief Convert an integer to a byte array of big endian.
 * @param _val  An unsigned integer or bigint.
 * @param o_out Convert results.
 */
template <class T, class Out>
inline void toBigEndian(T _val, Out& o_out) {
  static_assert(
      !std::numeric_limits<T>::is_signed,
      "only unsigned types or bigint supported");  // bigint does not carry sign
                                                   // bit on shift
  for (auto i = o_out.size(); i != 0; _val >>= 8, i--) {
    T v = _val & (T)0xff;
    o_out[i - 1] = (typename Out::value_type)(uint8_t)v;
  }
}

/**
 * @brief Convert a byte array of big endian to an integer.
 * @param _bytes A byte array of big endian.
 * @return An integer.
 */
template <class T, class _In>
inline T fromBigEndian(_In const& _bytes) {
  T ret = (T)0;
  for (auto i : _bytes)
    ret = (T)((ret << 8) |
              (byte)(typename std::make_unsigned<decltype(i)>::type)i);
  return ret;
}
}  // namespace platon
