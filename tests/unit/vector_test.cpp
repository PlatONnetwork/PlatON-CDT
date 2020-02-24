//
// Created by yangzhou on 2/19/20.
//

#include "platon/container/vector.h"
#include <vector>
#include "platon/print.hpp"
#include "platon/storagetype.hpp"
#include "unit_test.hpp"

using namespace platon::container;
struct TypeSet {
  TypeSet() : i(2) {}
  TypeSet(int n) : i(n) {}
  TypeSet(const TypeSet& t) { i = t.i; }
  bool operator==(const TypeSet& other) const { return i == other.i; }
  bool operator<(const TypeSet& other) const { return i < other.i; }
  int i;
};

TEST_CASE(vector, construct) {
  TypeSet t;
  t.i = 123;

  vector<TypeSet> v0;
  ASSERT(v0.empty());
  ASSERT(v0.size() == 0);

  vector<TypeSet> v1(5);
  ASSERT(!v1.empty());
  ASSERT(v1.size() == 5, "v1 size", v1.size());

  for (int i = 0; i < v1.size(); ++i) {
    ASSERT(v1[i].i == 2);
  }

  TypeSet temp(4);
  vector<TypeSet> v2(5, temp);

  for (int i = 0; i < v2.size(); ++i) {
    ASSERT(v2[i].i == 4);
  }

  vector<TypeSet> v3(5, TypeSet(3));
  ASSERT(v3.size() == 5, "v3 size", v3.size());
  for (int i = 0; i < v3.size(); ++i) {
    ASSERT(v3[i].i == 3);
  }

  vector<TypeSet> v4(v3.begin(), v3.end());
  ASSERT(v4.size() == 5, "v4 size", v4.size());
  for (int i = 0; i < v4.size(); ++i) {
    ASSERT(v4[i].i == 3);
  }

  vector<TypeSet> v5 = {TypeSet(7), TypeSet(7), TypeSet(7)};
  ASSERT(v5.size() == 3);
  for (int i = 0; i < v5.size(); ++i) {
    ASSERT(v5[i].i == 7);
  }
}

TEST_CASE(vector, evaluation) {
  vector<TypeSet> v0(5, TypeSet(3));
  ASSERT(v0.size() == 5);
  vector<TypeSet> v1 = v0;
  ASSERT(v1.size() == 5);
  for (int i = 0; i < v1.size(); ++i) {
    ASSERT(v1[i].i == 3);
  }

  vector<TypeSet> v2 = std::move(v0);
  ASSERT(v2.size() == 5);

  for (int i = 0; i < v2.size(); ++i) {
    ASSERT(v2[i].i == 3);
  }

  vector<TypeSet> v3 = {TypeSet(7), TypeSet(7), TypeSet(7)};
  ASSERT(v3.size() == 3);
  for (int i = 0; i < v3.size(); ++i) {
    ASSERT(v3[i].i == 7);
  }

  v3 = {TypeSet(8), TypeSet(8), TypeSet(8), TypeSet(8)};
  ASSERT(v3.size() == 4);
  for (int i = 0; i < v3.size(); ++i) {
    ASSERT(v3[i].i == 8);
  }
}

TEST_CASE(vector, assign) {
  vector<TypeSet> v0(5, TypeSet(3));
  ASSERT(v0.size() == 5);
  vector<TypeSet> v1;
  v1.assign(v0.begin(), v0.end());
  ASSERT(v1.size() == 5);
  for (int i = 0; i < v1.size(); ++i) {
    ASSERT(v1[i].i == 3);
  }

  v1.assign(10, TypeSet(8));
  ASSERT(v1.size() == 10);

  for (int i = 0; i < v1.size(); ++i) {
    ASSERT(v1[i].i == 8);
  }

  v1.assign({TypeSet(7), TypeSet(7), TypeSet(7)});
  ASSERT(v1.size() == 3);
  for (int i = 0; i < v1.size(); ++i) {
    ASSERT(v1[i].i == 7);
  }
}

TEST_CASE(vector, front) {
  vector<TypeSet> v0(5, TypeSet(3));
  ASSERT(v0.size() == 5);
  ASSERT(v0.front().i == 3);
}
TEST_CASE(vector, back) {
  vector<TypeSet> v0(5, TypeSet(3));
  ASSERT(v0.size() == 5);
  ASSERT(v0.back().i == 3);
}

TEST_CASE(vector, data) {
  vector<TypeSet> v0(5, TypeSet(3));
  ASSERT(v0.size() == 5);
  auto begin = v0.data();
  for (int i = 0; i < 5; i++) {
    auto obj = begin + i;
    ASSERT(obj->i == 3);
  }
}

TEST_CASE(vector, iterator) {
  vector<TypeSet> v0(5, TypeSet(3));
  int count = 0;
  for (auto iter = v0.begin(); iter != v0.end(); ++iter) {
    ASSERT(iter->i == 3);
    count++;
  }
  ASSERT(count == 5);

  count = 0;
  for (auto iter = v0.cbegin(); iter != v0.cend(); ++iter) {
    ASSERT(iter->i == 3);
    count++;
  }
  ASSERT(count == 5);
  count = 0;
  for (auto iter = v0.rend() - 1; iter != v0.rbegin(); --iter) {
    ASSERT(iter->i == 3);
    count++;
  }
  ASSERT(count == 4);
  count = 0;
  for (auto iter = v0.crend() - 1; iter != v0.crbegin(); --iter) {
    ASSERT(iter->i == 3);
    count++;
  }
  ASSERT(count == 4);
}

TEST_CASE(vector, size) {
  vector<TypeSet> v0(5, TypeSet(3));
  ASSERT(v0.size() == 5);
  size_t size = (size_t)1 << (sizeof(size_t) * 8 - 1);
  ASSERT(v0.max_size() == size);
}

TEST_CASE(vector, empty) {
  vector<TypeSet> v0(5, TypeSet(3));
  ASSERT(!v0.empty());
}

TEST_CASE(vector, reserve) {
  vector<TypeSet> v0(5, TypeSet(3));
  v0.reserve(10);
  ASSERT(v0.capacity() == 10);
}

TEST_CASE(vector, clear) {
  vector<TypeSet> v0(5, TypeSet(3));
  v0.clear();
  ASSERT(v0.empty());
}

TEST_CASE(vector, shrink_to_fit) {
  vector<TypeSet> v0(5, TypeSet(3));
  ASSERT(v0.capacity() > 0);
  v0.clear();
  ASSERT(v0.empty());
  v0.shrink_to_fit();
  ASSERT(v0.capacity() == 2, "capacity", v0.capacity(), "size", v0.size());
}

TEST_CASE(vector, insert) {
  vector<TypeSet> v0(5, TypeSet(3));
  auto iter = v0.insert(v0.begin(), TypeSet(4));
  ASSERT(iter == v0.begin());
  ASSERT(v0.size() == 6);
  ASSERT(v0.begin()->i == 4, "begin", v0.begin()->i);

  auto tmp = TypeSet(5);
  iter = v0.insert(v0.begin(), tmp);
  ASSERT(iter == v0.begin());
  ASSERT(v0.size() == 7);
  ASSERT(v0.begin()->i == 5);

  iter = v0.insert(v0.begin(), 8, tmp);
  ASSERT(iter == v0.begin());
  ASSERT(v0.size() == 15);
  for (auto i = 0; i < 8; ++i) {
    ASSERT(v0[i].i == 5);
  }

  vector<TypeSet> v1(5, TypeSet(6));

  iter = v0.insert(v0.begin(), v1.begin(), v1.end());
  ASSERT(iter == v0.begin());
  ASSERT(v0.size() == 20);
  for (auto i = 0; i < 5; ++i) {
    ASSERT(v0[i].i == 6);
  }

  iter = v0.insert(v0.begin(), {TypeSet(7), TypeSet(7), TypeSet(7)});
  ASSERT(v0.size() == 23);
  for (auto i = 0; i < 3; ++i) {
    ASSERT(v0[i].i == 7);
  }
  ASSERT(iter == v0.begin());
}

TEST_CASE(vector, emplace) {
  vector<TypeSet> v0(5, TypeSet(3));
  auto iter = v0.emplace(v0.begin(), 4);
  ASSERT(iter == v0.begin());
  ASSERT(v0.size() == 6);
  ASSERT(v0.begin()->i == 4);
  iter = v0.emplace(v0.begin(), TypeSet(5));
  ASSERT(iter == v0.begin());
  ASSERT(v0.size() == 7);
  ASSERT(v0.begin()->i == 5);
}

TEST_CASE(vector, erase) {
  vector<TypeSet> v0(5, TypeSet(3));
  auto iter = v0.erase(v0.begin());
  ASSERT(iter == v0.begin());
  ASSERT(v0.size() == 4);

  iter = v0.erase(v0.begin(), v0.end());
  ASSERT(v0.empty(), "size", v0.size());
  ASSERT(iter == v0.end(), "distance", v0.end() - iter);
}

TEST_CASE(vector, push_back) {
  vector<TypeSet> v0;
  for (int i = 0; i < 5; i++) {
    v0.push_back(TypeSet(3));
  }
  ASSERT(v0.size() == 5);

  for (int i = 0; i < 5; i++) {
    ASSERT(v0[i].i == 3);
  }
  auto tmp = TypeSet(7);

  for (int i = 0; i < 5; i++) {
    v0.push_back(tmp);
  }
  ASSERT(v0.size() == 10);
  for (int i = 5; i < 10; i++) {
    ASSERT(v0[i].i == 7);
  }
}

TEST_CASE(vector, emplace_back) {
  vector<TypeSet> v0;
  for (int i = 0; i < 5; i++) {
    auto obj = v0.emplace_back(TypeSet(3));
    ASSERT(obj.i == 3);
  }
  ASSERT(v0.size() == 5);

  for (int i = 0; i < 5; i++) {
    ASSERT(v0[i].i == 3);
  }
  auto tmp = TypeSet(7);

  for (int i = 0; i < 5; i++) {
    auto obj = v0.emplace_back(tmp);
    ASSERT(obj.i == 7);
  }
  ASSERT(v0.size() == 10);
  for (int i = 5; i < 10; i++) {
    ASSERT(v0[i].i == 7);
  }
}

TEST_CASE(vector, pop_back) {
  vector<TypeSet> v0(5, TypeSet(3));
  for (int i = 1; i <= 5; i++) {
    v0.pop_back();
    ASSERT(v0.size() == 5 - i);
  }
}

TEST_CASE(vector, resize) {
  TypeSet def;
  vector<TypeSet> v0(5, TypeSet(3));
  v0.resize(10);
  ASSERT(v0.size() == 10);
  for (int i = 0; i < 5; i++) {
    ASSERT(v0[i].i == 3);
  }
  for (int i = 5; i < 10; i++) {
    ASSERT(v0[i].i == 2);
  }
  auto tmp = TypeSet(4);
  v0.resize(20, tmp);
  ASSERT(v0.size() == 20);
  for (int i = 10; i < 20; i++) {
    ASSERT(v0[i].i == 4);
  }
  v0.resize(1);
  ASSERT(v0.size() == 1);
  ASSERT(v0[0].i == 3);
}

TEST_CASE(vector, compare) {
  vector<TypeSet> v0(5, TypeSet(3));
  vector<TypeSet> v1(5, TypeSet(3));
  ASSERT(v0 == v1);
  ASSERT(!(v0 != v1));
  ASSERT(v0 <= v1);
  ASSERT(v0 >= v1);
  ASSERT(!(v0 < v1));
  ASSERT(!(v0 > v1));
}

TEST_CASE(vector, rlp) {
  vector<int> v0(6, 4);
  set_state("hello", v0);

  vector<int> v1;

  get_state("hello", v1);
  ASSERT(v0 == v1);
  for (int i = 0; i < v1.size(); i++) {
    ASSERT(v1[i] == 4);
  }

  v1.assign(7, 5);
  for (int i = 0; i < v1.size(); i++) {
    ASSERT(v1[i] == 5);
  }
  RLPStream vrlp;
  vector<uint64_t> v2;
  vrlp << v1;

  vector<byte> v3;
  fetch(RLP(vrlp.out()), v3);
  for (int i = 0; i < v3.size(); i++) {
    ASSERT(v3[i] == 5);
  }
}

TEST_CASE(vector, swap) {
  vector<byte> v0(6, 4);
  vector<byte> v1(4, 3);
  std::swap(v1, v0);
  ASSERT(v1.size() == 6);
  ASSERT(v0.size() == 4);
  for (int i = 0; i < 6; i++) {
    ASSERT(v1[i] == 4);
  }
  for (auto const& i : v0) {
    ASSERT(i == 3);
  }
}

TEST_CASE(vector, storage) {
  Vector<"name"_n, uint64_t> a;
  a->push_back(10);
}
UNITTEST_MAIN() {
  RUN_TEST(vector, construct);
  RUN_TEST(vector, evaluation);
  RUN_TEST(vector, assign);
  RUN_TEST(vector, front);
  RUN_TEST(vector, back);
  RUN_TEST(vector, data);
  RUN_TEST(vector, iterator);
  RUN_TEST(vector, size);
  RUN_TEST(vector, empty);
  RUN_TEST(vector, reserve);
  RUN_TEST(vector, clear);
  RUN_TEST(vector, shrink_to_fit);
  RUN_TEST(vector, insert);
  RUN_TEST(vector, emplace);
  RUN_TEST(vector, erase);
  RUN_TEST(vector, push_back);
  RUN_TEST(vector, emplace_back);
  RUN_TEST(vector, pop_back);
  RUN_TEST(vector, resize);
  RUN_TEST(vector, compare);
  RUN_TEST(vector, rlp);
  RUN_TEST(vector, swap);
  RUN_TEST(vector, storage);
}