#include "platon/common.h"
#include "platon/crypto/bls12381/bls12381.hpp"
#include "unit_test.hpp"


namespace platon {
namespace crypto {
namespace bls12381 {
namespace g16 {
namespace pairing {

/// Convenience method for a pairing check for two pairs.
bool PairingProd2(const G1 &a1, const G2 &a2, const G1 &b1, const G2 &b2) {
  std::array<G1, 2> g1{a1, b1};
  std::array<G2,2> g2{a2, b2};
  return bls12381::pairing(g1,g2) == 0;
}
/// Convenience method for a pairing check for three pairs.
bool PairingProd3(const G1 &a1, const G2 &a2, const G1 &b1, const G2 &b2,
                  const G1 &c1, const G2 &c2) {
  std::array<G1, 3> g1 {a1, b1, c1};
  std::array<G2, 3> g2 {a2, b2, c2};
  return bls12381::pairing(g1, g2) == 0;
}
/// Convenience method for a pairing check for four pairs.
bool PairingProd4(const G1 &a1, const G2 &a2, const G1 &b1, const G2 &b2,
                  const G1 &c1, const G2 &c2, const G1 &d1, const G2 &d2) {
  std::array<G1, 4> g1 {a1, b1, c1, d1};
  std::array<G2,4> g2 {a2, b2, c2, d2};
  return bls12381::pairing(g1, g2) == 0;
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
        G1{std::uint384_t("0x169d7633e3da4d413bf1918c412fc54c548ddf641a423f47b61ca883c0ba1b85f5ee13dd63d7c1661cc4fe2ca38f00e1"), std::uint384_t("0x065dbcfb2123f8258ae2b3cf92035485f621e55d433b1f251ad37c02ae2b3ec6a1658ae23bbc77649878ec0871a6d8f1")},
        G2(std::uint384_t("0x16c58ffdfec9c8b7a4d3826e32a40f99e97bd237067971e474438078e8bca6ccbbee0870bef905972fe879030273dd67"), std::uint384_t("0x1116f52efd0f128a0bd6be9042bec761332408e765609caae2b6f7805ab3287143a9bafeb94a7cdd6635fd1ee293c2cf"), std::uint384_t("0x037bef7f92e32e639cd632611077b121db404705da6a507b9b8d8adc08a38eba27b2d31b7b22e95a96e22e26660162d3"), std::uint384_t("0x0dd3791f5adb64150c2e7082f23a214b7ef5aa97fd903e648637deb13c156061788756f74b75545c3453e10822012e6a")),
        G2(std::uint384_t("0x00e3114ca5fa1c0af741154f059c94a4d2647481ba6071b97317587a937e21f048e5f85191ba2fbd0f3929121485f4e2"), std::uint384_t("0x1889d7bcf405c1932c7cc66ea8d353b34844769b50da863b3f0f11079371d2e6b7c10031e270761f136e2f4e52f16224"), std::uint384_t("0x1182c943f4672b1293f79699f6747d874e805f57e8211aae940d3e31693619054e2be22cb99b177cfc092db80c1f7896"), std::uint384_t("0x16872517c4811f11bba424dc64fdb3e6f27455e736d369d04de53a3e51eb36d4be6686a07f8be5352c138aceea7c6357")),
        G2(std::uint384_t("0x1394d62459f5e72373706504085111cae6afb88121e8f5707a9ff82daf97d300b9a20fc52a6ac7149dac1626d1e4d0ae"), std::uint384_t("0x095f3f088b18f2ab4e44df666560e101f4c83da739ce074527deb3fde2fcb25e2a19fe2a0f7c4f546ca7f904575875b9"), std::uint384_t("0x16ec4958b2b639d08f0c1bbd4af8e9e9289280e84b7308d790da4707ceefe61c78b661fbd76f8be9bd365ddd9a5f0b25"), std::uint384_t("0x0d108a1585684a6ccbc94bdcb37c133579866e5dabb75c77178927d38f59a687015995d7c6d7fb536ee9f76f7e8081b2")),
        {G1(std::uint384_t("0x07f1710668da99b2b45e6392d5cd89db7a1d34eff180ead6c886d51bca063f8111a179640d5d2dfc209e0a92783fea07"), std::uint384_t("0x0b5a9436d5c1be84f88a4127c9a02d307c5858df52ce76ae0b6fbb5c0d855438419e06a1c37f633b306e9da44eaf345e")),
         G1(std::uint384_t("0x18c1bf32977afa48e224b2209e9df000ea072af1cf06efc9e83cf9b156ff577e154da015b077626e2bab7a8300a087ea"), std::uint384_t("0x18dbc6bb77f98da0fc7e492c0624fd30b513ef5cfb54f2ed052216485052ba8a117f83a63e925b735fb6fe7c4c88e364")),
         G1(std::uint384_t("0x102bae3361383e2cef7d4aed348743201f6fd5fd25f4400d0888451a5556cd980f311510e6457e36fe8f330947269539"), std::uint384_t("0x0cf8968b8d779e53c04632588e402ae5884cc49ee3988159ede6a292fb3b9cd1071eec4bf3fffe51b7325184cadeab1d"))}
    };
  }

  int Verify(const std::vector<std::uint384_t> &inputs,
             const Proof &proof) {
    std::uint384_t snark_scalar_field =
        "21888242871839275222246405745257275088548364400416034343698204186575808495617"_uint256;
    VerifyingKey vk = GetVerifyingKey();
    platon_assert(inputs.size() + 1 == vk.gamma_abc.size());

    // Compute the linear combination vk_x
    G1 vk_x = G1{0, 0};
    for (int i = 0; i < inputs.size(); i++) {
      platon_assert(inputs[i] < snark_scalar_field);
      vk_x = Addition(
          vk_x, ScalarMul(vk.gamma_abc[i + 1], inputs[i]));
    }
    vk_x = Addition(vk_x, vk.gamma_abc[0]);

    if (!pairing::PairingProd4(proof.a, proof.b, Neg(vk_x),
                               vk.gamma, Neg(proof.c), vk.delta,
                               Neg(vk.alpha), vk.beta))
      return -1;
    return 0;
  }

  bool VerifyTx(const std::array<std::uint384_t,2> &a, const std::array<std::array<std::uint384_t,2>,2> &b,
                const std::array<std::uint384_t,2> &c, const std::vector<std::uint384_t> &inputs) {
    Proof proof{G1{a[0], a[1]}, G2(b[0][1], b[0][0], b[1][1], b[1][0]),
                G1{c[0], c[1]}};

    return Verify(inputs, proof) == 0;
  }
};
}  // namespace g16
}  // namespace bls1238
}  // namespace crypto
}  // namespace platon
using namespace platon;
using namespace platon::crypto::bls12381::g16;
TEST_CASE(zk, verify) {
  std::array<std::uint384_t, 2> a{
      std::uint384_t(
          "0x0be7a1f93063b64f1c4fe808f270fd6c8308308ece32e008210f74eb86528763134fc88a712c5451fe8ceef98284206f"),
      std::uint384_t("0x1940743c0c34528170e0f5c1c65cd6d60a0ea8697d95ebd8dc0a878e9443bc32f3388c47f596cfc6574b2c3a4c362b1b")};
  std::array<std::array<std::uint384_t, 2>, 2> b{
      {{std::uint384_t("0x0cca39682acbf7fa4ec06de7c8cf0054bdd7905ac84d4828152f8aa041162d97c6437f414577b5cdd2c11fbfc4aa0989"),
        std::uint384_t("0x0308a50cce70539b2770a3aa721134800236f8f7b26d76dccf8c367f29b6f5c423555b1b98441a7e795d696e6d23e4c2")},
       {std::uint384_t("0x0e6bbc8cf1f6a0516cd52c322a336a28de8b86a1d15ed3c8d727245991b145d2cb3ac69043a8619832319e533c49455a"),
        std::uint384_t("0x09c9678e63994ce7846898f592c1f51c8f164e5f1f299050dc0f6e656692f1e3d10e63ce24ab46045e4c9b7727d12df2")}}};
  std::array<std::uint384_t, 2> c{
      std::uint384_t(
          "0x09323a4fd658ab2e113dd6a2b79a8afb68ee2172a19da27c6d5fd2b165fbc5c6cb22a3e109095105917775ba152c3b96"),
      std::uint384_t("0x138b65ff88e08a00e7cbe3180a132b5248d2593ace21babd1c063ddf0634d637c087b786b8cfce5d7bb6d30f1147cdeb")};
  std::vector<std::uint384_t> inputs{"113569"_uint256, "1"_uint256};
  Verifier verifier;
  ASSERT(verifier.VerifyTx(a, b, c, inputs));
}

UNITTEST_MAIN() { RUN_TEST(zk, verify); }