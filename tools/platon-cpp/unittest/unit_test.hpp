#pragma once
#include <stdio.h>

class TestResult {
  public:
    TestResult(): testcases(0), assertions(0), failures(0), isContinue(false), skip(false) {}
    unsigned testcases;
    unsigned assertions ;
    unsigned failures;
    bool isContinue;
    bool skip;
};

#define TEST(testName, testGroup)                \
  class testGroup##testName##Test {                   \
   public:                                            \
    testGroup##testName##Test(TestResult &testResult) \
        : testResult_(testResult) {                   \
      testResult_.testcases += 1;                     \
    }                                                 \
    void run();                                       \
                                                      \
   private:                                           \
    TestResult &testResult_;                          \
    const char* name_ = #testName;                    \
    const char* group_ = #testGroup;                  \
  };                                                  \
  void testGroup##testName##Test::run()

#define RUN_TEST(testName, testGroup)                       \
  testGroup##testName##Test testGroup##testName##Instance = \
      testGroup##testName##Test(testResult);                \
  testGroup##testName##Instance.run();

#define ASSERT(cond)                                                 \
  if (!testResult_.skip) {                                           \
    if (!(cond)) {                                                   \
      if (!testResult_.isContinue) {                                 \
        testResult_.skip = true;                                     \
      }                                                              \
      testResult_.failures += 1;                                     \
      printf("file %s:%u\n", __FILE__, __LINE__);                    \
      printf("test name %s:%s\n", group_, name_);                    \
      printf("assertion failed %s\n\n", #cond);                      \
    }                                                                \
    testResult_.assertions += 1;                                     \
  }

#define EXPECT_EQ(A,B) ASSERT((A)==(B))
#define EXPECT_TRUE(A) ASSERT(A)

#define UNITTEST_MAIN()                                                 \
void testSuit(TestResult &testResult);                                  \
int main() {                                                            \
    TestResult testResult;                                              \
    testResult.isContinue = true;                                       \
    testSuit(testResult);                                               \
    printf("all test case %d\n", testResult.testcases);                 \
    printf("assertions %d\n", testResult.assertions);                   \
    printf("failures %d\n", testResult.failures);                       \
    return 0;                                                           \
}                                                                       \
void testSuit(TestResult &testResult)
