#include "platon/common.h"
#include "unittest.hpp"

namespace platon {
namespace test {
class TestReturn {
 public:
  TestReturn() : s_("test"), n_(1) {}
  const char* ReturnString() const { RETURN(s_.c_str()); }
  int32_t ReturnInt32() const { RETURN(n_); }

 private:
  std::string s_;
  int32_t n_;
};
}  // namespace test
}  // namespace platon

TEST_CASE(return_test, return_string) {
  platon::test::TestReturn t;
  ASSERT(strcmp(t.ReturnString(), "test") == 0);
}

TEST_CASE(return_test, return_int32) {
  platon::test::TestReturn t;
  ASSERT(t.ReturnInt32(), 1);
}

UNITTEST_MAIN() {
  RUN_TEST(return_test, return_string);
  RUN_TEST(return_test, return_int32);
}
