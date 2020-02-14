#include "unit_test.hpp"
#include "platon/RLP.h"
#include "platon/rlp_extend.hpp"
#include "platon/rlp_serialize.hpp"
#include "platon/cross_call.hpp"
#include <tuple>

platon::bytes global_bytes;

#ifdef __cplusplus
extern "C" {
#endif

size_t platon_get_call_output_length(){
    return global_bytes.size();
}

void platon_get_call_output(uint8_t *value){
    for (auto one :global_bytes) {
        *value = one;
        value++;
    }
}

#ifdef __cplusplus
}
#endif

TEST_CASE(cross_call, args){
    platon::bytes paras = platon::cross_call_args("init", std::string("jatel"), uint32_t(56));
    global_bytes = paras;
    std::tuple<std::string, std::string, uint32_t> call_paras;
    platon::fetch(platon::RLP(paras), call_paras);
    ASSERT_EQ(std::get<0>(call_paras), "init");
    ASSERT_EQ(std::get<1>(call_paras), "jatel");
    ASSERT_EQ(std::get<2>(call_paras), 56);
}

TEST_CASE(cross_call, values){
    uint32_t value = 4 + 256*3 + 256*256*2 + 256*256*256*1;
    platon::bytes value_bytes = platon::value_to_bytes(value);
    platon::bytes result_bytes = {1, 2, 3, 4};
    ASSERT_EQ(value_bytes, result_bytes);
}

TEST_CASE(cross_call, output){
    std::tuple<std::string, std::string, uint32_t> call_result;
    platon::get_call_output(call_result);
    ASSERT_EQ(std::get<0>(call_result), "init");
    ASSERT_EQ(std::get<1>(call_result), "jatel");
    ASSERT_EQ(std::get<2>(call_result), 56);
}

UNITTEST_MAIN() {
  RUN_TEST(cross_call, args);
  RUN_TEST(cross_call, values);
  RUN_TEST(cross_call, output);
}