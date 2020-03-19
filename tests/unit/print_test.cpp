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

UNITTEST_MAIN() {
  RUN_TEST(print, char);
  RUN_TEST(print, int);
  // RUN_TEST(print, float);
  // RUN_TEST(print, double);
  RUN_TEST(print, string);
  RUN_TEST(print, u128);
}