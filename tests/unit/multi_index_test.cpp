
#include "platon/db/multi_index.hpp"
#include <map>
#include <platon/chain.hpp>
#include <string>
#include "platon/name.hpp"
#include "platon/rlp_serialize.hpp"
#include "platon/storagetype.hpp"
#include "unit_test.hpp"
using namespace platon::db;
struct Member {
  std::string name;
  uint8_t age;
  uint8_t sex;
  uint64_t $seq_;
  void print_info() { /*println("print_info", "sex", sex);*/
  }
  std::string Name() const { return name; }
  uint8_t Age() const { return age; }
  PLATON_SERIALIZE(Member, (name)(age)(sex))
};

TEST_CASE(multi_index, unique) {
  MultiIndex<
      "table"_n, Member,
      IndexedBy<"index"_n, IndexMemberFun<Member, std::string, &Member::Name,
                                          IndexType::UniqueIndex>>,
      IndexedBy<"index2"_n, IndexMemberFun<Member, uint8_t, &Member::Age,
                                           IndexType::NormalIndex>>>
      member_table;

  auto r = member_table.emplace([&](auto &m) {
    m.age = 10;
    m.name = "yang";
    m.sex = 1;
  });
  ASSERT(r.second);

  member_table.modify(r.first, [&](auto &m) { m.sex = 15; });

  r = member_table.emplace([&](auto &m) {
    m.age = 10;
    m.name = "yang";
  });
  ASSERT(!r.second);

  auto iter = member_table.find<"index"_n>(std::string("yang"));
  ASSERT(iter->sex == 15, "sex:", iter->sex);
  ASSERT(iter != member_table.cend());

  member_table.erase(iter);
  iter = member_table.find<"index"_n>(std::string("yang"));
  ASSERT(iter == member_table.cend());
}

TEST_CASE(multi_index, normal) {
  MultiIndex<
      "tablenormal"_n, Member,
      IndexedBy<"index"_n, IndexMemberFun<Member, std::string, &Member::Name,
                                          IndexType::UniqueIndex>>,
      IndexedBy<"index2"_n, IndexMemberFun<Member, uint8_t, &Member::Age,
                                           IndexType::NormalIndex>>>
      member_table;

  auto r = member_table.emplace([&](auto &m) {
    m.age = 10;
    m.name = "yang";
    m.sex = 1;
  });
  ASSERT(r.second);

  member_table.modify(r.first, [&](auto &m) { m.sex = 15; });

  r = member_table.emplace([&](auto &m) {
    m.age = 10;
    m.name = "yang";
  });

  ASSERT(!r.second);

  auto iter = member_table.find<"index2"_n>(uint8_t(10));
  ASSERT(iter->sex == 15, "sex:", iter->sex);
  ASSERT(iter != member_table.cend());

  member_table.erase(iter);

  iter = member_table.find<"index2"_n>(uint8_t(10));
  ASSERT(iter == member_table.cend());
}
TEST_CASE(multi_index, find) {
  MultiIndex<
      "tablefind"_n, Member,
      IndexedBy<"index"_n, IndexMemberFun<Member, std::string, &Member::Name,
                                          IndexType::UniqueIndex>>,
      IndexedBy<"index2"_n, IndexMemberFun<Member, uint8_t, &Member::Age,
                                           IndexType::NormalIndex>>>
      member_table;

  auto r = member_table.emplace([&](auto &m) {
    m.age = 10;
    m.name = "yang";
    m.sex = 1;
  });
  ASSERT(r.second);
  r = member_table.emplace([&](auto &m) {
    m.age = 10;
    m.name = "hello";
    m.sex = 1;
  });
  ASSERT(r.second);
  auto iter = member_table.find<"index2"_n>(uint8_t(10));
  int size = 0;
  for (; iter != member_table.cend(); iter++) {
    ASSERT(iter->age == 10);
    size++;
  }
  ASSERT(size == 2);
  size = 0;
  ASSERT(iter == member_table.cend());
  for (; iter != member_table.cbegin(); --iter) {
    ASSERT(iter->age == 10);
    size++;
  }
  ASSERT(size == 2, "size:", size);
  ASSERT(iter == member_table.cbegin());
}

UNITTEST_MAIN() {
  RUN_TEST(multi_index, unique);
  RUN_TEST(multi_index, normal);
  RUN_TEST(multi_index, find);
}
