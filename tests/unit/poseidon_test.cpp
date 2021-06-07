#undef NDEBUG
#include "platon/platon.hpp"
#include "platon/hash/poseidon.hpp"
#include "platon/bigint.hpp"
#include "unit_test.hpp"

TEST_CASE(poseidon, hash){
    std::vector<std::uint256_t> input = {1, 2};
    const char *fn = "Poseidon::Hash";
    platon_debug_gas(__LINE__, fn, strlen(fn));
    std::uint256_t hash_result = Poseidon::Hash(input);
    platon_debug_gas(__LINE__, fn, strlen(fn));
    std::uint256_t result = "7853200120776062878684798364095072458815029376092732009249414926327459813530"_uint256;
    ASSERT_EQ(hash_result, result);

    input = {1};
    hash_result = Poseidon::Hash(input);
    result = std::uint256_t(std::string_view("29176100eaa962bdc1fe6c654d6a3c130e96a4d1168b33848b897dc502820133"));
    ASSERT_EQ(hash_result, result);
}

UNITTEST_MAIN()
{
  RUN_TEST(poseidon, hash);
}