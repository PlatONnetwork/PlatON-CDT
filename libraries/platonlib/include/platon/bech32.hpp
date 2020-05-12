#pragma once
#include "fixedhash.hpp"

namespace platon {

/** The Bech32 character set for encoding. */
const char* charset = "qpzry9x8gf2tvdw0s3jn54khce6mua7l";

/** The Bech32 character set for decoding. */
const int8_t charset_rev[128] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 15, -1, 10, 17, 21, 20, 26, 30, 7,
    5,  -1, -1, -1, -1, -1, -1, -1, 29, -1, 24, 13, 25, 9,  8,  23, -1, 18, 22,
    31, 27, 19, -1, 1,  0,  3,  16, 11, 28, 12, 14, 6,  4,  2,  -1, -1, -1, -1,
    -1, -1, 29, -1, 24, 13, 25, 9,  8,  23, -1, 18, 22, 31, 27, 19, -1, 1,  0,
    3,  16, 11, 28, 12, 14, 6,  4,  2,  -1, -1, -1, -1, -1};

/** Concatenate two byte arrays. */
inline bytes cat(bytes x, const bytes& y) {
  x.insert(x.end(), y.begin(), y.end());
  return x;
}

/**
 * @brief Find the polynomial with value coefficients mod the generator as 30-bit.
 * 
 * @param values values.
 * 
 * @return The resulting data. 
 * 
 */
inline uint32_t polymod(const bytes& values) {
  uint32_t chk = 1;
  for (size_t i = 0; i < values.size(); ++i) {
    uint8_t top = chk >> 25;
    chk = (chk & 0x1ffffff) << 5 ^ values[i] ^
          (-((top >> 0) & 1) & 0x3b6a57b2UL) ^
          (-((top >> 1) & 1) & 0x26508e6dUL) ^
          (-((top >> 2) & 1) & 0x1ea119faUL) ^
          (-((top >> 3) & 1) & 0x3d4233ddUL) ^
          (-((top >> 4) & 1) & 0x2a1462b3UL);
  }
  return chk;
}


/**
 * @brief Expand a HRP for use in checksum computation.
 * 
 * @param hrp The human-readable part of the address.
 * 
 * @return The resulting data. 
 * 
 */
inline bytes expand_hrp(const std::string& hrp) {
  bytes ret;
  ret.resize(hrp.size() * 2 + 1);
  for (size_t i = 0; i < hrp.size(); ++i) {
    unsigned char c = hrp[i];
    ret[i] = c >> 5;
    ret[i + hrp.size() + 1] = c & 0x1f;
  }
  ret[hrp.size()] = 0;
  return ret;
}


/**
 * @brief Verify a checksum.
 * 
 * @param hrp The human-readable part of the address.
 * @param values The data part.
 * 
 * @return Verify success or failure. 
 * 
 */
inline bool verify_checksum(const std::string& hrp, const bytes& values) {
  return polymod(cat(expand_hrp(hrp), values)) == 1;
}

/**
 * @brief Create a checksum.
 * 
 * @param hrp The human-readable part of the address.
 * @param values The data part.
 * 
 * @return checksum data. 
 * 
 */
inline bytes create_checksum(const std::string& hrp, const bytes& values) {
  bytes enc = cat(expand_hrp(hrp), values);
  enc.resize(enc.size() + 6);
  uint32_t mod = polymod(enc) ^ 1;
  bytes ret;
  ret.resize(6);
  for (size_t i = 0; i < 6; ++i) {
    ret[i] = (mod >> (5 * (5 - i))) & 31;
  }
  return ret;
}

/**
 * @brief Convert from the encoding byte of the frombits to the encoding byte of the tobits bit.
 * 
 * @param out Output bytes.
 * @param in Input bytes.
 * 
 * @return Whether the conversion was successful or not. 
 * 
 */
template <int frombits, int tobits, bool pad>
bool convertbits(bytes& out, const bytes& in) {
  int acc = 0;
  int bits = 0;
  const int maxv = (1 << tobits) - 1;
  const int max_acc = (1 << (frombits + tobits - 1)) - 1;
  for (size_t i = 0; i < in.size(); ++i) {
    int value = in[i];
    acc = ((acc << frombits) | value) & max_acc;
    bits += frombits;
    while (bits >= tobits) {
      bits -= tobits;
      out.push_back((acc >> bits) & maxv);
    }
  }
  if (pad) {
    if (bits) out.push_back((acc << (tobits - bits)) & maxv);
  } else if (bits >= frombits || ((acc << (tobits - bits)) & maxv)) {
    return false;
  }
  return true;
}

/**
 * @brief Do a Bech32 on an Address of type Address to get the string Address.
 * 
 * @param address Address of type Address.
 * @param hrp The human-readable part of the address.
 * 
 * @return The string address encoded by Bech32. 
 * 
 */
inline std::string encode(const Address& address, const std::string& hrp) {
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
inline std::pair<Address, bool> decode(const std::string& str_address,
                                       const std::string& str_hrp) {
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
 * @brief Converts the literal Address of a string to an Address of type Address.
 * The default is the address format of the main network. 
 * To get the address format of the test network, define the macro TESTNET.
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
  std::string hrp = "lax";
#else
  std::string hrp = "lat";
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
inline std::pair<Address, bool> make_address(const std::string& str_address) {
#ifdef TESTNET
  std::string hrp = "lax";
#else
  std::string hrp = "lat";
#endif

  return decode(str_address, hrp);
}

/**
 * @brief Converts the address to a string.
 * The default is the address format of the main network. 
 * To get the address format of the test network, define the macro TESTNET.
 *
 * @param address An Address of type Address
 *
 * @return The address represented by a string
 */
inline std::string get_string_address(const Address& address) {
#ifdef TESTNET
  std::string hrp = "lax";
#else
  std::string hrp = "lat";
#endif

  return encode(address, hrp);
}

}  // namespace platon