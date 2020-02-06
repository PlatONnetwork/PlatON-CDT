#include "platon/db/map.hpp"
#include "platon/name.hpp"
#include "unit_test.hpp"

typedef platon::db::Map<"map_str"_n, std::string, std::string> MapStr;

typedef platon::db::Map<"map_insert"_n, std::string, std::string> MapInsert;

TEST_CASE(map, operator) {
  {
    MapStr map;
    map.insert("hello", "world");
    ASSERT(map["hello"] == "world", map["hello"].c_str());
    //      TRACE("map.size:", map.size());
  }

  {
    MapStr map;
    //      TRACE("map.size:", map.size());
    ASSERT(map["hello"] == "world", map["hello"].c_str());
  }

  {
    //    DEBUG("test insert");
    MapStr map;
    //      TRACE("map.size:", map.size());
    map["hello1"] = "world";
    ASSERT(map.contains("hello1"));
  }

  {
    //    DEBUG("test del");
    MapStr map;
    map.erase("hello");
    ASSERT(!map.contains("hello"));
  }

  {
    //    DEBUG("test reopen del");
    MapStr map;
    ASSERT(!map.contains("hello"));
  }
}

TEST_CASE(map, insert) {
  MapInsert map;
  map["hello"] = "world";
  map.insert_const("hello", "helloworld");
  ASSERT(map["hello"] == "helloworld");
}

UNITTEST_MAIN() {
  RUN_TEST(map, operator);
  //  RUN_TEST(map, insert);
}
