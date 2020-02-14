#include "platon/contract.hpp"
#include "platon/rlp_serialize.hpp"
#include "platon/print.hpp"
#include "platon/dispatcher.hpp"
#include "unit_test.hpp"
#include <string>
#include <vector>

using namespace platon;

#ifdef __cplusplus
extern "C" {
#endif

size_t platon_get_input_length(void){
    std::vector<byte> input_result = {234, 132, 105, 110, 105, 116, 5, 227, 10, 30, 11, 138, 105, 
        110, 112, 117, 116, 95, 116, 101, 115, 116, 212, 137, 99, 104, 105, 108, 
        100, 114, 101, 110, 49, 137, 99, 104, 105, 108, 100, 114, 101, 110, 50};
    return input_result.size();
}

void platon_get_input(uint8_t *value){
    std::vector<byte> input_result = {234, 132, 105, 110, 105, 116, 5, 227, 10, 30, 11, 138, 105, 
        110, 112, 117, 116, 95, 116, 101, 115, 116, 212, 137, 99, 104, 105, 108, 
        100, 114, 101, 110, 49, 137, 99, 104, 105, 108, 100, 114, 101, 110, 50};
    for (auto one :input_result) {
        *value = one;
        value++;
    }
}

#ifdef __cplusplus
}
#endif

class Info {
    public:
        uint64_t number_;
        uint64_t age_;
        uint8_t married_;
        std::string desc_;
        std::vector<std::string> children_;
        PLATON_SERIALIZE(Info, (number_)(age_)(married_)(desc_)(children_))
        friend bool operator == (const Info &lhs, const Info &rhs){
            return lhs.number_ == rhs.number_ && lhs.age_ == rhs.age_ && lhs.married_ == rhs.married_
            && lhs.desc_ == rhs.desc_ && lhs.children_ == rhs.children_;
        }
};

uint64_t g_serial;
Info g_args;

TEST_CASE(input, string) {
    ASSERT_EQ(g_serial, 5)
    Info info;
    info.number_ = 10;
    info.age_ = 30;
    info.married_ = 11;
    info.desc_ = "input_test";
    info.children_ = std::vector<std::string>{"children1", "children2"};
    ASSERT_EQ(g_args, info)
}

void testSuit(TestResult &testResult){
    RUN_TEST(input, string);
}

CONTRACT InputTest : public platon::Contract{
    public:
        ACTION uint64_t init(uint64_t serial, Info args_info){
            g_serial = serial;
            g_args = args_info;
            TestResult testResult;                                              
            testResult.isContinue = true;                                       
            testSuit(testResult);                                               
            println("all test case", testResult.testcases, "assertions",    
            testResult.assertions, "failures", testResult.failures);
            return 11;
        }
};

PLATON_DISPATCH(InputTest, (init))
