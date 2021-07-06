#undef NDEBUG
#include "platon/platon.hpp"
#include "platon/hash/mimc.hpp"
#include "platon/bigint.hpp"
#include "unit_test.hpp"

using namespace platon::hash::mimc;
TEST_CASE(mimc, hash){
  std::vector<std::uint256_t> input = {12};
  const char *fn = "Mimc::Hash";
  platon_debug_gas(__LINE__, fn, strlen(fn));
  std::uint256_t hash_result = Mimc::Hash(input, 0);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  std::uint256_t result(std::string_view("237c92644dbddb86d8a259e0e923aaab65a93f1ec5758b8799988894ac0958fd"));
  ASSERT_EQ(hash_result, result);

  std::uint256_t mimc_hash_result = Mimc::Mimc7Hash(1, 2);
  result = std::uint256_t(std::string_view("176c6eefc3fdf8d6136002d8e6f7a885bbd1c4e3957b93ddc1ec3ae7859f1a08"));
  ASSERT_EQ(mimc_hash_result, result);

  const char *data = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";
  std::vector<uint8_t> bytes_input(reinterpret_cast<const uint8_t *>(data), reinterpret_cast<const uint8_t *>(data) + strlen(data));
  std::uint256_t bytes_hash_result = Mimc::HashBytes(bytes_input);
  result = "16855787120419064316734350414336285711017110414939748784029922801367685456065"_uint256;
  ASSERT_EQ(bytes_hash_result, result);
 }

UNITTEST_MAIN()
{
  RUN_TEST(mimc, hash);
}