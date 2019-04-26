//
// Created by zhou.yang on 2018/11/22.
//

#include "platon/RLP.h"
#include "platon/fixedhash.hpp"
#include "unittest.hpp"

using namespace platon;
TEST_CASE(rlp, int) {
  std::string data = "c3010203";
  RLPStream stream(3);
  stream << 1 << 2 << 3;
  std::string result = toHex(stream.out());
  ASSERT_EQ(result, data, result, data);
}

TEST_CASE(rlp, address) {
  std::string data = "9443355c787c50b647c425f594b441d4bd751951c1";
  RLPStream stream;
  stream << Address("0x43355c787c50b647c425f594b441d4bd751951c1", true);
  std::string result = toHex(stream.out());
  ASSERT_EQ(result, data, result, data);
}

TEST_CASE(rlp, array) {
  std::string data =
      "aa3078343333353563373837633530623634376334323566353934623434316434626437"
      "35313935316331";
  RLPStream stream;
  stream << "0x43355c787c50b647c425f594b441d4bd751951c1";
  std::string result = toHex(stream.out());
  ASSERT_EQ(result, data, result, data);
}

UNITTEST_MAIN() {
  RUN_TEST(rlp, int)
  RUN_TEST(rlp, address)
  RUN_TEST(rlp, array)
}
