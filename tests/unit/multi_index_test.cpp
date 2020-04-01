
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
      unique_multi_type;

  ASSERT(unique_multi_type::has_unique_index());

  typedef MultiIndex<
      "table"_n, Member,
      IndexedBy<"index2"_n, IndexMemberFun<Member, uint8_t, &Member::Age,
                                           IndexType::NormalIndex>>>
      normal_multi_type;

  ASSERT(!normal_multi_type::has_unique_index());

  // check_index_unique
  ASSERT(unique_multi_type::check_index_unique<"index"_n>());
  ASSERT(!unique_multi_type::check_index_unique<"index2"_n>());

  // check_index_exist
  ASSERT(unique_multi_type::check_index_exist<"index"_n>());
  ASSERT(!unique_multi_type::check_index_exist<"jatel"_n>());

  unique_multi_type member_table;

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
  ASSERT(r.first == member_table.cend());

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

  r = member_table.emplace([&](auto &m) {
    m.age = 10;
    m.name = "feng";
    m.sex = 1;
  });
  ASSERT(r.second);
  r.first->print_info();

  r = member_table.emplace([&](auto &m) {
    m.age = 10;
    m.name = "chen";
    m.sex = 1;
  });
  ASSERT(r.second);
  r.first->print_info();

  println("------------------");
  // 遍历
  auto it_end = member_table.cend();
  for (auto it = member_table.cbegin(); it != it_end; ++it) {
    it->print_info();
  }

  println("------------------");
  // 更改

  auto iter = member_table.find<"index"_n>(std::string("wang"));
  member_table.modify(iter, [&](auto &m) { m.sex = 15; });
  iter = member_table.find<"index"_n>(std::string("wang"));
  ASSERT(iter != member_table.cend());
  ASSERT(iter->sex == 15, "sex:", iter->sex);
  iter->print_info();
  size_t count = member_table.count<"index"_n>(std::string("wang"));
  ASSERT(1 == count);

  // 删除
  member_table.erase(iter);
  iter = member_table.find<"index"_n>(std::string("wang"));
  ASSERT(iter == member_table.cend());
  count = member_table.count<"index"_n>(std::string("wang"));
  ASSERT(0 == count);

  // 删除
  iter = member_table.find<"index"_n>(std::string("zhao"));
  ASSERT(iter != member_table.cend());
  ASSERT(iter->sex == 1, "sex:", iter->sex);
  iter->print_info();
  count = member_table.count<"index"_n>(std::string("zhao"));
  ASSERT(count == 1);
  member_table.erase(iter);
  iter = member_table.find<"index"_n>(std::string("zhao"));
  ASSERT(iter == member_table.cend());
  count = member_table.count<"index"_n>(std::string("zhao"));
  ASSERT(count == 0);

  // 更改
  iter = member_table.find<"index"_n>(std::string("qian"));
  member_table.modify(iter, [&](auto &m) { m.sex = 15; });
  iter = member_table.find<"index"_n>(std::string("qian"));
  ASSERT(iter != member_table.cend());
  ASSERT(iter->sex == 15, "sex:", iter->sex);
  iter->print_info();
  count = member_table.count<"index"_n>(std::string("qian"));
  ASSERT(count == 1);

  println("------------------");

  // 遍历
  it_end = member_table.cend();
  for (auto it = member_table.cbegin(); it != it_end; ++it) {
    it->print_info();
  }

  // 删除
  iter = member_table.find<"index"_n>(std::string("zhou"));
  ASSERT(iter != member_table.cend());
  count = member_table.count<"index"_n>(std::string("zhou"));
  ASSERT(1 == count);
  iter->print_info();
  member_table.erase(iter);
  iter = member_table.find<"index"_n>(std::string("zhou"));
  ASSERT(iter == member_table.cend());
  count = member_table.count<"index"_n>(std::string("zhou"));
  ASSERT(0 == count);

  println("------------------");

  // 查找
  count = member_table.count<"index2"_n>(uint8_t(10));
  ASSERT(count == 7);

  auto index = member_table.get_index<"index2"_n>();
  auto index_iter = index.cbegin(uint8_t(10));
  for (; index_iter != index.cend(uint8_t(10)); ++index_iter) {
    index_iter->print_info();
  }

  println("------------------");
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
  member_table.erase(r.first);

  // 查找
  auto iter = member_table.find<"index"_n>(std::string("yang"));
  ASSERT(iter == member_table.cend());
  auto count = member_table.count<"index"_n>(std::string("yang"));
  ASSERT(0 == count);
  auto index = member_table.get_index<"index2"_n>();
  auto index_iter = index.cbegin(uint8_t(10));
  ASSERT(index_iter == index.cend(uint8_t(10)));

  // 增加
  for (int i = 0; i < 100; ++i) {
    std::string name = "jatel" + std::to_string(i);
    auto r = member_table.emplace([&](auto &m) {
      m.age = 10;
      m.name = name;
      m.sex = 1;
    });
    ASSERT(r.second);
  }

  count = member_table.count<"index2"_n>(uint8_t(10));
  ASSERT(count == 100);
  count = 0;
  index = member_table.get_index<"index2"_n>();
  for (auto it = index.cbegin(uint8_t(10)); it != index.cend(uint8_t(10));
       ++it) {
    ++count;
  }
  ASSERT(count == 100);

  count = 0;
  for (auto it = member_table.cbegin(); it != member_table.cend(); ++it) {
    ++count;
  }
  ASSERT(count == 100);

  // 删除
  for (int i = 0; i < 32; ++i) {
    std::string name = "jatel" + std::to_string(i);
    iter = member_table.find<"index"_n>(name);
    ASSERT(iter != member_table.cend());
    member_table.erase(iter);
  }

  count = member_table.count<"index2"_n>(uint8_t(10));
  ASSERT(count == 68);
  count = 0;
  index = member_table.get_index<"index2"_n>();
  for (auto it = index.cbegin(uint8_t(10)); it != index.cend(uint8_t(10));
       ++it) {
    ++count;
  }
  ASSERT(count == 68);

  count = 0;
  for (auto it = member_table.cbegin(); it != member_table.cend(); ++it) {
    ++count;
  }
  ASSERT(count == 68);

  // 删除
  for (int i = 99; i >= 68; --i) {
    std::string name = "jatel" + std::to_string(i);
    iter = member_table.find<"index"_n>(name);
    ASSERT(iter != member_table.cend());
    member_table.erase(iter);
  }

  count = member_table.count<"index2"_n>(uint8_t(10));
  ASSERT(count == 36);
  count = 0;
  index = member_table.get_index<"index2"_n>();
  for (auto it = index.cbegin(uint8_t(10)); it != index.cend(uint8_t(10));
       ++it) {
    ++count;
  }
  ASSERT(count == 36);

  count = 0;
  for (auto it = member_table.cbegin(); it != member_table.cend(); ++it) {
    ++count;
  }
  ASSERT(count == 36);

  // 增加
  for (int i = 68; i < 100; ++i) {
    std::string name = "jatel" + std::to_string(i);
    auto r = member_table.emplace([&](auto &m) {
      m.age = 10;
      m.name = name;
      m.sex = 1;
    });
    ASSERT(r.second);
  }

  // 删除
  for (int i = 32; i < 100; i += 3) {
    std::string name = "jatel" + std::to_string(i);
    iter = member_table.find<"index"_n>(name);
    ASSERT(iter != member_table.cend());
    member_table.erase(iter);
  }

  println("------------------");
  count = member_table.count<"index2"_n>(uint8_t(10));
  ASSERT(count == 45);
  count = 0;
  index = member_table.get_index<"index2"_n>();
  for (auto it = index.cbegin(uint8_t(10)); it != index.cend(uint8_t(10));
       ++it) {
    ++count;
  }
  ASSERT(count == 45);

  count = 0;
  for (auto it = member_table.cbegin(); it != member_table.cend(); ++it) {
    ++count;
  }
  ASSERT(count == 45);
}

TEST_CASE(multi_index, find) {
  MultiIndex<
      "tablefind"_n, Member,
      IndexedBy<"index"_n, IndexMemberFun<Member, std::string, &Member::Name,
                                          IndexType::UniqueIndex>>,
      IndexedBy<"index2"_n, IndexMemberFun<Member, uint8_t, &Member::Age,
                                           IndexType::NormalIndex>>>
      member_table;

  // 增加
  for (int i = 0; i < 5; ++i) {
    std::string name = "jatel" + std::to_string(i);
    auto r = member_table.emplace([&](auto &m) {
      m.age = 10;
      m.name = name;
      m.sex = 1;
    });
    ASSERT(r.second);
  }

  auto index = member_table.get_index<"index2"_n>();
  auto iter = index.cbegin(uint8_t(10));
  index.erase(iter);
  iter = index.cbegin(uint8_t(10));
  index.modify(iter, [&](auto &m) { m.sex = 15; });
  iter = index.cbegin(uint8_t(10));
  ASSERT(iter->sex == 15);

  size_t count = 0;
  for (auto it = member_table.cbegin(); it != member_table.cend(); ++it) {
    count++;
  }
  ASSERT(4 == count);

  count = 0;
  for (auto it = index.cbegin(uint8_t(10)); it != index.cend(uint8_t(10));
       ++it) {
    count++;
  }
  ASSERT(4 == count);
}

UNITTEST_MAIN() {
  RUN_TEST(multi_index, unique);
  RUN_TEST(multi_index, normal);
  RUN_TEST(multi_index, find);
}
