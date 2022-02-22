#undef NDEBUG
#include "platon/print.hpp"
#include "platon/common.h"
#include "platon/fixedhash.hpp"
#include "unit_test.hpp"


TEST_CASE(print, char) {
  char test_char = 'a';
  platon::println(test_char);
  DEBUG(test_char)
}

TEST_CASE(print, int) {
  int test_int = -123;
  platon::println(test_int);
  DEBUG(test_int)
}

// TEST_CASE(print, float) {
//   float test_float = 1.23;
//   platon::println(test_float);
//   DEBUG(test_float)
// }

// TEST_CASE(print, double) {
//   double test_double = -4.56;
//   platon::println(test_double);
//   DEBUG(test_double)
// }

TEST_CASE(print, string) {
  std::string test_string = "string test";
  platon::println(test_string);
  DEBUG(test_string)
}

TEST_CASE(print, u128) {
  u128 i = 9999999;
  platon::println(i);

  u128 info = "1339673436730796483172139894692642816"_u128;
  platon::println(info);

  info = "0x01020300000000000000000000000000"_u128;
  platon::println(info);

  info = "66051"_u128;
  platon::println(info);

  info = "0x010203"_u128;
  platon::println(info);
}

TEST_CASE(print, int128_t) {
  u128 i = 9999999;
  platon::println(i);

  u128 info = "1339673436730796483172139894692642816"_u128;
  int128_t signed_info = 0 - int128_t(info);
  platon::println(int128_t(info));
  platon::println(signed_info);
}

TEST_CASE(print, address) {
  h64 h64_data("0x0102030405060708");
  platon::println("h64_data:", h64_data.toString());

#ifdef TESTNET
  auto address_info =
      make_address("atx10jc0t4ndqarj4q6ujl3g3ycmufgc77ep6ahhap");
  platon::println("check result:", address_info.second ? "true" : "false");
  platon::println("address:", address_info.first.toString());

  std::string eth_str_address = address_info.first.toEthAddress();
  platon::println("address:", address_info.first.toEthAddress());

  auto address_same_info =
      make_address("atx10jc0t4ndqarj4q6ujl3g3ycmufgc77ep6ahhap");
  platon::println("check same:", address_info.first == address_same_info.first
                                     ? "true"
                                     : "false");
  platon::println(
      "check not same:",
      address_info.first != address_same_info.first ? "true" : "false");

  auto address_same_info_eth = make_address(eth_str_address);
  platon::println("check same:", address_info.first == address_same_info_eth.first
                                     ? "true"
                                     : "false");
  platon::println(
      "check not same:",
      address_info.first != address_same_info_eth.first ? "true" : "false");

  auto address_other_info =
      make_address("atx10jc0t4ndqarj4q6ujl3g3ycmufgec7epra7lt8");
  platon::println("check same:", address_info.first == address_other_info.first
                                     ? "true"
                                     : "false");
  platon::println(
      "check not same:",
      address_info.first != address_other_info.first ? "true" : "false");
#else
  auto address_info =
      make_address("atp10jc0t4ndqarj4q6ujl3g3ycmufgc77epsmtawt");
  platon::println("check result:", address_info.second ? "true" : "false");
  platon::println("address:", address_info.first.toString());
  auto eip55_addr = "0x7cB0f5D66D07472A835C97e288931BE2518f7b21";
  std::string eth_str_address = address_info.first.toEthAddress();
  platon::println("address:", address_info.first.toEthAddress());
  ASSERT_EQ(address_info.second, true);
  ASSERT_EQ(address_info.first.toEthAddress(), eip55_addr);

  auto address_same_info =
      make_address("atp10jc0t4ndqarj4q6ujl3g3ycmufgc77epsmtawt");
  platon::println("check same:", address_info.first == address_same_info.first
                                     ? "true"
                                     : "false");
  
  ASSERT_EQ(address_info.first, address_same_info.first);
  platon::println(
      "check not same:",
      address_info.first != address_same_info.first ? "true" : "false");

  auto address_same_info_eth = make_address(eth_str_address);
  platon::println("check same:", address_info.first == address_same_info_eth.first
                                     ? "true"
                                     : "false");
  
  ASSERT_EQ(address_info.first, address_same_info_eth.first);
  platon::println(
      "check not same:",
      address_info.first != address_same_info_eth.first ? "true" : "false");

  auto address_other_info =
      make_address("atp10jc0t4ndqarj4q6ujl3g3ycmufgec7epfmz4cd");
  platon::println("check same:", address_info.first == address_other_info.first
                                     ? "true"
                                     : "false");
  ASSERT_NE(address_info.first, address_other_info.first);
  platon::println(
      "check not same:",
      address_info.first != address_other_info.first ? "true" : "false");
#endif
}

TEST_CASE(print, fixedhash) {
  bytes zero_bytes;
  for (int i = 0; i < 32; i++) zero_bytes.push_back(0);
  h256 data_256;
  ASSERT_EQ(data_256, h256(zero_bytes));
  h128 data_128;
  ASSERT_EQ(data_128, h128(zero_bytes));
  h64 data_64;
  ASSERT_EQ(data_64, h64(zero_bytes));
  Address data_160;
  ASSERT_EQ(data_160, Address(zero_bytes));
}

UNITTEST_MAIN() {
  RUN_TEST(print, char);
  RUN_TEST(print, int);
  // RUN_TEST(print, float);
  // RUN_TEST(print, double);
  RUN_TEST(print, string);
  RUN_TEST(print, u128);
  RUN_TEST(print, int128_t);
  RUN_TEST(print, address);
}