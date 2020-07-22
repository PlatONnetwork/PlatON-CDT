#undef NDEBUG
#define TESTNET
#include "platon/platon.hpp"
#include "../../unit/unit_test.hpp"

constexpr bigint one_int = 0xff;
constexpr bigint four_int = 0xffffffff;
constexpr bigint eight_int = 0xffffffffffffffffllu;


TEST_CASE(debug, bigint) {
    bigint test_case = four_int;
    test_case <<= 64;
    test_case += eight_int;
    int64_t begin_time = platon_nano_time();
    for(int i = 0; i <1000000; i++){
        auto size = pack_size(test_case);
        RLPStream wht_stream;
        wht_stream.reserve(size);
        wht_stream << test_case;
        bytesRef result = wht_stream.out();
        //print_rlp_code("bigint", "test", result);
    }
    int64_t end_time = platon_nano_time();
    printf("rlp encoding times:%d\t\n", 1000000);
    printf("spent time:%lld\t\n", (end_time-begin_time)/1000000000);
}


UNITTEST_MAIN() {
  // RUN_TEST(debug, time);

  // RUN_TEST(debug, rlp);

  RUN_TEST(debug, bigint);
}