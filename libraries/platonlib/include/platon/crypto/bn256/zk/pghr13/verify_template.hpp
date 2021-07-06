#pragma once
#include "platon/crypto/bn256/bn256.hpp"

namespace platon {
namespace crypto {
namespace bn256 {
namespace pghr13 {
namespace pairing {

/// Convenience method for a pairing check for two pairs.
bool PairingProd2(const G1 &a1, const G2 &a2, const G1 &b1, const G2 &b2) {
  std::array<G1, 2> g1{a1, b1};
  std::array<G2, 2> g2{a2, b2};
  return bn256::pairing(g1, g2) == 0;
}
/// Convenience method for a pairing check for three pairs.
bool PairingProd3(const G1 &a1, const G2 &a2, const G1 &b1, const G2 &b2,
                  const G1 &c1, const G2 &c2) {
  std::array<G1, 3> g1{a1, b1, c1};
  std::array<G2, 3> g2{a2, b2, c2};
  return bn256::pairing(g1, g2) == 0;
}
/// Convenience method for a pairing check for four pairs.
bool PairingProd4(const G1 &a1, const G2 &a2, const G1 &b1, const G2 &b2,
                  const G1 &c1, const G2 &c2, const G1 &d1, const G2 &d2) {
  std::array<G1, 4> g1{a1, b1, c1, d1};
  std::array<G2, 4> g2{a2, b2, c2, d2};
  return bn256::pairing(g1, g2) == 0;
}

};  // namespace pairing

class Verifier {
 public:
  struct VerifyingKey {
    G2 a;
    G1 b;
    G2 c;
    G2 gamma;
    G1 gamma_beta_1;
    G2 gamma_beta_2;
    G2 z;
    std::vector<G1> ic;
  };

  struct Proof {
    G1 a;
    G1 a_p;
    G2 b;
    G1 b_p;
    G1 c;
    G1 c_p;
    G1 h;
    G1 k;
  };

  VerifyingKey GetVerifyingKey() {
    return VerifyingKey{
        G2(<%vk_a%>),
        G1(<%vk_b%>),
        G2(<%vk_c%>),
        G2(<%vk_g%>),
        G1(<%vk_gb1%>),
        G2(<%vk_gb2%>),
        G2(<%vk_z%>),
        {
            <%vk_ic_pts%>
        }
    };
  }

  int Verify(const std::vector<std::uint256_t> &inputs,
             const Proof &proof) {
    std::uint256_t snark_scalar_field =
        "21888242871839275222246405745257275088548364400416034343698204186575808495617"_uint256;
    VerifyingKey vk = GetVerifyingKey();
    platon_assert(inputs.size() + 1 == vk.ic.size());

    // Compute the linear combination vk_x
    G1 vk_x = G1(0, 0);
    for (size_t i = 0; i < inputs.size(); i++) {
      platon_assert(inputs[i] < snark_scalar_field);
      vk_x =
          Addition(vk_x, ScalarMul(vk.ic[i + 1], inputs[i]));
    }

    vk_x = Addition(vk_x, vk.ic[0]);
    if (!pairing::PairingProd2(proof.a, vk.a, Neg(proof.a_p),
                               P2()))
      return -1;
    if (!pairing::PairingProd2(vk.b, proof.b, Neg(proof.b_p),
                               P2()))
      return -2;
    if (!pairing::PairingProd2(proof.c, vk.c, Neg(proof.c_p),
                               P2()))
      return -3;
    if (!pairing::PairingProd3(proof.k, vk.gamma,
                               Neg(Addition(
                                   vk_x, Addition(proof.a, proof.c))),
                               vk.gamma_beta_2,
                               Neg(vk.gamma_beta_1), proof.b))
      return -4;
    if (!pairing::PairingProd3(Addition(vk_x, proof.a), proof.b,
                               Neg(proof.h), vk.z,
                               Neg(proof.c), P2()))
      return -5;
    return 0;
  }
  bool VerifyTx(const std::array<std::uint256_t, 2> &a,
                const std::array<std::uint256_t, 2> &a_p,
                const std::array<std::array<std::uint256_t, 2>, 2> &b,
                const std::array<std::uint256_t, 2> &b_p,
                const std::array<std::uint256_t, 2> &c,
                const std::array<std::uint256_t, 2> &c_p,
                const std::array<std::uint256_t, 2> &h,
                const std::array<std::uint256_t, 2> &k,
                const std::vector<std::uint256_t> &inputs) {
    Proof proof{
        G1(a[0], a[1]),     G1(a_p[0], a_p[1]), G2(b[0][1], b[0][0], b[1][1], b[1][0]),
        G1(b_p[0], b_p[1]), G1(c[0], c[1]),     G1(c_p[0], c_p[1]),
        G1(h[0], h[1]),     G1(k[0], k[1])};

    return Verify(inputs, proof) == 0;
  }
  bool VerifyTx(const Proof &proof, const std::vector<std::uint256_t> &inputs) {
    return Verify(inputs, proof) == 0;
  }
};

}  // namespace pghr13
}  // namespace bn256
}  // namespace crypto
}  // namespace platon