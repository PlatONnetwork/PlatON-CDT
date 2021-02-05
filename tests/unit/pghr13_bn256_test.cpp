#include "platon/crypto/bn256/zk/pghr13/verify.hpp"

#include "platon/common.h"
#include "unit_test.hpp"
using namespace platon;
using namespace platon::crypto::bn256::pghr13;

TEST_CASE(zk, verify) {
  std::array<std::uint256_t, 2> a = {
      std::uint256_t(
          "0x12f964c4823d1731d069ec678ebfb3b460520a4533148b1dfb9b43bf41257d5b"),
      std::uint256_t("0x2d3c3621588a17455681e47ee0456bda535b8bee0e973005ae479e0"
                     "e15b7dda5")};
  std::array<std::uint256_t, 2> a_p = {
      std::uint256_t(
          "0x282957c6f34995b5ad4242ae2e2eb8285aa408cf0b69ae8804df7dddba507c41"),
      std::uint256_t("0x1fb87b4d0f85dbdba1abfff0f5ac50f44e80f4bc6c435319547cac5"
                     "17c8d64d3")};
  std::array<std::array<std::uint256_t, 2>, 2> b = {
      {{std::uint256_t("0x08a433ad868645ec09780f3b114815dd0182650a9c5c41a577755"
                       "9729a37cb53"),
        std::uint256_t("0x00fbf45442b974e6f246e5a25a943448b2dabf952f6c5ad0b00b1"
                       "843d8408ff3")},
       {std::uint256_t("0x23060ec6136a6269872088c9d4ecd8ae7b004acd7c77855d4ec9e"
                       "2ef88dd4697"),
        std::uint256_t("0x1c325afc4e510631f5b5134110332a4db504516227031803dd390"
                       "f95bcc88430")}}};
  std::array<std::uint256_t, 2> b_p = {
      std::uint256_t(
          "0x198c7e22a463ffad86cf894b7bce68ce5aff222cb855dc0b67f5531c256f142a"),
      std::uint256_t("0x13dd14e359df99c33b1daccaec448617f456fb0309c03e1a1435ed9"
                     "10f4141be")};
  std::array<std::uint256_t, 2> c = {
      std::uint256_t(
          "0x1068dcc4062c80899f3c840fb2e903aec253537845373e0ac876bcc3896587b9"),
      std::uint256_t("0x08e23cd9050ac1ab5035c870a3295d53b383dcab3f3c4810da066e8"
                     "5c84ce619")};
  std::array<std::uint256_t, 2> c_p = {
      std::uint256_t(
          "0x05fda8675aecf4c453e77590d1c0aeaaea950d86ebb9016efe8941f5f87408e5"),
      std::uint256_t("0x1fcc521754afbd2840b1f29635e4a1ca48c8087b3525c48feeffe5b"
                     "4f07eec08")};
  std::array<std::uint256_t, 2> h = {
      std::uint256_t(
          "0x015981a3eb64fcb4f61ef446db3bd07fde27685c11c4fbf3d9dd0edeb0e05074"),
      std::uint256_t("0x02012e02f14e865cabc06b486e83877c02fbc0932d613630cf733b7"
                     "2644052c5")};
  std::array<std::uint256_t, 2> k = {
      std::uint256_t(
          "0x1249cb029998f2173b19278c953bbf2ec12b013419de823b734251c4f73c86f3"),
      std::uint256_t("0x0739d1c6b0146b5678ef8b7d777fa91a9cb8b2cc8754250c61286df"
                     "56e041242")};
  std::vector<std::uint256_t> inputs{
      std::uint256_t(
          "0x000000000000000000000000000000000000000000000000000000000001bba1"),
      std::uint256_t("0x0000000000000000000000000000000000000000000000000000000"
                     "000000001")};
  Verifier verifier;
  ASSERT(verifier.VerifyTx(a, a_p, b, b_p, c, c_p, h, k, inputs));
}

UNITTEST_MAIN() { RUN_TEST(zk, verify); }