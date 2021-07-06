//
// Created by yangzhou on 4/7/21.
//

#include "platon/fixedhash.hpp"
#include "platon/solidity/decode.hpp"
#include "platon/solidity/encode.hpp"
#include "platon/solidity/encode_size.hpp"
#include "platon/solidity/sol_serialize.hpp"
#include "platon/solidity/sol_cross_call.hpp"
#include "unit_test.hpp"

using namespace platon::solidity::abi;

SolEncode sol;
SolEncode sol_copy;
SolEncodeSize sol_size;

template <typename T>
int CheckSerialize(const T &t) {
  sol_size.Reset();
  sol_size.Encode(t);
  size_t size = sol_size.Size();
  sol.Reset(size);
  sol.Encode(t);
  if (sol.Out().size() != size) {
    PrettyPrint(sol.Out().out());
    return -1;
  }
  T temp;
  Decode(temp, sol.Out().out());
  sol_copy.Reset(size);
  sol_copy.Encode(temp);
  if (sol_copy.Out().size() != size) {
    return -2;
  }
  for (size_t i = 0; i < size; i++) {
    if (sol.Out()[i] != sol_copy.Out()[i]) {
      return -3;
    }
  }
  return 0;
}

#define CHECK_SERIALIZE(t, e) \
  int r = CheckSerialize(t);  \
  ASSERT_EQ(r, e, "expect:", e, "actual:", r);

TEST_CASE(solidity, simple) {
  {
    std::uint256_t t(2);
    CHECK_SERIALIZE(t, 0);
  }
  {
    std::vector<std::uint256_t> t = {1, 2};
    CHECK_SERIALIZE(t, 0);
  }
  {
    std::int256_t t(1);
    CHECK_SERIALIZE(t, 0);
  }
  {
    std::array<uint8_t, 5> t = {1, 2, 3, 4, 5};
    int r = CheckSerialize(t);
    ASSERT_EQ(r, 0, r);
  }
  {
    auto t = fromHex("0x12345678911121314151617181920211");
    CHECK_SERIALIZE(t, 0);
  }
  {
    std::string t = "foobar";
    CHECK_SERIALIZE(t, 0);
  }
  {
    std::array<std::vector<uint8_t>, 2> t = {std::vector<uint8_t>{1},
                                             std::vector<uint8_t>{1}};
    CHECK_SERIALIZE(t, 0);
  }
  {
    std::vector<Address> t = {{1}, {2}};
    int r = CheckSerialize(t);
    ASSERT_EQ(r, 0, r);
  }
  {
    std::vector<std::array<uint8_t, 10>> t = {
        {0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0x10},
        {0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0x10}};
    CHECK_SERIALIZE(t, 0);
  }
  {
    std::vector<std::vector<uint8_t>> t = {fromHex("0x11"), fromHex("0x22")};
    CHECK_SERIALIZE(t, 0);
  }
  {
    std::vector<std::string> t = {"hello", "foobar"};
    CHECK_SERIALIZE(t, 0);
  }
  {
    std::array<std::string, 2> t = {"hello", "foobar"};
    CHECK_SERIALIZE(t, 0);
  }
  {
    std::vector<std::vector<std::array<uint8_t, 32>>> t = {
        std::vector<std::array<uint8_t, 32>>{std::array<uint8_t, 32>{1},
                                             std::array<uint8_t, 32>{2}},
        std::vector<std::array<uint8_t, 32>>{std::array<uint8_t, 32>{3},
                                             std::array<uint8_t, 32>{4},
                                             std::array<uint8_t, 32>{5}}};
    CHECK_SERIALIZE(t, 0);
  }
  {
    std::array<std::vector<std::array<uint8_t, 32>>, 2> t = {
        std::vector<std::array<uint8_t, 32>>{std::array<uint8_t, 32>{1},
                                             std::array<uint8_t, 32>{2}},
        std::vector<std::array<uint8_t, 32>>{std::array<uint8_t, 32>{3},
                                             std::array<uint8_t, 32>{4},
                                             std::array<uint8_t, 32>{5}}};
    CHECK_SERIALIZE(t, 0);
  }
  {
    std::array<std::array<std::array<uint8_t, 32>, 3>, 2> t = {
        std::array<std::array<uint8_t, 32>, 3>{std::array<uint8_t, 32>{1},
                                               std::array<uint8_t, 32>{2},
                                               std::array<uint8_t, 32>{3}},
        std::array<std::array<uint8_t, 32>, 3>{std::array<uint8_t, 32>{3},
                                               std::array<uint8_t, 32>{4},
                                               std::array<uint8_t, 32>{5}}};
    CHECK_SERIALIZE(t, 0);
  }
  {
    std::vector<std::array<std::vector<uint16_t>, 2>> t = {
        std::array<std::vector<uint16_t>, 2>{std::vector<uint16_t>{0, 1},
                                             std::vector<uint16_t>{2, 3}},
        std::array<std::vector<uint16_t>, 2>{std::vector<uint16_t>{4, 5},
                                             std::vector<uint16_t>{6, 7}}};
    CHECK_SERIALIZE(t, 0);
  }
}
TEST_CASE(solidity, tuple) {
  {
    std::tuple<std::vector<uint8_t>> t = {
        std::vector<uint8_t>{0xf0, 0xf0, 0xf0}};
    CHECK_SERIALIZE(t, 0);
  }
  {
    std::tuple<std::array<uint64_t, 2>> t = {std::array<uint64_t, 2>{1, 2}};
    CHECK_SERIALIZE(t, 0);
  }
  {
    std::tuple<std::vector<int32_t>> t = {std::vector<int32_t>{1, 2}};
    CHECK_SERIALIZE(t, 0);
  }
  {
    std::tuple<int32_t, int32_t> t = {1, 2};
    CHECK_SERIALIZE(t, 0);
  }
  {
    std::tuple<std::string, int32_t> t = {"Hello World", 2};
    CHECK_SERIALIZE(t, 0);
  }

  {
    std::tuple<std::array<int32_t, 2>, std::vector<int32_t>> t = {
        std::array<int32_t, 2>{1, 2}, std::vector<int32_t>{4, 5, 6}};
    CHECK_SERIALIZE(t, 0);
  }
  {
    std::tuple<std::vector<int32_t>, std::array<int32_t, 2>> t = {
        std::vector<int32_t>{4, 5, 6}, std::array<int32_t, 2>{1, 2}};
    CHECK_SERIALIZE(t, 0);
  }
  {
    std::tuple<std::vector<int32_t>, std::vector<int32_t>> t = {
        std::vector<int32_t>{1, 2, 3}, std::vector<int32_t>{4, 5, 6}};
    CHECK_SERIALIZE(t, 0);
  }

  {
    std::tuple<std::string, int64_t> t = {"hello world", 266};
    CHECK_SERIALIZE(t, 0);
  }

  {
    std::array<std::tuple<int32_t, int32_t>, 2> t = {
        std::tuple<int32_t, int32_t>{1, 2}, std::tuple<int32_t, int32_t>{3, 4}};
    CHECK_SERIALIZE(t, 0);
  }

  {
    std::array<std::tuple<std::vector<int32_t>>, 2> t = {
        std::tuple<std::vector<int32_t>>{std::vector<int32_t>{1, 2, 3}},
        std::tuple<std::vector<int32_t>>{std::vector<int32_t>{4, 5, 6}}};
    CHECK_SERIALIZE(t, 0);
  }

  {
    std::vector<std::tuple<int32_t, std::vector<int32_t>>> t = {
        std::tuple<int32_t, std::vector<int32_t>>{1,
                                                  std::vector<int32_t>{2, 3}},
        std::tuple<int32_t, std::vector<int32_t>>{4,
                                                  std::vector<int32_t>{5, 6}}};
    CHECK_SERIALIZE(t, 0);
  }

  {
    std::vector<std::tuple<int32_t, std::vector<int32_t>>> t = {
        std::tuple<int32_t, std::vector<int32_t>>{1,
                                                  std::vector<int32_t>{2, 3}},
        std::tuple<int32_t, std::vector<int32_t>>{4,
                                                  std::vector<int32_t>{5, 6}}};
    CHECK_SERIALIZE(t, 0);
  }

  {
    std::tuple<std::tuple<int32_t, std::vector<int32_t>>, std::vector<int32_t>,
               std::vector<int32_t>>
        t = {std::tuple<int32_t, std::vector<int32_t>>{
                 1, std::vector<int32_t>{2, 3}},
             std::vector<int32_t>{1, 2}, std::vector<int32_t>{8, 9}};
    CHECK_SERIALIZE(t, 0);
  }
  {
    std::tuple<std::array<int32_t, 2>,
               std::array<std::tuple<int32_t, int32_t>, 2>, int16_t,
               std::array<std::array<int64_t, 2>, 1>>
        t = {std::array<int32_t, 2>{1, 2},
             std::array<std::tuple<int32_t, int32_t>, 2>{
                 std::tuple<int32_t, int32_t>{10, 11},
                 std::tuple<int32_t, int32_t>{20, 21}},
             3,
             std::array<std::array<int64_t, 2>, 1>{
                 std::array<int64_t, 2>{4, 5}}};
    CHECK_SERIALIZE(t, 0);
  }
  {
    std::vector<std::array<std::tuple<uint16_t, uint16_t>, 1>> t = {
        std::array<std::tuple<uint16_t, uint16_t>, 1>{
            std::tuple<uint16_t, uint16_t>{1, 2}},
        std::array<std::tuple<uint16_t, uint16_t>, 1>{
            std::tuple<uint16_t, uint16_t>{3, 4}},
        std::array<std::tuple<uint16_t, uint16_t>, 1>{
            std::tuple<uint16_t, uint16_t>{5, 6}},
        std::array<std::tuple<uint16_t, uint16_t>, 1>{
            std::tuple<uint16_t, uint16_t>{7, 8}}};
    CHECK_SERIALIZE(t, 0);
  }
  {
    std::tuple<std::vector<std::vector<uint64_t>>,
               std::array<std::tuple<uint32_t, uint32_t>, 1>, uint64_t>
        t = {std::vector<std::vector<uint64_t>>{std::vector<uint64_t>{3, 4}},
             std::array<std::tuple<uint32_t, uint32_t>, 1>{
                 std::tuple<uint32_t, uint32_t>{1, 2}},
             10};
    CHECK_SERIALIZE(t, 0);
  }
}
struct T1 {
  std::vector<uint8_t> v1;
  SOLIDITY_SERIALIZE(T,(v1))
};
struct T2 {
  std::array<uint64_t, 2> v1;
  SOLIDITY_SERIALIZE(T,(v1))
};
struct T3 {
  std::vector<int32_t> v1;
  SOLIDITY_SERIALIZE(T,(v1))
};

struct T4 {
  int32_t v1;
  int32_t v2;
  SOLIDITY_SERIALIZE(T,(v1)(v2))
};

struct T5 {
  std::string v1;
  int32_t v2;
  SOLIDITY_SERIALIZE(T,(v1)(v2))
};

struct T6 {
  std::array<int32_t, 2> v1;
  std::vector<int32_t> v2;
  SOLIDITY_SERIALIZE(T,(v1)(v2))
};

struct T7 {
  std::vector<int32_t> v1;
  std::array<int32_t, 2> v2;
  SOLIDITY_SERIALIZE(T,(v1)(v2))
};

struct T8 {
  std::vector<int32_t> v1;
  std::vector<int32_t> v2;
  SOLIDITY_SERIALIZE(T,(v1)(v2))
};

struct T9 {
  std::string v1;
  int64_t v2;
  SOLIDITY_SERIALIZE(T,(v1)(v2))
};
struct T10_1 {
  int32_t v1;
  std::vector<int32_t> v2;
  SOLIDITY_SERIALIZE(T,(v1)(v2))
};
struct T10 {
  T10_1 v1;
  std::vector<int32_t> v2;
  std::vector<int32_t> v3;
  SOLIDITY_SERIALIZE(T,(v1)(v2)(v3))
};
struct T11_1 {
  int32_t v1;
  int32_t v2;
  SOLIDITY_SERIALIZE(T11_1, (v1)(v2))
};

struct T11 {
  std::array<int32_t, 2> v1;
  std::array<T11_1, 2> v2;
  int16_t v3;
  std::array<std::array<int64_t,2>,1> v4;
  SOLIDITY_SERIALIZE(T11,(v1)(v2)(v3)(v4))
};
TEST_CASE(solidity, tuple_struct) {
  {
    T1 t = {{0xf0,0xf0,0xf0}};
    CHECK_SERIALIZE(t, 0);
  }
  {
    T2 t = {{1,2}};
    CHECK_SERIALIZE(t, 0);
  }
  {
    T3 t = {{1,2}};
    CHECK_SERIALIZE(t, 0);
  }
  {
    T4 t = {1,2};
    CHECK_SERIALIZE(t, 0);
  }
  {
    T5 t = {"Hello World",5};
    CHECK_SERIALIZE(t, 0);
  }
  {
    T6 t = {{1,2},{4,5,6}};
    CHECK_SERIALIZE(t, 0);
  }
  {
    T7 t = {{4,5,6},{1,2}};
    CHECK_SERIALIZE(t, 0);
  }
  {
    T8 t = {{1,2,3},{4,5,6}};
    CHECK_SERIALIZE(t, 0);
  }
  {
    T9 t = {"hello world", 266};
    CHECK_SERIALIZE(t, 0);
  }
  {
    T10 t = {{1,{1,2}}, {1,2},{8,9}};
    CHECK_SERIALIZE(t, 0);
  }
  {
    T11 t = {{1,2}, {T11_1{10,11},T11_1{20,21}}, 3, {{4,5}}};
    CHECK_SERIALIZE(t, 0);
  }
}

UNITTEST_MAIN() {
  RUN_TEST(solidity, simple);
  RUN_TEST(solidity, tuple);
  RUN_TEST(solidity, tuple_struct);
}