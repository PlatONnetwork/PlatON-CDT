#define TESTNET
#include "platon/fixedhash.hpp"
#include "platon/print.hpp"
#include "platon/bech32.hpp"
#include "unit_test.hpp"

TEST_CASE(constexpr, u128) {
  std::array<int, 3> array_test1{1, 2, 3};

  std::array<int, fromHexChar('3')> array_1{1, 2, 3};
  ASSERT_EQ(array_1, array_test1);

  std::array<int, 10> array_test2{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

  std::array<int, "10"_u128> array_2{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ASSERT_EQ(array_2, array_test2);

  std::array<int, "0a"_u128> array_3{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ASSERT_EQ(array_3, array_test2);

  std::array<int, "0x0a"_u128> array_4{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ASSERT_EQ(array_4, array_test2);
}

TEST_CASE(constexpr, Address) {
  auto arrr_1 = make_address("lax10jc0t4ndqarj4q6ujl3g3ycmufgc77epxg02lt");
  std::string info = "0x7cB0f5D66D07472A835C97e288931BE2518f7b21";
  ASSERT_EQ(Address(info), arrr_1.first)
  ASSERT_EQ(get_string_address(arrr_1.first), "lax10jc0t4ndqarj4q6ujl3g3ycmufgc77epxg02lt")

  char test_add[] = "lax10jc0t4ndqarj4q6ujl3g3ycmufgc77epxg02lt";
  arrr_1 = make_address(test_add);
  ASSERT_EQ(Address(info), arrr_1.first)

  std::string str_address = "lax10jc0t4ndqarj4q6ujl3g3ycmufgc77epxg02lt";
  arrr_1 = make_address(str_address);
  ASSERT_EQ(Address(info), arrr_1.first)
}

UNITTEST_MAIN() {
  RUN_TEST(constexpr, Address);
  RUN_TEST(constexpr, u128);
}