#pragma once

#include "platon/crypto/bn256/bn256.hpp"

namespace platon {
namespace crypto {
namespace bn256 {

/**
 * @brief Pedersen Commitment
 */
class Pedersen {
 public:
  /**
   * @brief Generate Pedersen Commitment C = r * a * G + m * G
   * @param a Random point for generating another generator H
   * @param m Message to commit to
   * @param r Random Value
   * @return pedersen commitment
   */
  static G1 CommitWithG(const std::uint256_t &a,
                                                   const std::uint256_t &m,
                                                   const std::uint256_t &r) {
    platon::crypto::bn256::G1 H = G1::ScalarBaseMul(a);
    platon::crypto::bn256::G1 G = G1::Base();
    // H*r + G*m
    H.ScalarMul(r).Add(G.ScalarMul(m));

    return H;
  }

  /**
   * @brief Generate Pedersen Commitment C = r * H + m * G using provided generator H
   * @param h Supposedly another random generator
   * @param m Message to commit to
   * @param r Random value
   * @return pedersen commitment
   */
  static G1 CommitWithH(
      const G1 &h, const std::uint256_t &m,
      const std::uint256_t &r) {
    platon::crypto::bn256::G1 H = h;
    platon::crypto::bn256::G1 G = G1::Base();
    
    // H*r + G*m
    H.ScalarMul(r).Add(G.ScalarMul(m));
    
    return H;
  }

  /**
   * @brief Verify a pederson commitment by reconstructing commitment
   * @param commitment The commitment
   * @param a Message committed to
   * @param m Random value
   * @param r Random value to generator random generator H
   * @return true Success or false failure
   */
  static bool VerifyWithG(const G1 &commitment,
                          const std::uint256_t &a, const std::uint256_t &m,
                          const std::uint256_t &r) {
    platon::crypto::bn256::G1 H = G1::ScalarBaseMul(a);
    platon::crypto::bn256::G1 G = G1::Base();
    // H*r + G*m
    H.ScalarMul(r).Add(G.ScalarMul(m));
    return H == commitment;
  }

  /**
   * @brief Verify a pederson commitment by reconstructing commitment
   * @param commitment The commitment
   * @param h Random generator
   * @param m Message committed to
   * @param r Random value
   * @return true Success or false failure
   */
  static bool VerifyWithH(const G1 &commitment,
                          const G1 &H,
                          const std::uint256_t &m, const std::uint256_t &r) {
    platon::crypto::bn256::G1 G = G1::Base();
    G1 R = H;
    // H*r + G*m
    R.ScalarMul(r).Add(G.ScalarMul(m));
    return  R == commitment;
  }
};
}  // namespace bn256
}  // namespace crypto
}  // namespace platon