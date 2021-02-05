
#undef NDEBUG
#include "platon/platon.hpp"
#include "unit_test.hpp"

using namespace platon;

TEST_CASE(uint256, compile) {
  constexpr std::uint256_t orign = 100;
  std::uint512_t other = orign;
  ASSERT_EQ(orign, std::uint256_t(100));
  ASSERT_EQ(orign, other);
}

TEST_CASE(uint256, assign) {
  const char *fn = "assign";
  std::uint256_t orign(30);
  std::uint256_t other;

  {
    uint64_t start = platon_gas();
    orign = other;
    uint64_t end = platon_gas();
    std::string result = std::string(fn) + " function, line: " + std::to_string(__LINE__);
    result += ", gas used: " + std::to_string(start - end);
    printf("%s\t\n", result.c_str());
  }

  ASSERT_EQ(orign, other);
}

TEST_CASE(uint256, add) {
  const char *fn = "add";
  std::uint256_t orign(202);
  std::uint256_t other(243);

  {
    uint64_t start = platon_gas();
    orign = orign + other;
    uint64_t end = platon_gas();
    std::string result = std::string(fn) + " function, line: " + std::to_string(__LINE__);
    result += ", gas used: " + std::to_string(start - end);
    printf("%s\t\n", result.c_str());
  }

  ASSERT_EQ(orign, std::uint256_t(445));
  orign += other;
  ASSERT_EQ(orign, std::uint256_t(688));

  orign = orign + 12;
  ASSERT_EQ(orign, std::uint256_t(700));
  orign += 100;
  ASSERT_EQ(orign, std::uint256_t(800));
}

TEST_CASE(uint256, sub) {
  const char *fn = "sub";
  std::uint256_t orign(202);
  std::uint256_t other(24);

  {
    uint64_t start = platon_gas();
    orign = orign - other;
    uint64_t end = platon_gas();
    std::string result = std::string(fn) + " function, line: " + std::to_string(__LINE__);
    result += ", gas used: " + std::to_string(start - end);
    printf("%s\t\n", result.c_str());
  }

  ASSERT_EQ(orign, std::uint256_t(178));
  orign -= other;
  ASSERT_EQ(orign, std::uint256_t(154));
  orign = orign - 4;
  ASSERT_EQ(orign, std::uint256_t(150));
  orign -= 50;
  ASSERT_EQ(orign, std::uint256_t(100));
}

TEST_CASE(uint256, multip) {
  const char *fn = "multip";
  std::uint256_t orign(202);
  std::uint256_t other(24);

  {
    uint64_t start = platon_gas();
    orign = orign * other;
    uint64_t end = platon_gas();
    std::string result = std::string(fn) + " function, line: " + std::to_string(__LINE__);
    result += ", gas used: " + std::to_string(start - end);
    printf("%s\t\n", result.c_str());
  }

  ASSERT_EQ(orign, std::uint256_t(4848));
  orign /= other;
  ASSERT_EQ(orign, std::uint256_t(202));
  orign = orign * 10;
  ASSERT_EQ(orign, std::uint256_t(2020));
  orign *= 10;
  ASSERT_EQ(orign, std::uint256_t(20200));
}

TEST_CASE(uint256, division) {
  const char *fn = "division";
  std::uint256_t orign(202);
  std::uint256_t other(24);

  {
    uint64_t start = platon_gas();
    orign = orign / other;
    uint64_t end = platon_gas();
    std::string result = std::string(fn) + " function, line: " + std::to_string(__LINE__);
    result += ", gas used: " + std::to_string(start - end);
    printf("%s\t\n", result.c_str());
  }

  ASSERT_EQ(orign, std::uint256_t(8));
  orign /= other;
  ASSERT_EQ(orign, std::uint256_t(0));
  orign = 100;
  orign = orign / 10;
  ASSERT_EQ(orign, std::uint256_t(10));
  orign /= 10;
  ASSERT_EQ(orign, std::uint256_t(1));
}

TEST_CASE(uint256, mod) {
  const char *fn = "mod";
  std::uint256_t orign(503);
  std::uint256_t other(10);

  {
    uint64_t start = platon_gas();
    orign = orign % other;
    uint64_t end = platon_gas();
    std::string result = std::string(fn) + " function, line: " + std::to_string(__LINE__);
    result += ", gas used: " + std::to_string(start - end);
    printf("%s\t\n", result.c_str());
  }

  ASSERT_EQ(orign, std::uint256_t(3));
  orign += std::uint256_t(2);
  orign %= other;
  ASSERT_EQ(orign, std::uint256_t(5));
  orign = orign % 3;
  ASSERT_EQ(orign, std::uint256_t(2));
  orign %= 3;
  ASSERT_EQ(orign, std::uint256_t(2));
}

TEST_CASE(uint256, and) {
  const char *fn = "and";
  std::uint256_t orign(503);
  std::uint256_t other(100);

  {
    uint64_t start = platon_gas();
    orign = orign & other;
    uint64_t end = platon_gas();
    std::string result = std::string(fn) + " function, line: " + std::to_string(__LINE__);
    result += ", gas used: " + std::to_string(start - end);
    printf("%s\t\n", result.c_str());
  }

  ASSERT_EQ(orign, std::uint256_t(100));
  orign = orign & std::uint256_t(0xff);
  ASSERT_EQ(orign, std::uint256_t(100));
  orign = orign & 0x0f;
  ASSERT_EQ(orign, std::uint256_t(4));
  orign &= 2;
  ASSERT_EQ(orign, std::uint256_t(0));
}

TEST_CASE(uint256, or) {
  const char *fn = "or";
  std::uint256_t orign(503);
  std::uint256_t other(100);

  {
    uint64_t start = platon_gas();
    orign = orign | other;
    uint64_t end = platon_gas();
    std::string result = std::string(fn) + " function, line: " + std::to_string(__LINE__);
    result += ", gas used: " + std::to_string(start - end);
    printf("%s\t\n", result.c_str());
  }

  ASSERT_EQ(orign, std::uint256_t(503));
  orign = orign | std::uint256_t(0xff);
  ASSERT_EQ(orign, std::uint256_t(511));
  orign = 100;
  orign = orign | 0xff;
  ASSERT_EQ(orign, std::uint256_t(0xff));
  orign |= 0x0fff;
  ASSERT_EQ(orign, std::uint256_t(0x0fff));
}

TEST_CASE(uint256, xor) {
  const char *fn = "xor";
  std::uint256_t orign(503);
  std::uint256_t other(100);

  {
    uint64_t start = platon_gas();
    orign = orign ^ other;
    uint64_t end = platon_gas();
    std::string result = std::string(fn) + " function, line: " + std::to_string(__LINE__);
    result += ", gas used: " + std::to_string(start - end);
    printf("%s\t\n", result.c_str());
  }

  ASSERT_EQ(orign, std::uint256_t(403));
  orign = orign ^ std::uint256_t(0xff);
  ASSERT_EQ(orign, std::uint256_t(364));
  orign = 100;
  orign = orign ^ 0xff;
  ASSERT_EQ(orign, std::uint256_t(0x9b));
  orign ^= 0xff;
  ASSERT_EQ(orign, std::uint256_t(100));
}

TEST_CASE(uint256, left_shift) {
  const char *fn = "left_shift";
  std::uint256_t orign(503);
  std::uint256_t other(100);

  {
    uint64_t start = platon_gas();
    orign = orign << 6;
    uint64_t end = platon_gas();
    std::string result = std::string(fn) + " function, line: " + std::to_string(__LINE__);
    result += ", gas used: " + std::to_string(start - end);
    printf("%s\t\n", result.c_str());
  }

  ASSERT_EQ(orign, std::uint256_t(32192));
  orign = other << 2;
  ASSERT_EQ(orign, std::uint256_t(400));
}

TEST_CASE(uint256, right_shift) {
  const char *fn = "right_shift";
  std::uint256_t orign(503);
  std::uint256_t other(100);

  {
    uint64_t start = platon_gas();
    orign = orign >> 2;
    uint64_t end = platon_gas();
    std::string result = std::string(fn) + " function, line: " + std::to_string(__LINE__);
    result += ", gas used: " + std::to_string(start - end);
    printf("%s\t\n", result.c_str());
  }

  ASSERT_EQ(orign, std::uint256_t(125));
  orign = other >> 2;
  ASSERT_EQ(orign, std::uint256_t(25));
}

TEST_CASE(int256_t, neg) {
  std::int256_t orign(503);
  std::int256_t result = -orign;
  ASSERT_EQ(result, std::int256_t(-503));
}

TEST_CASE(int256_t, add) {
  const char *fn = "add";
  std::int256_t orign(202);
  std::int256_t other(-243);

  {
    uint64_t start = platon_gas();
    orign = orign + other;
    uint64_t end = platon_gas();
    std::string result = std::string(fn) + " function, line: " + std::to_string(__LINE__);
    result += ", gas used: " + std::to_string(start - end);
    printf("%s\t\n", result.c_str());
  }

  ASSERT_EQ(orign, std::int256_t(-41));
  orign += other;
  ASSERT_EQ(orign, std::int256_t(-284));
}

TEST_CASE(int256_t, compare) {
  const char *fn = "compare";
  std::int256_t orign(202);
  std::int256_t other(-243);
  bool compare_result;

  {
    uint64_t start = platon_gas();
    compare_result = orign < other;
    uint64_t end = platon_gas();
    std::string result = std::string(fn) + " function, line: " + std::to_string(__LINE__);
    result += ", gas used: " + std::to_string(start - end);
    printf("%s\t\n", result.c_str());
  }

  ASSERT_EQ(compare_result, false);
  orign += other;
  compare_result = orign < other;
  ASSERT_EQ(compare_result, false);
  orign += other;
  compare_result = orign < other;
  ASSERT_EQ(compare_result, true);
  orign = -200;
  compare_result = orign < -300;
  ASSERT_EQ(compare_result, false);
}

TEST_CASE(uint256, overflow) {
  std::uint256_t orign;
  const char * fn = "string_convert";
  {
    uint64_t start = platon_gas();
    orign =
      "41234123412341234123412341234123412341234123412341234123412341234123412341234"_uint256;
    uint64_t end = platon_gas();
    std::string result = std::string(fn) + " function, line: " + std::to_string(__LINE__);
    result += ", gas used: " + std::to_string(start - end);
    printf("%s\t\n", result.c_str());
  }
  std::uint256_t other = orign;
  std::uint256_t result = orign + other;
  ASSERT_EQ(
      result,
      "82468246824682468246824682468246824682468246824682468246824682468246824682468"_uint256);
}

TEST_CASE(uint512, overflow) {
  const char *fn = "overflow";
  std::uint512_t orign;

  {
    uint64_t start = platon_gas();
    orign =
        "3412341234123412341234123412341234123412341234123412341234123412341234123412341234123412341234123412341234123412341234123412341234123412341234123412341234"_uint512;
    uint64_t end = platon_gas();
    std::string result = std::string(fn) + " function, line: " + std::to_string(__LINE__);
    result += ", gas used: " + std::to_string(start - end);
    printf("%s\t\n", result.c_str());
  }

  std::uint512_t other = orign;
  std::uint512_t result = orign + other;
  ASSERT_EQ(
      result,
      "6824682468246824682468246824682468246824682468246824682468246824682468246824682468246824682468246824682468246824682468246824682468246824682468246824682468"_uint512);
}

TEST_CASE(uint256, exp) {
  std::uint256_t left(10);
  std::uint256_t right(5);
  std::uint256_t mod(17);
  const char *fn = "exp";
  std::uint256_t result;

  {
    uint64_t start = platon_gas();
    result = Exp(left, right, mod);
    uint64_t end = platon_gas();
    std::string result = std::string(fn) + " function, line: " + std::to_string(__LINE__);
    result += ", gas used: " + std::to_string(start - end);
    printf("%s\t\n", result.c_str());
  }

  ASSERT_EQ(result, std::uint256_t(6));
}

TEST_CASE(uint256, encode) {
  const char *fn = "encode";
  std::uint256_t result;
  std::vector<uint8_t> test_bytes{0x01, 0x02};

  {
    uint64_t start = platon_gas();
    result.FromBigEndian(test_bytes);
    uint64_t end = platon_gas();
    std::string result = std::string(fn) + " function, line: " + std::to_string(__LINE__);
    result += ", gas used: " + std::to_string(start - end);
    printf("%s\t\n", result.c_str());
  }

  ASSERT_EQ(result, std::uint256_t(258));

  std::vector<uint8_t> new_bytes;
  auto func = [](std::vector<uint8_t> &result, uint8_t one) {
    result.push_back(one);
  };
  {
    uint64_t start = platon_gas();
    result.ToBigEndian(new_bytes, func);
    uint64_t end = platon_gas();
    std::string result = std::string(fn) + " function, line: " + std::to_string(__LINE__);
    result += ", gas used: " + std::to_string(start - end);
    printf("%s\t\n", result.c_str());
  }

  auto it = new_bytes.begin();
  while (1) {
    if (it == new_bytes.end()) break;

    if (0 == *it) {
      it = new_bytes.erase(it);
    } else {
      break;
    }
  }

  ASSERT_EQ(new_bytes, test_bytes);
}

TEST_CASE(int256, rlp) {
  GasUsed(__LINE__, "int256_rlp");
  std::vector<std::int256_t> cases = {
      0,
      -1,
      1,
      -123456789,
      std::numeric_limits<int128_t>::min(),
      std::numeric_limits<uint128_t>::max(),
      std::int256_t(-1) >> 1,
      -std::int256_t("0x0FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
                     "FFFFFFFFF")};
  for (auto res : cases) {
    RLPStream r;
    r << res;
    RLP p(r.out());
    std::int256_t a2;
    fetch(p, a2);
    ASSERT_EQ(a2, res, "");
  }
}

TEST_CASE(uint256, rlp) {
  GasUsed(__LINE__, "uint256_rlp");
  std::vector<std::uint256_t> cases = {
      0,
      -1,
      1,
      123456789,
      std::numeric_limits<uint64_t>::min(),
      std::numeric_limits<uint128_t>::max(),
      std::uint256_t(1) >> 1,
      std::uint256_t("0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
                     "FFFFFFFFF")};
  for (auto res : cases) {
    RLPStream r;
    r << res;
    RLP p(r.out());
    std::uint256_t a2;
    fetch(p, a2);
    ASSERT_EQ(a2, res, "");
  }
}

TEST_CASE(uint256, valid_bytes) {
  GasUsed(__LINE__, "valid_bytes");

  std::vector<std::pair<std::uint256_t, size_t>> cases = {
      {0, 0},
      {1, 1},
      {0xFF, 1},
      {0xFFFF, 2},
      {0xFFFFFF, 3},
      {std::numeric_limits<uint128_t>::max(), 16},
      {std::uint256_t(
           "0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
           "FFFFFFFFF"),
       32}};
  for (auto res : cases) {
    ASSERT_EQ(res.first.ValidBytes(), res.second, "cal bytes error",
              toHex(res.first.Value()), "actual:", res.first.ValidBytes(),
              "expect:", res.second);
  }
}

TEST_CASE(uint256, limit) {
  ASSERT_EQ(std::numeric_limits<std::uint256_t>::is_signed, false);
  ASSERT_EQ(std::numeric_limits<std::uint256_t>::is_integer, true);
  ASSERT_EQ(std::numeric_limits<std::uint256_t>::is_iec559, false);
  ASSERT_EQ(std::numeric_limits<std::uint256_t>::min(), 0);
  ASSERT_EQ(
      std::numeric_limits<std::uint256_t>::max(),
      "115792089237316195423570985008687907853269984665640564039457584007913129639935"_uint256);
}
UNITTEST_MAIN() {
  RUN_TEST(uint256, compile);
  RUN_TEST(uint256, assign);
  RUN_TEST(uint256, add);
  RUN_TEST(uint256, sub);
  RUN_TEST(uint256, multip);
  RUN_TEST(uint256, division);
  RUN_TEST(uint256, mod);
  RUN_TEST(uint256, and);
  RUN_TEST(uint256, or);
  RUN_TEST(uint256, xor);
  RUN_TEST(uint256, left_shift);
  RUN_TEST(uint256, right_shift);
  RUN_TEST(int256_t, neg);
  RUN_TEST(int256_t, add);
  RUN_TEST(int256_t, compare);
  RUN_TEST(uint256, overflow);
  RUN_TEST(uint512, overflow);
  RUN_TEST(uint256, exp);
  RUN_TEST(uint256, encode);
  RUN_TEST(int256, rlp);
  RUN_TEST(uint256, rlp);
  RUN_TEST(uint256, valid_bytes);
  RUN_TEST(uint256, limit);
}