#include "platon/crypto/bn256/pedersen.hpp"
#include "platon/common.h"
#include "unit_test.hpp"
using namespace platon;
using namespace platon::crypto::bn256;

TEST_CASE(pedersen, commit_g) {
  auto cm =Pedersen::CommitWithG(11, 22, 33);
  ASSERT(Pedersen::VerifyWithG(cm, 11, 22, 33));
}

TEST_CASE(pedersen, commit_h) {
  G1 H = G1::ScalarBaseMul("13503271286267599787575175649399662841097896087035805320611596397995555658534"_uint256);
  auto cm = Pedersen::CommitWithH(H, 22, 33);
//  ASSERT(Pedersen::VerifyWithH(cm, H, 22, 33));
}

UNITTEST_MAIN() {
//  RUN_TEST(pedersen, commit_g);
  RUN_TEST(pedersen, commit_h);
}