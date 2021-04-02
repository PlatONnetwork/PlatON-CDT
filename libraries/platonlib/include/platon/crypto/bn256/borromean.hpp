#pragma once

#include <tuple>
#include "platon/chain.hpp"
#include "platon/crypto/bn256/bn256.hpp"
#include "platon/fixedhash.hpp"
namespace platon {
namespace crypto {
namespace bn256 {
/**
 * @brief This implements Borromean Ring Signatures which is 
a generalization of AOS Ring Signatures scheme
 */
class Borromean {
 public:
  /**
   * @brief Borromean Ring Signatures
   * @param rings a list of public keys
   * @param secret private key
   * @param index The position of the public key held in the list
   * @param k a list of random value
   * @param message message hash
   * @param s a matrix of random value
   * @return seed value & sign success of failed
   */
  static std::pair<std::uint256_t, int> Sign(
      const std::vector<std::vector<G1>> &rings,
      const std::vector<std::uint256_t> &secret,
      const std::vector<size_t> &index, const std::vector<std::uint256_t> &k,
      const h256 &message, std::vector<std::vector<std::uint256_t>> &s) {
    if (secret.size() != rings.size() || index.size() != rings.size() ||
        s.size() != rings.size() || k.size() != rings.size()) {
      return {0, -1};
    }

    std::vector<std::vector<std::uint256_t>> e;
    e.resize(rings.size());
    for (size_t i = 0; i < rings.size(); i++) {
      e[i].resize(rings[i].size());
      for (size_t j = 0; j < rings[i].size(); j++) {
        e[i][j] = 0;
      }
    }
    std::vector<G1> R;
    R.resize(rings.size());

    for (size_t i = 0; i < rings.size(); i++) {
      R[i] = G1::ScalarBaseMul(k[i]);

      e[i][index[i]] = BorH(R[i], message);
      size_t pos = index[i] + 1;
      for (size_t j = pos; j < rings[i].size(); j++) {
        G1 sG = G1::ScalarBaseMul(s[i][j]);
        G1 eP = rings[i][j];
        eP.ScalarMul(e[i][j]);
        R[i] = sG.Add(eP.Neg());
        e[i][j] = BorH(R[i], message);
      }
    }

    std::uint256_t e0 = BorH(R, message);

    for (size_t i = 0; i < rings.size(); i++) {
      R[i] = G1::ScalarBaseMul(k[i]);
      e[i][index[i]] = BorH(R[i], message);
      size_t end = index[i];
      for (size_t j = 0; j < end; j++) {
        G1 sG = G1::ScalarBaseMul(s[i][j]);
        G1 eP = rings[i][j];
        G1 Rij = sG.Add(eP.Neg());
        e[i][j] = BorH(Rij, message);
      }
      s[i][end] = AddMod(MulMod(e[i][end], secret[i], bn256::Order), k[i],
                         bn256::Order);
    }
    return {e0, 0};
  }

  /**
   * @brief Verify Borromean Ring Signatures
   * @param rings a list of public keys
   * @param e0 seed value in signature
   * @param s a matrix of value in signature
   * @param message message hash
   * @return true success of false failed
   */
  static bool Verify(const std::vector<std::vector<G1>> &rings,
                     const std::uint256_t &e0,
                     const std::vector<std::vector<std::uint256_t>> &s,
                     const h256 &message) {
    std::vector<G1> R;
    R.resize(rings.size());
    for (size_t i = 0; i < rings.size(); i++) {
      std::uint256_t eij = e0;
      G1 Rij;
      for (size_t j = 0; j < rings[i].size(); j++) {
        G1 sG = G1::ScalarBaseMul(s[i][j]);
        G1 eP = rings[i][j];
        eP.ScalarMul(eij);
        Rij = sG.Add(eP.Neg());
        eij = BorH(R[i], message);
      }
      R[i] = Rij;
    }
    std::uint256_t e00 = BorH(R, message);
    return e00 == e0;
  }

 private:
  static std::uint256_t BorH(const G1 &g, const h256 &m) {
    std::array<uint8_t, 96> data;
    memcpy(data.data(), m.data(), 32);
    memcpy(data.data() + 32, g.X().Values(), 32);
    memcpy(data.data() + 64, g.Y().Values(), 32);
    std::uint256_t p;
    platon_sha3(data.data(), data.size(), p.Values(), 32);
    return p % bn256::Order;
  }

  static std::uint256_t BorH(const std::vector<G1> &g, const h256 &m) {
    std::vector<uint8_t> data;
    data.resize(g.size() * 2 + 1);
    memcpy(data.data(), m.data(), 32);

    for (size_t i = 0; i < g.size(); i++) {
      memcpy(data.data() + (i * 2 + 1) * 32, g[i].X().Values(), 32);
      memcpy(data.data() + (i * 2 + 2) * 32, g[i].Y().Values(), 32);
    }

    std::uint256_t p;
    platon_sha3(data.data(), data.size(), p.Values(), 32);
    return p % bn256::Order;
  }
};
}  // namespace bn256
}  // namespace crypto
}  // namespace platon
