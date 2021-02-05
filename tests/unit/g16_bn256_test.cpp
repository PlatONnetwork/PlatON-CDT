#include "platon/common.h"
#include "platon/crypto/bn256/zk/g16/verify.hpp"
#include "unit_test.hpp"

using namespace platon;
using namespace platon::crypto::bn256::g16;

TEST_CASE(zk, verify) {
  std::array<std::uint256_t, 2> a{
      std::uint256_t(
          "0x2ea377caf7635203e02a1c3e0e48b73cc7bcdab40449275f884a248e2302d598"),
      std::uint256_t("0x1ac3d8199aebe63c36d867942bb8a62361973e51390c41d7c27283d"
                     "bd02076a9")};
  std::array<std::array<std::uint256_t, 2>, 2> b{
      {{std::uint256_t("0x27f7b46b36f968d5f4dab8b8b15f7fa63766325646403aa3a89ce"
                       "584dde0898f"),
        std::uint256_t("0x030dc19cb238db5cf3ef90c463e6d8d5ba06f452dc443e49fdfa2"
                       "f5e878e46fd")},
       {std::uint256_t("0x0757e81a7a3240939c9ebb724695f5696b9b89c5fd734028bfaf6"
                       "160de218fbd"),
        std::uint256_t("0x12e5a0601c9906932350d2c1fb34faa270591fe7d5b29a8b399f7"
                       "622c03cdc9f")}}};
  std::array<std::uint256_t, 2> c{
      std::uint256_t(
          "0x22557b4a1d67dcc661a1a689c62e1b0200cc847daac959df352d85b03c0049bf"),
      std::uint256_t("0x02f47002c238a84d04da2eddb26c62655682f2f141085ef8b3247b3"
                     "24851fc3c")};
  std::vector<std::uint256_t> inputs{"113569"_uint256, "1"_uint256};
  Verifier verifier;
  ASSERT(verifier.VerifyTx(a, b, c, inputs));
}

UNITTEST_MAIN() { RUN_TEST(zk, verify); }