
#include "platon/db/multi_index.hpp"
#include <map>
#include <platon/chain.hpp>
#include <string>
#include "platon/name.hpp"
#include "platon/rlp_serialize.hpp"
#include "platon/storagetype.hpp"
#include "unit_test.hpp"

using namespace platon;
using namespace platon::db;
std::map<std::vector<byte>, std::vector<byte>> result;
std::vector<byte> get_vector(const uint8_t *address, size_t len) {
  byte *ptr = (byte *)address;
  std::vector<byte> vect_result;
  for (size_t i = 0; i < len; i++) {
    vect_result.push_back(*ptr);
    ptr++;
  }
  return vect_result;
}

#ifdef __cplusplus
extern "C" {
#endif

void platon_set_state(const uint8_t *key, size_t klen, const uint8_t *value,
                      size_t vlen) {
  std::vector<byte> vect_key, vect_value;
  vect_key = get_vector(key, klen);
  vect_value = get_vector(value, vlen);
  result[vect_key] = vect_value;
}

size_t platon_get_state_length(const uint8_t *key, size_t klen) {
  std::vector<byte> vect_key;
  vect_key = get_vector(key, klen);
  return result[vect_key].size();
}

int32_t platon_get_state(const uint8_t *key, size_t klen, uint8_t *value,
                         size_t vlen) {
  std::vector<byte> vect_key, vect_value;
  vect_key = get_vector(key, klen);
  vect_value = result[vect_key];
  for (size_t i = 0; i < vlen && i < vect_value.size(); i++) {
    *(value + i) = vect_value[i];
  }
  return vlen;
}

#ifdef __cplusplus
}
#endif

struct Member {
  std::string name;
  uint8_t age;
  uint8_t sex;
  uint64_t $seq_;
  void print_info() const {
    printf("name:%s, age:%d, sex:%d\t\n", name.c_str(), age, sex);
  }
  std::string Name() const { return name; }
  uint8_t Age() const { return age; }
  PLATON_SERIALIZE(Member, (name)(age)(sex))
};

TEST_CASE(multi_index, unique) {
  typedef MultiIndex<
      "table"_n, Member,
      IndexedBy<"index"_n, IndexMemberFun<Member, std::string, &Member::Name,
                                          IndexType::UniqueIndex>>,
      IndexedBy<"index2"_n, IndexMemberFun<Member, uint8_t, &Member::Age,
                                           IndexType::NormalIndex>>>
      multi_type;
  multi_type member_table;

  println(multi_type::has_unique_index());

  // 增加
  auto r = member_table.emplace([&](auto &m) {
    m.age = 10;
    m.name = "zhao";
    m.sex = 1;
  });
  ASSERT(r.second);
  r.first->print_info();

  // 重复
  r = member_table.emplace([&](auto &m) {
    m.age = 10;
    m.name = "zhao";
  });
  ASSERT(!r.second);

  r = member_table.emplace([&](auto &m) {
    m.age = 10;
    m.name = "qian";
    m.sex = 1;
  });
  ASSERT(r.second);
  r.first->print_info();

  r = member_table.emplace([&](auto &m) {
    m.age = 10;
    m.name = "sun";
    m.sex = 1;
  });
  ASSERT(r.second);
  r.first->print_info();

  r = member_table.emplace([&](auto &m) {
    m.age = 10;
    m.name = "li";
    m.sex = 1;
  });
  ASSERT(r.second);
  r.first->print_info();

  r = member_table.emplace([&](auto &m) {
    m.age = 10;
    m.name = "zhou";
    m.sex = 1;
  });
  ASSERT(r.second);
  r.first->print_info();

  r = member_table.emplace([&](auto &m) {
    m.age = 10;
    m.name = "wu";
    m.sex = 1;
  });
  ASSERT(r.second);
  r.first->print_info();

  r = member_table.emplace([&](auto &m) {
    m.age = 10;
    m.name = "zheng";
    m.sex = 1;
  });
  ASSERT(r.second);
  r.first->print_info();

  r = member_table.emplace([&](auto &m) {
    m.age = 10;
    m.name = "wang";
    m.sex = 1;
  });
  ASSERT(r.second);
  r.first->print_info();

  println("------------------");

  // 更改
  member_table.modify(r.first, [&](auto &m) { m.sex = 15; });

  // 遍历
  auto it_end = member_table.cend();
  for (auto it = member_table.cbegin(); it != it_end; ++it) {
    it->print_info();
  }

  // 删除
  auto vect_iter = member_table.find_all<"index"_n>(std::string("wang"));
  ASSERT(vect_iter.size() == 1);
  ASSERT(vect_iter[0]->sex == 15, "sex:", vect_iter[0]->sex);
  ASSERT(vect_iter[0] != member_table.cend());

  auto iter = member_table.find<"index"_n>(std::string("wang"));
  size_t count = member_table.count<"index"_n>(std::string("wang"));
  ASSERT(count == 1);
  ASSERT(iter->sex == 15, "sex:", iter->sex);
  ASSERT(iter != member_table.cend());
  member_table.erase(iter);

  // 删除
  vect_iter = member_table.find_all<"index"_n>(std::string("zhao"));
  ASSERT(vect_iter.size() == 1);
  ASSERT(vect_iter[0]->sex == 1, "sex:", vect_iter[0]->sex);
  ASSERT(vect_iter[0] != member_table.cend());

  iter = member_table.find<"index"_n>(std::string("zhao"));
  count = member_table.count<"index"_n>(std::string("zhao"));
  ASSERT(count == 1);
  ASSERT(iter->sex == 1, "sex:", iter->sex);
  ASSERT(iter != member_table.cend());
  member_table.erase(iter);

  // 查找
  vect_iter = member_table.find_all<"index"_n>(std::string("zhao"));
  ASSERT(vect_iter.size() == 0);

  iter = member_table.find<"index"_n>(std::string("zhao"));
  count = member_table.count<"index"_n>(std::string("zhao"));
  ASSERT(count == 0);
  ASSERT(iter == member_table.cend());

  // 更改
  vect_iter = member_table.find_all<"index"_n>(std::string("qian"));
  ASSERT(vect_iter.size() == 1);

  iter = member_table.find<"index"_n>(std::string("qian"));
  member_table.modify(iter, [&](auto &m) { m.sex = 15; });

  println("------------------");

  // 遍历
  it_end = member_table.cend();
  for (auto it = member_table.cbegin(); it != it_end; ++it) {
    it->print_info();
  }

  // 删除
  vect_iter = member_table.find_all<"index"_n>(std::string("zhou"));
  ASSERT(vect_iter.size() == 1);

  iter = member_table.find<"index"_n>(std::string("zhou"));
  count = member_table.count<"index"_n>(std::string("zhou"));
  ASSERT(1 == count);
  ASSERT(iter != member_table.cend());
  member_table.erase(iter);

  println("------------------");

  // 查找
  vect_iter = member_table.find_all<"index2"_n>(uint8_t(10));
  ASSERT(vect_iter.size() == 5);
  int vect_size = vect_iter.size();
  for (int i = 0; i < vect_size; ++i) {
    vect_iter[i]->print_info();
  }

  println("------------------");

  iter = member_table.find<"index2"_n>(uint8_t(10));
  count = member_table.count<"index2"_n>(uint8_t(10));
  ASSERT(count == 5);

  it_end = member_table.cend();
  for (auto it = member_table.cbegin(); it != it_end; ++it) {
    it->print_info();
  }
  println("------------------");
}

TEST_CASE(multi_index, normal) {
  MultiIndex<
      "tablenormal"_n, Member,
      IndexedBy<"index2"_n, IndexMemberFun<Member, uint8_t, &Member::Age,
                                           IndexType::NormalIndex>>,
      IndexedBy<"index"_n, IndexMemberFun<Member, std::string, &Member::Name,
                                          IndexType::UniqueIndex>>>
      member_table;

  auto r = member_table.emplace([&](auto &m) {
    m.age = 10;
    m.name = "yang";
    m.sex = 1;
  });
  ASSERT(r.second);
  r.first->print_info();
  for (auto it = member_table.cbegin(); it != member_table.cend(); ++it) {
    it->print_info();
  }

  member_table.modify(r.first, [&](auto &m) { m.sex = 15; });
  for (auto it = member_table.cbegin(); it != member_table.cend(); ++it) {
    it->print_info();
  }

  r = member_table.emplace([&](auto &m) {
    m.age = 10;
    m.name = "yang";
  });

  ASSERT(!r.second);

  auto iter = member_table.find<"index2"_n>(uint8_t(10));
  auto count = member_table.count<"index2"_n>(uint8_t(10));
  ASSERT(count == 1);
  ASSERT(iter->sex == 15, "sex:", iter->sex);
  ASSERT(iter != member_table.cend());

  member_table.erase(iter);

  iter = member_table.find<"index2"_n>(uint8_t(10));
  count = member_table.count<"index2"_n>(uint8_t(10));
  ASSERT(count == 0);
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
  r.first->print_info();
  for (auto it = member_table.cbegin(); it != member_table.cend(); ++it) {
    it->print_info();
  }

  r = member_table.emplace([&](auto &m) {
    m.age = 10;
    m.name = "hello";
    m.sex = 1;
  });
  ASSERT(r.second);
  r.first->print_info();
  for (auto it = member_table.cbegin(); it != member_table.cend(); ++it) {
    it->print_info();
  }

  auto iter = member_table.find<"index2"_n>(uint8_t(10));
  auto count = member_table.count<"index2"_n>(uint8_t(10));
  ASSERT(count == 2);
}

UNITTEST_MAIN() {
  RUN_TEST(multi_index, unique);
  // RUN_TEST(multi_index, normal);
  // RUN_TEST(multi_index, find);
}
