#pragma once
#include "platon/crypto/bn256/bn256.hpp"
namespace platon {
namespace crypto {
namespace bn256 {
namespace gm17 {
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
    G2 h;
    G1 g_alpha;
    G2 h_beta;
    G1 g_gamma;
    G2 h_gamma;
    std::vector<G1> query;
  };

  struct Proof {
    G1 a;
    G2 b;
    G1 c;
  };

  VerifyingKey GetVerifyingKey() {
    return VerifyingKey{
        G2(<%vk_h%>),
        G1(<%vk_g_alpha%>),
        G2(<%vk_h_beta%>),
        G1(<%vk_g_gamma%>),
        G2(<%vk_h_gamma%>),
        {
            <%vk_query_pts%>
        }
    };
  }

  int Verify(const std::vector<std::uint256_t> &inputs,
             const Proof &proof) {
    std::uint256_t snark_scalar_field =
        "21888242871839275222246405745257275088548364400416034343698204186575808495617"_uint256;
    VerifyingKey vk = GetVerifyingKey();
    platon_assert(inputs.size() + 1 == vk.query.size());
    // Compute the linear combination vk_x
    G1 vk_x(0, 0);
    for (size_t i = 0; i < inputs.size(); i++) {
      platon_assert(inputs[i] < snark_scalar_field);
      vk_x = Addition(vk_x,
                      ScalarMul(vk.query[i + 1], inputs[i]));
    }
    vk_x = Addition(vk_x, vk.query[0]);
    /**
     * e(A*G^{alpha}, B*H^{beta}) = e(G^{alpha}, H^{beta}) * e(G^{psi},
     * H^{gamma})
     *                              * e(C, H)
     * where psi = \sum_{i=0}^l input_i pvk.query[i]
     */

    if (!pairing::PairingProd4(
        vk.g_alpha, vk.h_beta, vk_x, vk.h_gamma, proof.c, vk.h,
        Neg(Addition(proof.a, vk.g_alpha)),
        Addition(proof.b, vk.h_beta))){
      return -1;
    }

    /**
     * e(A, H^{gamma}) = e(G^{gamma}, b)
     */
    if (!pairing::PairingProd2(proof.a, vk.h_gamma, Neg(vk.g_gamma),
                               proof.b))
      return -2;
    return 0;
  }

  bool VerifyTx(const std::array<std::uint256_t, 2> &a,
                const std::array<std::array<std::uint256_t, 2>, 2> &b,
                const std::array<std::uint256_t, 2> &c,
                const std::vector<std::uint256_t> &inputs) {
    Proof proof{G1(a[0], a[1]), G2(b[0][1], b[0][0], b[1][1], b[1][0]), G1(c[0], c[1])};
    return Verify(inputs, proof) == 0;
  }

  bool VerifyTx(const Proof &proof, const std::vector<std::uint256_t> &inputs) {
    return Verify(inputs, proof) == 0;
  }
};
}  // namespace gm17
}  // namespace bn256
}  // namespace crypto
}  // namespace platon