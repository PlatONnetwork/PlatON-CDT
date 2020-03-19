#include "platon/fixedhash.hpp"
#include "platon/print.hpp"
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
  Address arrr_1 = make_address("0x43355c787c50b647c425f594b441d4bd751951c1");
  println(arrr_1.toString());
  std::string info = "0x43355c787c50b647c425f594b441d4bd751951c1";
  std::string fetch_data = arrr_1.toString();
  ASSERT_EQ(info, fetch_data);

  char test_add[] = "0x43355c787c50b647c425f594b441d4bd751951c1";
  arrr_1 = make_address(test_add);
  println(arrr_1.toString());
  fetch_data = arrr_1.toString();
  ASSERT_EQ(info, fetch_data);
}

UNITTEST_MAIN() {
  RUN_TEST(constexpr, Address);
  RUN_TEST(constexpr, u128);
}