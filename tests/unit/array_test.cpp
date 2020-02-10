#define ENABLE_TRACE
#include "platon/db/array.hpp"
#include "platon/name.hpp"
#include "unit_test.hpp"

typedef platon::db::Array<"array_int"_n, uint8_t, 20> ArrayInt;
typedef platon::db::Array<"array_str"_n, std::string, 2> ArrayStr;
typedef platon::db::Array<"array_set"_n, std::string, 10> ArraySet;
typedef platon::db::Array<"name_test"_n, std::string, 3> ArrayName;

TEST_CASE(array, batch) {
  {
    ArrayInt arrayInt;
    for (size_t i = 0; i < 20; i++) {
      arrayInt[i] = i;
    }
    for (size_t i = 0; i < 10; i++) {
      arrayInt[i] = 0;
    }
  }

  {
    ArrayInt arrayInt;
    for (size_t i = 0; i < 10; i++) {
      ASSERT(arrayInt[i] == 0, "array[", i, "]", arrayInt[i]);
    }

    for (size_t i = 10; i < 20; i++) {
      ASSERT(arrayInt[i] == i, "array[", i, "]", arrayInt[i]);
    }
  }

  {
    DEBUG("test iterator");
    ArrayInt arrayInt;
    ArrayInt::iterator iter = arrayInt.begin();
    for (size_t i = 0; i < 10 && iter != arrayInt.end(); i++, iter++) {
      // TRACE("i:", i, "iter:", *iter);
      ASSERT(*iter == 0);
    }

    for (size_t i = 10; i < 20 && iter != arrayInt.end(); i++, iter++) {
      // TRACE("i:", i, "iter:", *iter);
      ASSERT(*iter == i, "iter:", *iter, "i:", i);
    }

    ArrayInt::const_iterator citer = arrayInt.cbegin();
    for (size_t i = 0; i < 10 && citer != arrayInt.cend(); i++, citer++) {
      // TRACE("i:", i, "iter:", *citer);
      ASSERT(*citer == 0);
    }

    for (size_t i = 10; i < 20 && citer != arrayInt.cend(); i++, citer++) {
      // TRACE("i:", i, "iter:", *citer);
      ASSERT(*citer == i, "iter:", *citer, "i:", i);
    }
  }

  {
    DEBUG("test  reserve iterator");
    ArrayInt arrayInt;

    ArrayInt::const_iterator citer = arrayInt.cbegin();
    for (size_t i = 0; i < 10 && citer != arrayInt.cend(); i++, citer++) {
      ASSERT(*citer == 0);
    }

    for (size_t i = 10; i < 20 && citer != arrayInt.cend(); i++, citer++) {
      ASSERT(*citer == i, "iter:", *citer, "i:", i);
    }

    ArrayInt::reverse_iterator iter = arrayInt.rbegin();
    for (size_t i = 19; i >= 10 && iter != arrayInt.rend(); i--, iter++) {
      ASSERT(*iter == i, "iter:", *iter, "i:", i);
    }

    for (size_t i = 9; i >= 0 && iter != arrayInt.rend(); i--, iter++) {
      ASSERT(*iter == 0, "iter:", *iter);
    }

    ArrayInt::const_reverse_iterator criter = arrayInt.crbegin();
    for (size_t i = 19; i >= 10 && criter != arrayInt.crend(); i--, ++criter) {
      ASSERT(*criter == i, "criter:", *criter, "i:", i);
    }

    for (size_t i = 9; i >= 0 && criter != arrayInt.crend(); i--, criter++) {
      ASSERT(*criter == 0, "criter:", *criter, "i:", i);
    }
  }
}

TEST_CASE(array, open) {
  {
    ArrayStr arrayStr;
    arrayStr[0] = "hello";
    arrayStr[1] = "world";
  }

  {
    DEBUG("test reopen");
    ArrayStr arrayStr;

    ASSERT(arrayStr[0] == "hello", "arrayStr[0]:", arrayStr[0]);
    ASSERT(arrayStr[1] == "world", "arrayStr[1]:", arrayStr[1]);
  }

  {
    DEBUG("test del");
    ArrayStr arrayStr;
    arrayStr[0] = "";
  }

  {
    DEBUG("test reopen deled");
    ArrayStr arrayStr;
    ASSERT(arrayStr[0] == "", "arrayStr[0]:", arrayStr[0]);
  }
}

TEST_CASE(array, set) {
  ArraySet array;
  array[0] = "hello";
  array.set_const(0, "helloworld");
  ASSERT_EQ(array[0], "helloworld");
  array.set_const(1, "world");
  ASSERT_EQ(array[1], "world");
}

TEST_CASE(array, name) {
  ArrayName arrName;
  arrName[0] = "test0";
  arrName[1] = "test1";
  arrName[2] = "test2";
}

UNITTEST_MAIN() {
  //  RUN_TEST(array, batch)
  //  RUN_TEST(array, open)
  //  RUN_TEST(array, set);
  RUN_TEST(array, name);
}