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
        G2(std::uint256_t("0x0ff7593697b40ff80959537b4dba6941c4577bc59e19276dd2"
                          "8d1b6c9ab18026"),
           std::uint256_t("0x25f512a71ae915c390777acb8a205c231b923c48c3db0a84bb"
                          "111d6fa395cc8f"),
           std::uint256_t("0x123c74e689ed7a8779eedf214534d1902568c8c2a66e2d99df"
                          "4ea66c29553e58"),
           std::uint256_t("0x28a028dda4c515a333433e7b48d38bde394822805855a74428"
                          "1efd7d584a3af8")),
        G1(std::uint256_t("0x1d03b7395245db68a61b6f3eb9ea04c99bb752c14891490ed0"
                          "d1df688b698b90"),
           std::uint256_t("0x2725392938acc7cdee99af746ec400fbd3177cd10c097a6013"
                          "e40a2cc778367a")),
        G2(std::uint256_t("0x2c7f28319ad19e5981c552f0c280c1aecc229313afa9c07135"
                          "6510a5bb0e6a4f"),
           std::uint256_t("0x262e4c51458cdca886be8487f20f3ebebf5efb5cfb34ae09b5"
                          "16c24db5b36097"),
           std::uint256_t("0x0d3b57cafeea9cf1c284c31ea589173b38600dd66aac61039a"
                          "5a514d8979e952"),
           std::uint256_t("0x2f51f11a130650039cc8c36d251c9bb6aacd1807b614145225"
                          "dd269589d57255")),
        G1(std::uint256_t("0x22dc67156521945f80c262fd09e330beaf8a4a53ec104929d8"
                          "c26b070c6409bd"),
           std::uint256_t("0x1e93606e2a8a1fd13358b27eab740e3dcdd6125ec45d91f748"
                          "e09908cc3f1dd6")),
        G2(std::uint256_t("0x2714d65fda0321f80bb4fec721810fe30366c23feb55390b4b"
                          "af34a447c56c72"),
           std::uint256_t("0x182d10abf2b2b0be0aa9f6978fd214d1b4b690f87e80a51ecc"
                          "cb9e036112786c"),
           std::uint256_t("0x01c29d109e882874bbc126392e1afe866d05ee32320057364e"
                          "010492cf0e7169"),
           std::uint256_t("0x26d2c1640805d7ba1ce5c2ccbb932c9e967d344f0d7575c1a8"
                          "d58b555070ea59")),
        {G1(std::uint256_t("0x094607d63745b4d3246cf61c105b40a1c798cfe9cb61a8238"
                           "a1e6f90cd17200a"),
            std::uint256_t("0x1f207025b5e40e27957655a4e29dcc3373defb3aed0b74ded"
                           "224e0651be1975c")),
         G1(std::uint256_t("0x286599737a47d791dca267ddf1a07d9491e6403252b726329"
                           "1ac01a3bf08caef"),
            std::uint256_t("0x160ed8b4599777bafb32bcbc86ed5e479bfa5ac173dbae632"
                           "9de84517490bbb5")),
         G1(std::uint256_t("0x0110fe5476a7ed3ea95da7d4948f3b8c5289b2106a211c0fc"
                           "f8f991b261c8db8"),
            std::uint256_t("0x01b7668c697b470c3411ea37f93b2fc083c86b0fb4c5954ff"
                           "698530da13f4a28"))}};
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
      vk_x = Addition(vk_x, ScalarMul(vk.query[i + 1], inputs[i]));
    }
    vk_x = Addition(vk_x, vk.query[0]);
    /**
     * e(A*G^{alpha}, B*H^{beta}) = e(G^{alpha}, H^{beta}) * e(G^{psi},
     * H^{gamma})
     *                              * e(C, H)
     * where psi = \sum_{i=0}^l input_i pvk.query[i]
     */

    if (!pairing::PairingProd4(vk.g_alpha, vk.h_beta, vk_x, vk.h_gamma, proof.c,
                               vk.h, Neg(Addition(proof.a, vk.g_alpha)),
                               Addition(proof.b, vk.h_beta))) {
      return -1;
    }

    /**
     * e(A, H^{gamma}) = e(G^{gamma}, b)
     */
    if (!pairing::PairingProd2(proof.a, vk.h_gamma, Neg(vk.g_gamma), proof.b))
      return -2;
    return 0;
  }

  bool VerifyTx(const std::array<std::uint256_t, 2> &a,
                const std::array<std::array<std::uint256_t, 2>, 2> &b,
                const std::array<std::uint256_t, 2> &c,
                const std::vector<std::uint256_t> &inputs) {
    Proof proof{G1(a[0], a[1]), G2(b[0][1], b[0][0], b[1][1], b[1][0]),
                G1(c[0], c[1])};
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
