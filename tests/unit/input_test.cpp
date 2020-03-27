#include "platon/contract.hpp"
#include "platon/dispatcher.hpp"
#include "platon/print.hpp"
#include "platon/rlp_serialize.hpp"
#include "unit_test.hpp"
#include <string>
#include <vector>

using namespace platon;

std::vector<byte> get_input_bytes();

#ifdef __cplusplus
extern "C" {
#endif

std::vector<byte> input_result = get_input_bytes();

size_t platon_get_input_length(void) { return input_result.size(); }

void platon_get_input(uint8_t *value) {
  for (auto one : input_result) {
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
  friend bool operator==(const Info &lhs, const Info &rhs) {
    return lhs.number_ == rhs.number_ && lhs.age_ == rhs.age_ &&
           lhs.married_ == rhs.married_ && lhs.desc_ == rhs.desc_ &&
           lhs.children_ == rhs.children_;
  }
};

std::vector<byte> get_input_bytes() {
  std::string str_method = "init";
  uint64_t t_method = Name(str_method).value;
  uint64_t serial = 5;
  Info info;
  info.number_ = 10;
  info.age_ = 30;
  info.married_ = 11;
  info.desc_ = "input_test";
  info.children_ = std::vector<std::string>{"children1", "children2"};
  RLPStream stream(3);
  stream << t_method << serial << info;
  return bytes(stream.out().begin(), stream.out().end());
}

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

void testSuit(TestResult &testResult) { RUN_TEST(input, string); }

CONTRACT InputTest : public platon::Contract{public : ACTION uint64_t init(
                         uint64_t serial, Info args_info){g_serial = serial;
g_args = args_info;
TestResult testResult;
testResult.isContinue = true;
testSuit(testResult);
println("all test case", testResult.testcases, "assertions",
        testResult.assertions, "failures", testResult.failures);
return 11;
}
}
;

PLATON_DISPATCH(InputTest, (init))
