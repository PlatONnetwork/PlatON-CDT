#include "platon/bech32.hpp"
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
#ifdef TESTNET
  auto arrr_1 = make_address("atx10jc0t4ndqarj4q6ujl3g3ycmufgc77ep6ahhap");
  ASSERT_EQ(arrr_1.first.toString(),
            "atx10jc0t4ndqarj4q6ujl3g3ycmufgc77ep6ahhap")

  char test_add[] = "atx10jc0t4ndqarj4q6ujl3g3ycmufgc77ep6ahhap";
  auto arrr_2 = make_address(test_add);
  ASSERT_EQ(arrr_2.first, arrr_1.first)

  std::string str_address = "atx10jc0t4ndqarj4q6ujl3g3ycmufgc77ep6ahhap";
  arrr_2 = make_address(str_address);
  ASSERT_EQ(arrr_2.first, arrr_1.first)
#else
  auto arrr_1 = make_address("atp10jc0t4ndqarj4q6ujl3g3ycmufgc77epsmtawt");
  ASSERT_EQ(arrr_1.first.toString(),
            "atp10jc0t4ndqarj4q6ujl3g3ycmufgc77epsmtawt")
  char test_add[] = "atp10jc0t4ndqarj4q6ujl3g3ycmufgc77epsmtawt";
  auto arrr_2 = make_address(test_add);
  ASSERT_EQ(arrr_2.first, arrr_1.first)

  std::string str_address = "atp10jc0t4ndqarj4q6ujl3g3ycmufgc77epsmtawt";
  arrr_2 = make_address(str_address);
  ASSERT_EQ(arrr_2.first, arrr_1.first)
#endif
}

UNITTEST_MAIN() {
  RUN_TEST(constexpr, Address);
  RUN_TEST(constexpr, u128);
}