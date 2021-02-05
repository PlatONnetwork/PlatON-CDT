#pragma once
#include "platon/chain.hpp"
#include "platon/crypto/bn256/bn256.hpp"
#include "platon/fixedhash.hpp"
namespace platon {
namespace crypto {
namespace bn256 {
/**
 * @brief This implements AOS 1-out-of-n ring signature which require only `n+1`
scalars to validate in addition to the `n` public keys
 */
class AosRing {
 public:
  /**
   * @brief create AOS ring signature
   * @param pks a list of public keys
   * @param sk private key
   * @param pos The position of the public key held in the list
   * @param tees a list of random value
   * @param alpha random value
   * @param message message hash
   * @return signature
   */
  static std::uint256_t Sign(const std::span<G1> &pks, std::uint256_t &sk,
                             size_t pos, std::vector<std::uint256_t> &tees,
                             const std::uint256_t &alpha,
                             const std::uint256_t &message) {
    std::vector<std::uint256_t> cees;
    cees.resize(pks.size());
    size_t n = pos;
    for (size_t i = 0; i < pks.size(); i++) {
      size_t idx = n % pks.size();
      cees[idx] = CalcProof(
          pks[idx], tees[idx],
          i == 0 ? alpha : cees[(idx + pks.size() - 1) % pks.size()], message);
      n++;
    }

    std::uint256_t gap =
        SubMod(alpha, cees[(pos + pks.size() - 1) % pks.size()], Order);
    tees[pos] = AddMod(tees[pos], MulMod(sk, gap, Order), Order);
    return cees[cees.size() - 1];
  }

  /**
   * @brief Verify AOS ring signature
   * @param pks a list of public keys
   * @param tees tees in signature
   * @param seed seed in signature
   * @param message 
   * @return 
   */
  static bool Verify(const std::span<G1> &pks,
                     const std::vector<std::uint256_t> &tees,
                     const std::uint256_t &seed, std::uint256_t &message) {
    std::uint256_t c = seed;
    for (size_t i = 0; i < pks.size(); i++) {
      c = CalcProof(pks[i], tees[i], c, message);
    }
    return c == seed;
  }

 private:
  static std::uint256_t CalcProof(const G1 &pk, const std::uint256_t &s,
                                  const std::uint256_t &e,
                                  const std::uint256_t &m) {
    G1 sg = G1::ScalarBaseMul(s);
    G1 xg = pk;
    G1 kg = sg.Add(xg.ScalarMul(e));
    return Hash(pk.X(), pk.Y(), kg.X(), kg.Y(), m);
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