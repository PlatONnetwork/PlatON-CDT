#include "platon/crypto/bn256/schnorr.hpp"
#include "platon/common.h"
#include "unit_test.hpp"
using namespace platon;
using namespace platon::crypto::bn256;

TEST_CASE(schnorr, sign) {
  G1 g(1,2);
  std::uint256_t sk = 3;
  G1 pk = g.ScalarMul(sk);
  std::uint256_t message = 456;
  auto sign = Schnorr::Sign(sk, message);
  ASSERT(Schnorr::Verify(std::get<0>(sign), std::get<1>(sign), pk, message));
  auto x = std::get<0>(sign).X();
  auto y = std::get<0>(sign).Y();
  auto s = std::get<1>(sign);
//  println("success:", success, std::to_string(x), std::to_string(y), std::to_string(s));
}

UNITTEST_MAIN() {
  RUN_TEST(schnorr, sign);
}