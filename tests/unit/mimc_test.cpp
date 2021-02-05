#undef NDEBUG
#include "platon/platon.hpp"
#include "platon/mimc.hpp"
#include "platon/bigint.hpp"
#include "unit_test.hpp"

TEST_CASE(mimc, hash)
{
  std::vector<uint8_t> result(32);
  const char *data = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";
  std::vector<uint8_t> input(reinterpret_cast<const uint8_t *>(data), reinterpret_cast<const uint8_t *>(data) + strlen(data));

  const char *fn = "mimc hash bytes";
  uint64_t start = platon_gas();
  std::uint256_t hash_result = Mimc::HashBytes(input);
  uint64_t end = platon_gas();
  std::string gas_info = std::string(fn) + " function, line: " + std::to_string(__LINE__);
  gas_info += ", gas used: " + std::to_string(start - end);
  printf("%s\t\n", gas_info.c_str());

  printf("%s\t\n", std::to_string(hash_result).c_str());
}

TEST_CASE(mimc, initial)
{
  const char *fn = "mimc hash";
  uint64_t start = platon_gas();
  std::uint256_t hash_result = Mimc::Mimc7Hash(1, 2);
  uint64_t end = platon_gas();
  std::string result = std::string(fn) + " function, line: " + std::to_string(__LINE__);
  result += ", gas used: " + std::to_string(start - end);
  printf("%s\t\n", result.c_str());

  printf("%s\t\n", std::to_string(hash_result).c_str());
}

UNITTEST_MAIN()
{
  RUN_TEST(mimc, hash);
  RUN_TEST(mimc, initial);
}