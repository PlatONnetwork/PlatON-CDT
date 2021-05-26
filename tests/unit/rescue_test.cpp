#undef NDEBUG
#include "platon/platon.hpp"
#include "platon/hash/rescue.hpp"
#include "platon/bigint.hpp"
#include "unit_test.hpp"

TEST_CASE(rescueBn256, hash){
    std::vector<std::uint256_t> input = {std::uint256_t(std::string_view("00000000000000000000000000000000000000000000000000000000000000f8"))};

    const char *fn = "Rescue::Hash";
    platon_debug_gas(__LINE__, fn, strlen(fn));
    std::uint256_t hash_result = Rescue<Bn256>::Hash(input);
    platon_debug_gas(__LINE__, fn, strlen(fn));

    std::uint256_t result = std::uint256_t(std::string_view("073908340a877ae48f3f3b13e0afe1b38c20c9e12b90e46a0217bf65506a1b59"));
    ASSERT_EQ(hash_result, result);

    input = {std::uint256_t(1), std::uint256_t(1)};
    hash_result = Rescue<Bn256>::Hash(input);
    result = std::uint256_t(std::string_view("0x1ac584a2ad2f3772779f2f6cdd8b2f286255cff623e254e0870e4cff2a587563"));
    ASSERT_EQ(hash_result, result);
}

UNITTEST_MAIN()
{
  RUN_TEST(rescueBn256, hash);
}