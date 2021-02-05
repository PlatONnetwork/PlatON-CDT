#include "platon/crypto/bn256/zk/gm17/verify.hpp"

#include "platon/common.h"
#include "unit_test.hpp"
using namespace platon;
using namespace platon::crypto::bn256::gm17;

TEST_CASE(zk, verify) {
  std::array<std::uint256_t, 2> a = {
      std::uint256_t(
          "0x1fe42b856e173acb2d4575a7302595932eaae96f7b1d9f84a683e7c14a3c8da2"),
      std::uint256_t("0x2c8233afe64194dd1ff3d7f9c2722de2aed8076ac992a6f3afede82"
                     "7b5678189")};
  std::array<std::array<std::uint256_t, 2>, 2> b = {
      {{std::uint256_t("0x17e427383ae384ace58bbce67a0e9015b93a8d1b196c73875cb40"
                       "79a0c54c8b0"),
        std::uint256_t("0x252cee73cd69eda1d8933f6d3db537f2dc28977549b44e77efd1f"
                       "b53e393d6f2")},
       {
           std::uint256_t("0x2ef7d723db20c4ec2a13f643a9fad65571379a1101b2707a20"
                          "7fc206d6ecda0a"),
           std::uint256_t("0x2f550819c468c9bcc7f966704c0893371d7c6795b7e0ea447f"
                          "fb8fc8c1eeb433"),
       }}};
  std::array<std::uint256_t, 2> c = {
      std::uint256_t(
          "0x115755ecbdc2aef3cd7e1b503fbf9ec303bbc68570838a6210a36146adb7c39c"),
      std::uint256_t("0x17dc520cce43367d6256bd3d9c49ac4697dccba08659a8a506335a4"
                     "e2ed340b1")};
  std::vector<std::uint256_t> input = {
      std::uint256_t(
          "0x000000000000000000000000000000000000000000000000000000000001bba1"),
      std::uint256_t("0x0000000000000000000000000000000000000000000000000000000"
                     "000000001")};
  Verifier verifier;
  ASSERT(verifier.VerifyTx(a, b, c, input));
}
UNITTEST_MAIN() { RUN_TEST(zk, verify); }