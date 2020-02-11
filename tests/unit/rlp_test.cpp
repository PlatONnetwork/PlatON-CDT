#include "unit_test.hpp"
#include "platon/RLP.h"
#include "platon/rlp_extend.hpp"
#include "platon/rlp_serialize.hpp"
#include "platon/fixedhash.hpp"
#include "platon/print.hpp"

using namespace platon;

TEST_CASE(rlp, int8_t) {
  int8_t int8_t_data = -2; 
  RLPStream stream;
  stream << int8_t_data;
  std::vector<byte> result = stream.out();
  for(auto i : result){
    platon::println(i);
  }
  int8_t_data = 0;
  fetch(RLP(result), int8_t_data);
  ASSERT_EQ(int8_t_data, -2, int8_t_data);
}

TEST_CASE(rlp, int16_t) {
  int16_t int16_t_data = -3; 
  RLPStream stream;
  stream << int16_t_data;
  std::vector<byte> result = stream.out();
  for(auto i : result){
    platon::println(i);
  }
  int16_t_data = 0;
  fetch(RLP(result), int16_t_data);
  ASSERT_EQ(int16_t_data, -3, int16_t_data);
}

TEST_CASE(rlp, int) {
  int int_data = -4; 
  RLPStream stream;
  stream << int_data;
  std::vector<byte> result = stream.out();
  for(auto i : result){
    platon::println(i);
  }
  int_data = 0;
  fetch(RLP(result), int_data);
  ASSERT_EQ(int_data, -4, int_data);
}

TEST_CASE(rlp, int32_t) {
  int32_t int32_t_data = -5; 
  RLPStream stream;
  stream << int32_t_data;
  std::vector<byte> result = stream.out();
  for(auto i : result){
    platon::println(i);
  }
  int32_t_data = 0;
  fetch(RLP(result), int32_t_data);
  ASSERT_EQ(int32_t_data, -5, int32_t_data);
}

TEST_CASE(rlp, int64_t) {
  int64_t int64_t_data = -6; 
  RLPStream stream;
  stream << int64_t_data;
  std::vector<byte> result = stream.out();
  for(auto i : result){
    platon::println(i);
  }
  int64_t_data = 0;
  fetch(RLP(result), int64_t_data);
  ASSERT_EQ(int64_t_data, -6, int64_t_data);
}

TEST_CASE(rlp, bool) {
  bool bool_data = true; 
  RLPStream stream;
  stream << bool_data;
  std::vector<byte> result = stream.out();
  for(auto i : result){
    platon::println(i);
  }
  bool_data = false;
  fetch(RLP(result), bool_data);
  ASSERT_EQ(bool_data, true, bool_data);
}

TEST_CASE(rlp, uint8_t) {
  uint8_t uint8_t_data = 1; 
  RLPStream stream;
  stream << uint8_t_data;
  std::vector<byte> result = stream.out();
  for(auto i : result){
    platon::println(i);
  }
  uint8_t_data = 0;
  fetch(RLP(result), uint8_t_data);
  ASSERT_EQ(uint8_t_data, 1, uint8_t_data);
}

TEST_CASE(rlp, uint16_t) {
  uint16_t uint16_t_data = 2; 
  RLPStream stream;
  stream << uint16_t_data;
  std::vector<byte> result = stream.out();
  for(auto i : result){
    platon::println(i);
  }
  uint16_t_data = 0;
  fetch(RLP(result), uint16_t_data);
  ASSERT_EQ(uint16_t_data, 2, uint16_t_data);
}

TEST_CASE(rlp, uint32_t) {
  uint32_t uint32_t_data = 3; 
  RLPStream stream;
  stream << uint32_t_data;
  std::vector<byte> result = stream.out();
  for(auto i : result){
    platon::println(i);
  }
  uint32_t_data = 0;
  fetch(RLP(result), uint32_t_data);
  ASSERT_EQ(uint32_t_data, 3, uint32_t_data);
}

TEST_CASE(rlp, uint64_t) {
  uint64_t uint64_t_data = 4; 
  RLPStream stream;
  stream << uint64_t_data;
  std::vector<byte> result = stream.out();
  for(auto i : result){
    platon::println(i);
  }
  uint64_t_data = 0;
  fetch(RLP(result), uint64_t_data);
  ASSERT_EQ(uint64_t_data, 4, uint64_t_data);
}

TEST_CASE(rlp, float) {
  float float_data = 1.23; 
  RLPStream stream;
  stream << float_data;
  std::vector<byte> result = stream.out();
  for(auto i : result){
    platon::println(i);
  }
  float_data = 0;
  fetch(RLP(result), float_data);
  platon::println(float_data);
}


TEST_CASE(rlp, double) {
  double double_data = 4.56; 
  RLPStream stream;
  stream << double_data;
  std::vector<byte> result = stream.out();
  for(auto i : result){
    platon::println(i);
  }
  double_data = 0;
  fetch(RLP(result), double_data);
  platon::println(double_data);
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
  RUN_TEST(rlp, int8_t)
  RUN_TEST(rlp, int16_t)
  RUN_TEST(rlp, int32_t)
  RUN_TEST(rlp, int)
  RUN_TEST(rlp, int64_t)
  RUN_TEST(rlp, bool)
  RUN_TEST(rlp, uint8_t)
  RUN_TEST(rlp, uint16_t)
  RUN_TEST(rlp, uint32_t)
  RUN_TEST(rlp, uint64_t)
  RUN_TEST(rlp, float)
  RUN_TEST(rlp, double)
  RUN_TEST(rlp, address)
  RUN_TEST(rlp, array)
}