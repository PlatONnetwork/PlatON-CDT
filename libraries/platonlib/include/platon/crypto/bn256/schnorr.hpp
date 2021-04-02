#pragma once
#include "platon/crypto/bn256/bn256.hpp"
namespace platon {
namespace crypto {
namespace bn256 {
/**
 * @brief Schnorr protocol
 *
 * https://en.wikipedia.org/wiki/Proof_of_knowledge#Schnorr_protocol
 */
class Schnorr {
 public:
  /**
   * @brief Create Schnorr signature
   *
   * @param k secret
   * @param m message hash
   */
  static std::tuple<G1, std::uint256_t> Sign(const std::uint256_t &k,
                                             const std::uint256_t &m) {
    // P = G*k
    G1 P = G1::ScalarBaseMul(k);
    // r = hash(k,m)
    std::uint256_t r = Hash(k, m);
    // R = G*r
    G1 R = G1::ScalarBaseMul(r);
    // e = H(R,P,m)
    std::uint256_t e = Hash(R.X(), R.Y(), P.X(), P.Y(), m);
    // s = r + k*e
    std::uint256_t s = AddMod(r, MulMod(k, e, Order), Order);
    return {R, s};
  }

  /**
   * @brief Verify Schnorr signature
   *
   * @param R random
   * @param s signature
   * @param P pubkey
   * @param m message
   */
  static bool Verify(const G1 &R, const std::uint256_t &s, const G1 &P,
                     const std::uint256_t &m) {
    // sG = G*s
    G1 sG = G1::ScalarBaseMul(s);
    G1 V = CalcProof(R, s, P, m);
    return sG.X() == V.X() && sG.Y() == V.Y();
  }

 private:
  static G1 CalcProof(const G1 &R, const std::uint256_t &s, const G1 &P,
                      const std::uint256_t &m) {
    // e = H(R,P,m)
    std::uint256_t e = Hash(R.X(), R.Y(), P.X(), P.Y(), m);

    G1 P1 = P;
    G1 R1 = R;
    // R + P*e
    R1.Add(P1.ScalarMul(e));
    return R1;
  }

  static std::uint256_t Hash(const std::uint256_t &r, const std::uint256_t &m) {
    std::array<uint8_t, 64> data;
    memcpy(data.data(), r.Values(), 32);
    memcpy(data.data() + 32, m.Values(), 32);
    std::uint256_t k;
    platon_sha3(data.data(), data.size(), k.Values(), 32);
    return k;
  }

  static std::uint256_t Hash(const std::uint256_t &rx, const std::uint256_t &ry,
                             const std::uint256_t &px, const std::uint256_t &py,
                             const std::uint256_t &m) {
    std::array<uint8_t, 160> data;
    memcpy(data.data(), rx.Values(), 32);
    memcpy(data.data() + 32, ry.Values(), 32);
    memcpy(data.data() + 64, px.Values(), 32);
    memcpy(data.data() + 96, py.Values(), 32);
    memcpy(data.data() + 128, m.Values(), 32);
    std::uint256_t k;
    platon_sha3(data.data(), data.size(), k.Values(), 32);
    return k;
  }
};
}  // namespace bn256
}  // namespace crypto
}  // namespace platon