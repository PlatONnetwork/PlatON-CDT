#pragma once

#include "common.h"

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
}  // namespace platon