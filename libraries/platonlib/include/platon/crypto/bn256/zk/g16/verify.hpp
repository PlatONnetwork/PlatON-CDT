#pragma once
#include "platon/crypto/bn256/bn256.hpp"
namespace platon {
namespace crypto {
namespace bn256 {
namespace g16 {
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
    G1 alpha;
    G2 beta;
    G2 gamma;
    G2 delta;
    std::vector<G1> gamma_abc;
  };
  struct Proof {
    G1 a;
    G2 b;
    G1 c;
  };
  VerifyingKey GetVerifyingKey() {
    return VerifyingKey{
        G1{std::uint256_t("0x1936c240636390dc823e3a728e94b208eb53c6756d81da57ec"
                          "3425e05d43ac10"),
           std::uint256_t("0x2d70ff78e8216bf29d58923a686d9738278b8ce2fd822e197c"
                          "85b09286d15566")},
        G2(std::uint256_t("0x2b4daf047abe2e7f0b311118c1b963b63695dc0d769cea7884"
                          "9604434de055bf"),
           std::uint256_t("0x29c13ecb6f33dbc4b3b8a02e2e255511ce4c26a8a2f299efcc"
                          "94caf2de4fce00"),
           std::uint256_t("0x1da9020008df7f549751f8a251af3b2dc4a2ad3e0870de54ac"
                          "aedd9fc1b47e17"),
           std::uint256_t("0x25ea0d7e2b29de431b86a943db30dbf4d98f68df9ca8a9628d"
                          "14d1591e817d90")),
        G2(std::uint256_t("0x011016e22ae045444f50fb80f246ec486c7e02af09132cd38c"
                          "4fcf484983e4f2"),
           std::uint256_t("0x00e83c788c2878d1d5eba3ed49b0d81e4c0487dedc3e4d1c2b"
                          "aab5833785b62f"),
           std::uint256_t("0x05eb89e741ed5b5d611cebf92d1ed02cd6f3311089f0d400df"
                          "7d9ced5a48fd41"),
           std::uint256_t("0x132a90a3b0d369ccd66e2a5ba04a935e44d8ad5dca93a76bba"
                          "592a578130a911")),
        G2(std::uint256_t("0x065f6a3323a2abffd621fc263f348eb914904b68d5897729ae"
                          "34a6b9d33f0852"),
           std::uint256_t("0x0c3b60f59d3bd50328a04c0ff6d979199685d0526f89f6ac29"
                          "d6174ce24707a2"),
           std::uint256_t("0x26e7ebce2b44efef6b6315938e33f0a8ecc82dbad635c9efa6"
                          "81ed85bbb59982"),
           std::uint256_t("0x12e0f3721230a0f38f6c9913048d5230fd2615ef3ff7f6ee4b"
                          "20dfe0bdea1a86")),
        {G1(std::uint256_t("0x1ef8d5d70234aa3e3d8fc4e3f1ca01c703182580b58110679"
                           "8f05b35fd5082c0"),
            std::uint256_t("0x2e468046d4ae35138e2032224925d5389712e5ca5e68f4d9c"
                           "1e1858e7d65602d")),
         G1(std::uint256_t("0x0cefae0e3fa6aa25a4485ab7b21d32794d3431a4e4a5ca82e"
                           "a427b831534c2c9"),
            std::uint256_t("0x23e3d2035b70884e547638b111870f5957f58ad8068f7a214"
                           "70164ad361e1e88")),
         G1(std::uint256_t("0x030ffe78ec3de150e8688db619bde78e21e894754e6be5ed8"
                           "3742677628b24bc"),
            std::uint256_t("0x053392f88cfa9092dfbc0bd199d8159e56207779473c24fc6"
                           "01eff91bcd345ca"))}};
  }

  int Verify(const std::vector<std::uint256_t> &inputs,
                        const Proof &proof) {
    std::uint256_t snark_scalar_field =
        "21888242871839275222246405745257275088548364400416034343698204186575808495617"_uint256;
    VerifyingKey vk = GetVerifyingKey();
    platon_assert(inputs.size() + 1 == vk.gamma_abc.size());
    // Compute the linear combination vk_x
    G1 vk_x = G1{0, 0};
    for (int i = 0; i < inputs.size(); i++) {
      platon_assert(inputs[i] < snark_scalar_field);
      vk_x = Addition(vk_x, ScalarMul(vk.gamma_abc[i + 1], inputs[i]));
    }
    vk_x = Addition(vk_x, vk.gamma_abc[0]);

    if (!pairing::PairingProd4(proof.a, proof.b, Neg(vk_x), vk.gamma,
                               Neg(proof.c), vk.delta, Neg(vk.alpha), vk.beta))
      return -1;
    return 0;
  }

  bool VerifyTx(const std::array<std::uint256_t, 2> &a,
                const std::array<std::array<std::uint256_t, 2>, 2> &b,
                const std::array<std::uint256_t, 2> &c,
                const std::vector<std::uint256_t> &inputs) {
    Proof proof{G1{a[0], a[1]}, G2(b[0][1], b[0][0], b[1][1], b[1][0]),
                G1{c[0], c[1]}};

    return Verify(inputs, proof) == 0;
  }
};
}  // namespace g16
}  // namespace bn256
}  // namespace crypto
}  // namespace platon
