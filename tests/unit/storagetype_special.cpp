#include "log.h"
#include "platon/name.hpp"
#include "platon/print.hpp"
#include "platon/storagetype.hpp"
#include "unittest.hpp"

using namespace platon;

TEST_CASE(testStorage, uint8) {
  typedef platon::Uint8<"test_uint8"_n> TestUint8;

  TestUint8 t;
  *t = 10;

  ASSERT(*t == 10);
  ASSERT(t.get() == 10);
}

TEST_CASE(testStorage, int8) {
  typedef platon::Int8<"test_int8"_n> TestInt8;
  TestInt8 t;
  *t = 10;

  ASSERT(*t == 10);
  ASSERT(t.get() == 10);
}

TEST_CASE(testStorage, uint16) {
  typedef platon::Uint16<"test_uint16"_n> TestUint16;
  TestUint16 t;
  *t = 10;

  ASSERT(*t == 10);
  ASSERT(t.get() == 10);
}

TEST_CASE(testStorage, int16) {
  typedef platon::Int16<"test_int16"_n> TestInt16;
  TestInt16 t;
  *t = 666;

  ASSERT(*t == 666);
  ASSERT(t.get() == 666);
}

TEST_CASE(testStorage, uint) {
  typedef platon::Uint<"test_uint"_n> TestUint;
  TestUint t;
  *t = 6767;

  ASSERT(*t == 6767);
  ASSERT(t.get() == 6767);
}

TEST_CASE(testStorage, int) {
  typedef platon::Int<"test_int"_n> TestInt;
  TestInt t;
  *t = 678;

  ASSERT(*t == 678);
  ASSERT(t.get() == 678);
}

TEST_CASE(testStorage, uint64) {
  typedef platon::Uint64<"test_uint64"_n> TestUint64;
  TestUint64 t;
  *t = 888;

  ASSERT(*t == 888);
  ASSERT(t.get() == 888);
}

TEST_CASE(testStorage, int64) {
  typedef platon::Int64<"test_int64"_n> TestInt64;
  TestInt64 t;
  *t = 8888;

  ASSERT(*t == 8888);
  ASSERT(t.get() == 8888);
}

TEST_CASE(testStorage, string) {
  typedef platon::String<"test_string"_n> TestString;
  TestString str;
  *str = "hello";
  ::clearLog();
  platon::print(str->c_str());
  ASSERT(test::getLog() == "hello");

  ::clearLog();
  platon::print((*str).c_str());
  ASSERT(test::getLog() == "hello");

  ::clearLog();
  str->append("platon");
  platon::print(str->c_str());
  ASSERT(test::getLog() == "helloplaton");

  ::clearLog();
  str->assign("Hello, PlatON!");
  platon::print(str->c_str());
  ASSERT(test::getLog() == "Hello, PlatON!");

  ::clearLog();
  platon::print(str.get().c_str());
  ASSERT(test::getLog() == "Hello, PlatON!");
}

TEST_CASE(testStorage, vector) {
  typedef platon::Vector<"test_vector"_n, int> TestVec;
  TestVec vec;
  for (auto i = 0; i < 10; i++) {
    (*vec).push_back(i);
  }

  for (auto i = 0; i < 10; i++) {
    ASSERT((*vec)[i] == i);
  }
  ASSERT(vec.get()[0] == 0);
  ASSERT(vec->front() == 0);
  ASSERT(vec->back() == 9);

  int i = 0;
  for (auto it = vec->begin(); it != vec->end(); ++it, i++) {
    ASSERT(*it == i);
  }

  vec->clear();
  ASSERT(vec->size() == 0);

  for (auto i = 0; i < 10; i++) {
    vec->push_back(i);
  }
  ASSERT(vec->size() == 10);
}

TEST_CASE(testStorage, set) {
  typedef platon::Set<"test_set"_n, int> TestSet;
  TestSet ts;
  for (auto i = 0; i < 10; i++) {
    (*ts).insert(i);
  }

  int i = 0;
  for (auto it = ts->begin(); it != ts->end(); ++it, i++) {
    ASSERT(*it == i);
  }

  ts->erase(8);
  ASSERT(ts->size() == 9);

  ts->clear();
  ASSERT(ts->size() == 0);
}

TEST_CASE(testStorage, map) {
  typedef platon::Map<"test_map"_n, std::string, std::string> TestMap;
  TestMap tm;

  (*tm)["hello"] = "platon";
  (*tm).insert(std::make_pair("key", "value"));
  tm->insert(std::make_pair("k", "v"));

  ASSERT(strcmp("platon", (*tm)["hello"].c_str()) == 0);
  ASSERT(strcmp("value", (*tm)["key"].c_str()) == 0);
  ASSERT(strcmp("v", (*tm)["k"].c_str()) == 0);
  ASSERT(strcmp("v", tm.get()["k"].c_str()) == 0);

  tm->erase("key");
  ASSERT(tm->find("key") == tm->end());

  tm->clear();
  ASSERT(tm->size() == 0);
}

TEST_CASE(testStorage, array) {
  typedef platon::Array<"test_array"_n, int, 10> TestArray;
  TestArray arr;
  for (auto i = 0; i < 10; i++) {
    (*arr)[i] = i;
  }

  for (auto i = 0; i < 10; i++) {
    ASSERT((*arr)[i] == i);
  }
  ASSERT(arr->at(0) == 0);
  ASSERT(arr->front() == 0);
  ASSERT(arr->back() == 9);
}

TEST_CASE(testStorage, tuple) {
  typedef platon::Tuple<"test_tuple"_n, int, std::string, int> TestTuple;
  TestTuple tpl;
  *tpl = std::make_tuple(10, "test", 9);

  ASSERT(std::get<0>(*tpl) == 10);
  ::clearLog();
  ASSERT(strcmp("test", std::get<1>(*tpl).c_str()) == 0);
  ASSERT(std::get<2>(*tpl) == 9);
}

UNITTEST_MAIN() {
  RUN_TEST(testStorage, uint8);
  RUN_TEST(testStorage, int8);
  RUN_TEST(testStorage, uint16);
  RUN_TEST(testStorage, int16);
  RUN_TEST(testStorage, uint);
  RUN_TEST(testStorage, int);
  RUN_TEST(testStorage, uint64);
  RUN_TEST(testStorage, int64);
  RUN_TEST(testStorage, string);
  RUN_TEST(testStorage, vector);
  RUN_TEST(testStorage, set);
  RUN_TEST(testStorage, map);
  RUN_TEST(testStorage, array);
  RUN_TEST(testStorage, tuple);
}
