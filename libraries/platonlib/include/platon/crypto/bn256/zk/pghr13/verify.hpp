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
        G2(std::uint256_t("0x2c713a96d371b0797cffef8e9d8c88d29f69b6ed2254640bff"
                          "cbb4af58048aec"),
           std::uint256_t("0x2cb4cf1bda7ad6d558dfc53a315ddeacc902607a038b3e334b"
                          "52fb386d9b2b80"),
           std::uint256_t("0x2c64f5e42ca611d695e7d57cc5947003bc88ee20d7d12db1c3"
                          "9336a165dc605f"),
           std::uint256_t("0x2cef319c86fa493f486f058e97278c00b0f72e616f207f459f"
                          "1b4c8f1fa6b771")),
        G1(std::uint256_t("0x22a0b024a8d3f69c0932c90ead26f0ac2f41ce94d28ee7dd6e"
                          "d8f5a419bacf80"),
           std::uint256_t("0x0e51ae372ac4e521763cdaeca7496a89e36e07d26ddc70a849"
                          "3f452b4b74562e")),
        G2(std::uint256_t("0x0b6982b062ccc6845e2451dd52acab7b799801ef8cc584c4f5"
                          "0bd74aaefd9c83"),
           std::uint256_t("0x1faf092da6f9ce9eac63ded33e88cbf8fb80bf8abee8fd446c"
                          "c2103cec5739dc"),
           std::uint256_t("0x2667fa98e5e89ef40a6bbbf5eef5256d81f5752fa3f65cc035"
                          "985cf2e0ecc251"),
           std::uint256_t("0x30552da756f6c5220cdc6c3abfd430b4e45d57b8e3408aab1c"
                          "60f5ec3520cd8a")),
        G2(std::uint256_t("0x0c1debca58ad14db49fe4c952568291d6712a2e5c64ee2f017"
                          "55249b4082515d"),
           std::uint256_t("0x06999da7e295d04aca401d3757863f99592269e847b86c3fb5"
                          "2d97feb17b6280"),
           std::uint256_t("0x0f7d469d18444dbdc2b35efd2d729e82fa65b8db21ab3da659"
                          "0339a170b05efc"),
           std::uint256_t("0x019324413bc826dfaf8696159bfe0ede669603ec34fcb49ced"
                          "cf7bc612a23c8e")),
        G1(std::uint256_t("0x223706c703a76ba581da804505a85f8bc696c2f93a64b20167"
                          "7595d16805b330"),
           std::uint256_t("0x092593779dd7f412ef4985582dd8482513c2d2d411a7267ae7"
                          "2e7ce2662494ae")),
        G2(std::uint256_t("0x09eb55afdb9005f2a4ca5551b94fa7cb936d8d2656d3f62ca5"
                          "cd24f33df3a7c7"),
           std::uint256_t("0x260dba5e503b1858c7e68e8b38e67686843765077f68c386d5"
                          "3cf7420649e4f1"),
           std::uint256_t("0x140b849be1e7ad18bb9e4691daa22d357786e9e86c3f663782"
                          "8436d1d5b007eb"),
           std::uint256_t("0x257b35527201caa15f300a59f4953e0abc5e72b4c14f83e42f"
                          "b9b1257c402f04")),
        G2(std::uint256_t("0x1a39448c0a9868054266e39ba17890395fc51b4da462a74e2a"
                          "9853fddddbd4eb"),
           std::uint256_t("0x17a2105e1c7f59d1f9fa9435b31920b82ef1e4fb53dcce18e7"
                          "c735ed625a9215"),
           std::uint256_t("0x118d861adac31c3f331e04a49661d8b6156249464930a98877"
                          "5701623752c6ed"),
           std::uint256_t("0x288b5ce78356fcc881ef1c38401e38f9dc970a7db9b2818910"
                          "a52a24bf3d2b03")),
        {G1(std::uint256_t("0x0df206ab25ff8f343456b791dcbdcac35d9d404d77dc9aca5"
                           "35fcfe68de50a0a"),
            std::uint256_t("0x0e866c697caa0aeecb24bc4f9452e9933295cff53f85470cd"
                           "13a30ee003a0f84")),
         G1(std::uint256_t("0x0e1e021153ad7f7d57a228d7434582ef0de3be7c030c8f1e4"
                           "8824063fd1c00ec"),
            std::uint256_t("0x07ee8ee83513ea8c54d657f225ea8727e67f150d1093f16db"
                           "336d1083959145b")),
         G1(std::uint256_t("0x252079f86bd059de801df45346f5a83a9c989c2ef7878d6c5"
                           "4202447f66450d4"),
            std::uint256_t("0x0b4cc25cf25d313d535a95e801fdb3b9cac7cf57130d2bb60"
                           "c565d4b4cd49359"))}};
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
      vk_x = Addition(vk_x, ScalarMul(vk.ic[i + 1], inputs[i]));
    }

    vk_x = Addition(vk_x, vk.ic[0]);
    if (!pairing::PairingProd2(proof.a, vk.a, Neg(proof.a_p), P2())) return -1;
    if (!pairing::PairingProd2(vk.b, proof.b, Neg(proof.b_p), P2())) return -2;
    if (!pairing::PairingProd2(proof.c, vk.c, Neg(proof.c_p), P2())) return -3;
    if (!pairing::PairingProd3(proof.k, vk.gamma,
                               Neg(Addition(vk_x, Addition(proof.a, proof.c))),
                               vk.gamma_beta_2, Neg(vk.gamma_beta_1), proof.b))
      return -4;
    if (!pairing::PairingProd3(Addition(vk_x, proof.a), proof.b, Neg(proof.h),
                               vk.z, Neg(proof.c), P2()))
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
    Proof proof{G1(a[0], a[1]),
                G1(a_p[0], a_p[1]),
                G2(b[0][1], b[0][0], b[1][1], b[1][0]),
                G1(b_p[0], b_p[1]),
                G1(c[0], c[1]),
                G1(c_p[0], c_p[1]),
                G1(h[0], h[1]),
                G1(k[0], k[1])};

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