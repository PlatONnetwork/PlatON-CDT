#undef NDEBUG
#include "platon/common.h"
#include "platon/fixedhash.hpp"
#include "platon/print.hpp"
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

TEST_CASE(debug, h64){
    h64 h64_data("0x0102030405060708");
    printf("h64_data:%s\t\n", h64_data.toString().c_str());
    auto address_info = make_address("lax10jc0t4ndqarj4q6ujl3g3ycmufgc77epxg02lt");
    printf("address:%s\t\n", address_info.first.toString().c_str());
}

UNITTEST_MAIN() {
  RUN_TEST(print, char);
  RUN_TEST(print, int);
  // RUN_TEST(print, float);
  // RUN_TEST(print, double);
  RUN_TEST(print, string);
  RUN_TEST(print, u128);
  RUN_TEST(debug, h64);
}