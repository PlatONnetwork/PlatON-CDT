//
// Created by zhou.yang on 2018/11/21.
//
#pragma once
#include <vector>
#include "platon/print.hpp"
#include "platon/contract.hpp"

using namespace platon;

class TestResult {
  public:
    TestResult(): testcases(0), assertions(0), failures(0), isContinue(false), skip(false) {}
    size_t testcases;
    size_t assertions ;
    size_t failures;
    bool isContinue;
    bool skip;
};

#define TEST_CASE(testName, testGroup)                \
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
    const std::string name_ = #testName;              \
    const std::string group_ = #testGroup;            \
  };                                                  \
  void testGroup##testName##Test::run()

#define RUN_TEST(testName, testGroup)                       \
  testGroup##testName##Test testGroup##testName##Instance = \
      testGroup##testName##Test(testResult);                \
  testGroup##testName##Instance.run();

#define ASSERT(cond, ...)                                            \
  if (!testResult_.skip) {                                           \
    if (!(cond)) {                                                   \
      if (!testResult_.isContinue) {                                 \
        testResult_.skip = true;                                     \
      }                                                              \
      testResult_.failures += 1;                                     \
      println("assertion failed", #cond, "line", __LINE__, \
                      "file", __FILE__, "test name", name_,        \
                      "group", group_, ##__VA_ARGS__);              \
    }                                                                \
    testResult_.assertions += 1;                                     \
  }

#define ASSERT_EQ(A, B, ...)                                                \
  if (!testResult_.skip) {                                                  \
    if (!((A) == (B))) {                                                    \
      if (!testResult_.isContinue) {                                        \
        testResult_.skip = true;                                            \
      }                                                                     \
      testResult_.failures += 1;                                            \
      println("assertion failed", #A, "==", #B, "line:", __LINE__, \
                      "file", __FILE__, "func", __func__, ##__VA_ARGS__); \
    }                                                                       \
    testResult_.assertions += 1;                                            \
  }

#define ASSERT_NE(A, B, ...)                                                \
  if (!testResult_.skip) {                                                  \
    if (!((A) != (B))) {                                                    \
      if (!testResult_.isContinue) {                                        \
        testResult_.skip = true;                                            \
      }                                                                     \
      testResult_.failures += 1;                                            \
      println("assertion failed", #A, "!=", #B, "line", __LINE__, \
                      "file", __FILE__, "func", __func__, ##__VA_ARGS__); \
    }                                                                       \
    testResult_.assertions += 1;                                            \
  }

//#define TEST_SUIT() \
//    void testSuit(TestResult &testResult)

#define UNITTEST_MAIN()                                                     \
ACTION void testSuit(TestResult &testResult);                                    \
extern "C" {                                                                \
    int invoke() {                                                          \
        TestResult testResult;                                              \
        testResult.isContinue = true;                                       \
        testSuit(testResult);                                               \
        println("all test case", testResult.testcases, "assertions",    \
          testResult.assertions, "failures", testResult.failures);    \
        return 0;                                                           \
    }                                                                       \
}                                                                           \
ACTION void testSuit(TestResult &testResult)
