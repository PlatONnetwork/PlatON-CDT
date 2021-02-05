//
// Created by zhou.yang on 2018/11/21.
//
#pragma once
#include <vector>
#include "platon/contract.hpp"
#include "platon/print.hpp"

using namespace platon;

#ifdef __cplusplus
extern "C" {
#endif

void platon_debug_gas(uint32_t line, const char* dst, size_t len);

// Get the currently consumed gas
uint64_t platon_gas();

// Get the current time accurate to nanoseconds
int64_t platon_nano_time();

#ifdef __cplusplus
}
#endif

class GasUsed{
  public:
  GasUsed(){start_ = platon_gas();}
  GasUsed(uint32_t line) : line_(line) {start_ = platon_gas();}
  GasUsed(uint32_t line, const char * info) : line_(line), info_(info) {start_ = platon_gas();}

  ~GasUsed(){
    uint64_t end = platon_gas();
    std::string result;
    if(nullptr != info_){
      result += "info: " + std::string(info_);
    }

    if(0 != line_){
      result += " line: " + std::to_string(line_);
    }

    result += " gas used: " + std::to_string(start_ - end);

    printf("%s\t\n", result.c_str());
  }

  private:
  uint32_t line_ = 0;
  const char * info_ = nullptr;
  uint64_t start_ = 0;

};

CONTRACT TestResult : public platon::Contract {
 public:
  ACTION void init() {}
  TestResult()
      : testcases(0),
        assertions(0),
        failures(0),
        isContinue(false),
        skip(false) {}
  size_t testcases;
  size_t assertions;
  size_t failures;
  bool isContinue;
  bool skip;
};

#define TEST_CASE(testName, testGroup)                \
  class testGroup##testName##Test {                   \
   public:                                            \
    testGroup##testName##Test(TestResult& testResult) \
        : testResult_(testResult) {                   \
      testResult_.testcases += 1;                     \
    }                                                 \
    void run();                                       \
                                                      \
   private:                                           \
    TestResult& testResult_;                          \
    const std::string name_ = #testName;              \
    const std::string group_ = #testGroup;            \
  };                                                  \
  void testGroup##testName##Test::run()

#define RUN_TEST(testName, testGroup)                       \
  testGroup##testName##Test testGroup##testName##Instance = \
      testGroup##testName##Test(testResult);                \
  testGroup##testName##Instance.run();

#define ASSERT(cond, ...)                                                    \
  if (!testResult_.skip) {                                                   \
    if (!(cond)) {                                                           \
      if (!testResult_.isContinue) {                                         \
        testResult_.skip = true;                                             \
      }                                                                      \
      testResult_.failures += 1;                                             \
      println("assertion failed", #cond, "line", __LINE__, "file", __FILE__, \
              "test name", name_, "group", group_, ##__VA_ARGS__);           \
    }                                                                        \
    testResult_.assertions += 1;                                             \
  }

#define ASSERT_EQ(A, B, ...)                                               \
  if (!testResult_.skip) {                                                 \
    if (!((A) == (B))) {                                                   \
      if (!testResult_.isContinue) {                                       \
        testResult_.skip = true;                                           \
      }                                                                    \
      testResult_.failures += 1;                                           \
      println("assertion failed", #A, "==", #B, "line:", __LINE__, "file", \
              __FILE__, "func", __func__, ##__VA_ARGS__);                  \
    }                                                                      \
    testResult_.assertions += 1;                                           \
  }

#define ASSERT_NE(A, B, ...)                                              \
  if (!testResult_.skip) {                                                \
    if (!((A) != (B))) {                                                  \
      if (!testResult_.isContinue) {                                      \
        testResult_.skip = true;                                          \
      }                                                                   \
      testResult_.failures += 1;                                          \
      println("assertion failed", #A, "!=", #B, "line", __LINE__, "file", \
              __FILE__, "func", __func__, ##__VA_ARGS__);                 \
    }                                                                     \
    testResult_.assertions += 1;                                          \
  }

  //#define TEST_SUIT() \
//    void testSuit(TestResult &testResult)

#define UNITTEST_MAIN()                                              \
  void testSuit(TestResult& testResult);                             \
  extern "C" {                                                       \
  void __wasm_call_ctors();                                          \
  void __funcs_on_exit();                                            \
  size_t _invoke() {                                                 \
    TestResult testResult;                                           \
    testResult.isContinue = true;                                    \
    testSuit(testResult);                                            \
    println("all test case", testResult.testcases, "assertions",     \
            testResult.assertions, "failures", testResult.failures); \
    return testResult.failures;                                      \
  }                                                                  \
  size_t invoke() {                                                  \
    __wasm_call_ctors();                                             \
    size_t failures = _invoke();                                     \
    __funcs_on_exit();                                               \
    return failures;                                                 \
  }                                                                  \
  }                                                                  \
  void testSuit(TestResult& testResult)

namespace platon {

#ifdef OLD

RLPStream& RLPStream::appendRaw(bytesConstRef _s, size_t _itemCount) {
  m_out.append(_s.begin(), _s.end());
  noteAppended(_itemCount);
  return *this;
}

void RLPStream::pushCount(size_t _count, byte _base) {
  auto br = bytesRequired(_count);
  if (int(br) + _base > 0xff) internal::platon_throw("Count too large for RLP");
  m_out.push_back((byte)(br + _base));  // max 8 bytes.
  pushInt(_count, br);
}

void RLPStream::noteAppended(size_t _itemCount) {
  if (!_itemCount) return;
  //	cdebug << "noteAppended(" << _itemCount << ")";
  while (m_listStack.size()) {
    if (m_listStack.back().first < _itemCount)
      internal::platon_throw("itemCount too large");
    m_listStack.back().first -= _itemCount;
    if (m_listStack.back().first)
      break;
    else {
      auto p = m_listStack.back().second;
      m_listStack.pop_back();
      size_t s = m_out.size() - p;  // list size
      auto brs = bytesRequired(s);
      unsigned encodeSize = s < c_rlpListImmLenCount ? 1 : (1 + brs);
      //			cdebug << "s: " << s << ", p: " << p << ",
      // m_out.size(): " << m_out.size() << ", encodeSize: " << encodeSize << "
      //(br: " << brs << ")";
      auto os = m_out.size();
      m_out.resize(os + encodeSize);
      memmove(m_out.data() + p + encodeSize, m_out.data() + p, os - p);
      if (s < c_rlpListImmLenCount)
        m_out[p] = (byte)(c_rlpListStart + s);
      else if (c_rlpListIndLenZero + brs <= 0xff) {
        m_out[p] = (byte)(c_rlpListIndLenZero + brs);
        byte* b = &(m_out[p + brs]);
        for (; s; s >>= 8) *(b--) = (byte)s;
      } else
        internal::platon_throw("itemCount too large for RLP");
    }
    _itemCount = 1;  // for all following iterations, we've effectively appended
                     // a single item only since we completed a list.
  }
}

RLPStream& RLPStream::appendList(size_t _items) {
  //	cdebug << "appendList(" << _items << ")";
  if (_items)
    m_listStack.push_back(std::make_pair(_items, m_out.size()));
  else
    appendList(bytes());
  return *this;
}

RLPStream& RLPStream::appendList(bytesConstRef _rlp) {
  if (_rlp.size() < c_rlpListImmLenCount)
    m_out.push_back((byte)(_rlp.size() + c_rlpListStart));
  else
    pushCount(_rlp.size(), c_rlpListIndLenZero);
  appendRaw(_rlp, 1);
  return *this;
}

RLPStream& RLPStream::append(bytesConstRef _s) {
  size_t s = _s.size();
  byte const* d = _s.data();

  if (s == 1 && *d < c_rlpDataImmLenStart)
    m_out.push_back(*d);
  else {
    if (s < c_rlpDataImmLenCount)
      m_out.push_back((byte)(s + c_rlpDataImmLenStart));
    else
      pushCount(s, c_rlpDataIndLenZero);
    appendRaw(bytesConstRef(d, s), 0);
  }
  noteAppended();
  return *this;
}

RLPStream& RLPStream::append(bigint _i) {
  if (!_i)
    m_out.push_back(c_rlpDataImmLenStart);
  else if (_i < c_rlpDataImmLenStart)
    m_out.push_back((byte)_i);
  else {
    unsigned br = bytesRequired(_i);
    if (br < c_rlpDataImmLenCount)
      m_out.push_back((byte)(br + c_rlpDataImmLenStart));
    else {
      auto brbr = bytesRequired(br);
      if (c_rlpDataIndLenZero + brbr > 0xff)
        internal::platon_throw("Number too large for RLP");
      m_out.push_back((byte)(c_rlpDataIndLenZero + brbr));
      pushInt(br, brbr);
    }
    pushInt(_i, br);
  }
  noteAppended();
  return *this;
}

#endif

}  // namespace platon