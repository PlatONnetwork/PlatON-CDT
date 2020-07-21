#undef NDEBUG
#define TESTNET
#include "platon/platon.hpp"
#include "../../unit/unit_test.hpp"

std::string one_case(int number){
  std::string one;
  for(int i = 0; i < number; i++){
    one += 'a';
  }
  return one;
}

TEST_CASE(debug, bigint) {
    std::string test_case = one_case(100);
    int64_t begin_time = platon_nano_time();
    for(int i = 0; i <10000; i++){
        RLPSize rlps;
        for(int j = 0; j < 5; j++){
          rlps << test_case;
        }
        auto size = pack_size(rlps.size());
        RLPStream wht_stream;
        wht_stream.reserve(size);
        for(int j = 0; j < 5; j++){
          wht_stream << test_case;
        }
        bytesRef result = wht_stream.out();
        //print_rlp_code("bigint", "test", result);
    }
    int64_t end_time = platon_nano_time();
    printf("rlp encoding times:%d\t\n", 10000);
    printf("spent time:%lld\t\n", (end_time-begin_time)/1000000000);
}


UNITTEST_MAIN() {
  // RUN_TEST(debug, time);

  // RUN_TEST(debug, rlp);

  RUN_TEST(debug, bigint);
}