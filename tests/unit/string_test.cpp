#include "platon/container/string.hpp"

#include <cstdlib>
#include <iomanip>
#include <list>
#include <random>
#include <sstream>
#include <string>

#include "unit_test.hpp"

using std::char_traits;
using std::initializer_list;

#define FOR_EACH_RANGE(i, begin, end) \
  for (auto i = (true ? (begin) : (end)); i < (end); ++i)

using RandomT = std::mt19937;
static uint32_t seed = 1141243124;
static RandomT rng(seed);
static const size_t maxString = 100;
static const bool avoidAliasing = true;

using namespace platon;
using namespace platon::container;

template <class Integral1, class Integral2>
Integral2 random(Integral1 low, Integral2 up) {
  std::uniform_int_distribution<> range(low, up);
  return range(rng);
}

template <class String>
void randomString(String* toFill, unsigned int maxSize = 1000) {
  assert(toFill);
  toFill->resize(random(0, maxSize));
  for (auto i = (*toFill).begin(); i != (*toFill).end(); i++) {
    *i = random('a', 'z');
  }
}

template <class String, class Integral>
void Num2String(String& str, Integral n) {
  std::string tmp = std::to_string(n);
  str = String(tmp.begin(), tmp.end());
}

std::list<char> RandomList(unsigned int maxSize) {
  std::list<char> lst(random(0u, maxSize));
  std::list<char>::iterator i = lst.begin();
  for (; i != lst.end(); ++i) {
    *i = random('a', 'z');
  }
  return lst;
}

template <class String>
void clause11_21_4_2_a(String& test, TestResult& testResult_,
                       const std::string& name_, const std::string& group_) {
  test.String::~String();
  new (&test) String();
}
template <class String>
void clause11_21_4_2_b(String& test, TestResult& testResult_,
                       const std::string& name_, const std::string& group_) {
  String test2(test);
  ASSERT(test2 == test);
}
template <class String>
void clause11_21_4_2_c(String& test, TestResult& testResult_,
                       const std::string& name_, const std::string& group_) {
  // Test move constructor. There is a more specialized test, see
  // testMoveCtor test
  String donor(test);
  String test2(std::move(donor));
  ASSERT_EQ(test2, test);
  // Technically not required, but all implementations that actually
  // support move will move large strings. Make a guess for 128 as the
  // maximum small string optimization that's reasonable.
  ASSERT(donor.size() <= 128);
}

template <class String>
void clause11_21_4_2_d(String& test, TestResult& testResult_,
                       const std::string& name_, const std::string& group_) {
  // Copy constructor with position and length
  const size_t pos = random(0, test.size());
  String s(test, pos,
           random(0, 9)
               ? random(0, (size_t)(test.size() - pos))
               : String::npos);  // test for npos, too, in 10% of the cases
  test = s;
}

template <class String>
void clause11_21_4_2_e(String& test, TestResult& testResult_,
                       const std::string& name_, const std::string& group_) {
  // Constructor from char*, size_t
  const size_t pos = random(0, test.size()), n = random(0, test.size() - pos);
  String before(test.data(), test.size());
  String s(test.c_str() + pos, n);
  String after(test.data(), test.size());
  ASSERT(before == after);
  test.swap(s);
}
template <class String>
void clause11_21_4_2_f(String& test, TestResult& testResult_,
                       const std::string& name_, const std::string& group_) {
  // Constructor from char*
  const size_t pos = random(0, test.size());
  String before(test.data(), test.size());
  String s(test.c_str() + pos);
  String after(test.data(), test.size());
  ASSERT_EQ(before, after);
  test.swap(s);
}
template <class String>
void clause11_21_4_2_g(String& test, TestResult& testResult_,
                       const std::string& name_, const std::string& group_) {
  // Constructor from size_t, char
  const size_t n = random(0, test.size());
  const auto c = test.front();
  test = String(n, c);
}
template <class String>
void clause11_21_4_2_h(String& test, TestResult& testResult_,
                       const std::string& name_, const std::string& group_) {
  // Constructors from various iterator pairs
  // Constructor from char*, char*
  String s1(test.begin(), test.end());
  ASSERT_EQ(test, s1);
  String s2(test.data(), test.data() + test.size());
  ASSERT_EQ(test, s2);
  // Constructor from other iterators
  std::list<char> lst;
  for (auto c : test) {
    lst.push_back(c);
  }
  String s3(lst.begin(), lst.end());
  ASSERT_EQ(test, s3);
  // Constructor from wchar_t iterators
  std::list<wchar_t> lst1;
  for (auto c : test) {
    lst1.push_back(c);
  }
  String s4(lst1.begin(), lst1.end());
  ASSERT_EQ(test, s4);
  // Constructor from wchar_t pointers
  wchar_t t[20];
  t[0] = 'a';
  t[1] = 'b';
  string s5(t, t + 2);
  ASSERT_EQ("ab", s5);
}
template <class String>
void clause11_21_4_2_i(String& test, TestResult& testResult_,
                       const std::string& name_, const std::string& group_) {
  // From initializer_list<char>
  std::initializer_list<typename String::value_type> il = {'h', 'e', 'l', 'l',
                                                           'o'};
  String s(il);
  test.swap(s);
}
template <class String>
void clause11_21_4_2_j(String& test, TestResult& testResult_,
                       const std::string& name_, const std::string& group_) {
  // Assignment from const String&
  auto size = random(0, 2000);
  String s(size, '\0');
  ASSERT_EQ(s.size(), size);
  for (int i = 0; i < s.size(); i++) {
    s[i] = random('a', 'z');
  }
  test = s;
}
template <class String>
void clause11_21_4_2_k(String& test, TestResult& testResult_,
                       const std::string& name_, const std::string& group_) {
  // Assignment from String&&
  auto size = random(0, 2000);
  String s(size, '\0');
  ASSERT_EQ(s.size(), size);
  for (int i = 0; i < s.size(); i++) {
    s[i] = random('a', 'z');
  }
  test = std::move(s);
  if (std::is_same<String, string>::value) {
    ASSERT(s.size() <= 128);
  }
}
template <class String>
void clause11_21_4_2_l(String& test, TestResult& testResult_,
                       const std::string& name_, const std::string& group_) {
  // Assignment from char*
  String s(random(0, 1000), '\0');
  size_t i = 0;
  for (; i != s.size(); ++i) {
    s[i] = random('a', 'z');
  }
  test = s.c_str();
}
template <class String>
void clause11_21_4_2_lprime(String& test, TestResult& testResult_,
                            const std::string& name_,
                            const std::string& group_) {
  // Aliased assign
  const size_t pos = random(0, test.size());
  if (avoidAliasing) {
    test = String(test.c_str() + pos);
  } else {
    test = test.c_str() + pos;
  }
}
template <class String>
void clause11_21_4_2_m(String& test, TestResult& testResult_,
                       const std::string& name_, const std::string& group_) {
  // Assignment from char
  using value_type = typename String::value_type;
  test = random(static_cast<value_type>('a'), static_cast<value_type>('z'));
}
template <class String>
void clause11_21_4_2_n(String& test, TestResult& testResult_,
                       const std::string& name_, const std::string& group_) {
  // Assignment from initializer_list<char>
  std::initializer_list<typename String::value_type> il = {'h', 'e', 'l', 'l',
                                                           'o'};
  test = il;
}

template <class String>
void clause11_21_4_3(String& test, TestResult& testResult_,
                     const std::string& name_, const std::string& group_) {
  // Iterators. The code below should leave test unchanged
  ASSERT_EQ(test.size(), test.end() - test.begin());
  ASSERT_EQ(test.size(), test.rend() - test.rbegin());
  ASSERT_EQ(test.size(), test.cend() - test.cbegin());
  ASSERT_EQ(test.size(), test.crend() - test.crbegin());

  auto s = test.size();
  test.resize(test.end() - test.begin());
  ASSERT_EQ(s, test.size());
  test.resize(test.rend() - test.rbegin());
  ASSERT_EQ(s, test.size());
}

template <class String>
void clause11_21_4_4(String& test, TestResult& testResult_,
                     const std::string& name_, const std::string& group_) {
  // exercise capacity, size, max_size
  ASSERT_EQ(test.size(), test.length());
  ASSERT(test.size() <= test.max_size());
  ASSERT(test.capacity() <= test.max_size());
  ASSERT(test.size() <= test.capacity());

  // exercise shrink_to_fit. Nonbinding request so we can't really do
  // much beyond calling it.
  auto copy = test;
  copy.reserve(copy.capacity() * 3);
  copy.shrink_to_fit();
  ASSERT_EQ(copy, test);

  // exercise empty
  std::string empty("empty");
  std::string notempty("not empty");
  if (test.empty()) {
    test = String(empty.begin(), empty.end());
  } else {
    test = String(notempty.begin(), notempty.end());
  }
}

template <class String>
void clause11_21_4_5(String& test, TestResult& testResult_,
                     const std::string& name_, const std::string& group_) {
  // exercise element access
  if (!test.empty()) {
    ASSERT_EQ(test[0], test.front());
    ASSERT_EQ(test[test.size() - 1], test.back());
    auto const i = random(0, test.size() - 1);
    ASSERT_EQ(test[i], test.at(i));
    test = test[i];
  }

  // EXPECT_THROW(test.at(test.size()), std::out_of_range);
  // EXPECT_THROW(as_const(test).at(test.size()), std::out_of_range);
}

template <class String>
void clause11_21_4_6_1(String& test, TestResult& testResult_,
                       const std::string& name_, const std::string& group_) {
  // 21.3.5 modifiers (+=)
  String test1;
  randomString(&test1);
  assert(test1.size() ==
         char_traits<typename String::value_type>::length(test1.c_str()));
  auto len = test.size();
  test += test1;
  ASSERT_EQ(test.size(), test1.size() + len);
  FOR_EACH_RANGE(i, 0, test1.size()) { ASSERT_EQ(test[len + i], test1[i]); }
  // aliasing modifiers
  String test2 = test;
  auto dt = test2.data();
  auto sz = test.c_str();
  len = test.size();
  ASSERT_EQ(memcmp(sz, dt, len), 0);
  String copy(test.data(), test.size());
  ASSERT_EQ(char_traits<typename String::value_type>::length(test.c_str()),
            len);
  test += test;
  // test.append(test);
  ASSERT_EQ(test.size(), 2 * len);
  ASSERT_EQ(char_traits<typename String::value_type>::length(test.c_str()),
            2 * len);
  FOR_EACH_RANGE(i, 0, len) {
    ASSERT_EQ(test[i], copy[i]);
    ASSERT_EQ(test[i], test[len + i]);
  }
  len = test.size();
  ASSERT_EQ(char_traits<typename String::value_type>::length(test.c_str()),
            len);
  // more aliasing
  auto const pos = random(0, test.size());
  ASSERT_EQ(
      char_traits<typename String::value_type>::length(test.c_str() + pos),
      len - pos);
  if (avoidAliasing) {
    String addMe(test.c_str() + pos);
    ASSERT_EQ(addMe.size(), len - pos);
    test += addMe;
  } else {
    test += test.c_str() + pos;
  }
  ASSERT_EQ(test.size(), 2 * len - pos);
  // single char
  len = test.size();
  test += random('a', 'z');
  ASSERT_EQ(test.size(), len + 1);
  // initializer_list
  initializer_list<typename String::value_type> il{'a', 'b', 'c'};
  test += il;
}

template <class String>
void clause11_21_4_6_2(String& test, TestResult& testResult_,
                       const std::string& name_, const std::string& group_) {
  // 21.3.5 modifiers (append, push_back)
  String s;

  // Test with a small string first
  char c = random('a', 'z');
  s.push_back(c);
  ASSERT_EQ(s[s.size() - 1], c);
  ASSERT_EQ(s.size(), 1);
  s.resize(s.size() - 1);

  randomString(&s, maxString);
  test.append(s);
  randomString(&s, maxString);
  test.append(s, random(0, s.size()), random(0, maxString));
  randomString(&s, maxString);
  test.append(s.c_str(), random(0, s.size()));
  randomString(&s, maxString);
  test.append(s.c_str());
  test.append(random(0, maxString), random('a', 'z'));
  std::list<char> lst(RandomList(maxString));
  test.append(lst.begin(), lst.end());
  c = random('a', 'z');
  test.push_back(c);
  ASSERT_EQ(test[test.size() - 1], c);
  // initializer_list
  initializer_list<typename String::value_type> il{'a', 'b', 'c'};
  test.append(il);
}

template <class String>
void clause11_21_4_6_3_a(String& test, TestResult& testResult_,
                         const std::string& name_, const std::string& group_) {
  // assign
  String s;
  randomString(&s);
  test.assign(s);
  ASSERT_EQ(test, s);
  // move assign
  test.assign(std::move(s));
  if (std::is_same<String, string>::value) {
    ASSERT(s.size() <= 128);
  }
}

template <class String>
void clause11_21_4_6_3_b(String& test, TestResult& testResult_,
                         const std::string& name_, const std::string& group_) {
  // assign
  String s;
  randomString(&s, maxString);
  test.assign(s, random(0, s.size()), random(0, maxString));
}

template <class String>
void clause11_21_4_6_3_c(String& test, TestResult& testResult_,
                         const std::string& name_, const std::string& group_) {
  // assign
  String s;
  randomString(&s, maxString);
  test.assign(s.c_str(), random(0, s.size()));
}

template <class String>
void clause11_21_4_6_3_d(String& test, TestResult& testResult_,
                         const std::string& name_, const std::string& group_) {
  // assign
  String s;
  randomString(&s, maxString);
  test.assign(s.c_str());
}

template <class String>
void clause11_21_4_6_3_e(String& test, TestResult& testResult_,
                         const std::string& name_, const std::string& group_) {
  // assign
  String s;
  randomString(&s, maxString);
  test.assign(random(0, maxString), random('a', 'z'));
}

template <class String>
void clause11_21_4_6_3_f(String& test, TestResult& testResult_,
                         const std::string& name_, const std::string& group_) {
  // assign from bidirectional iterator
  std::list<char> lst(RandomList(maxString));
  test.assign(lst.begin(), lst.end());
}

template <class String>
void clause11_21_4_6_3_g(String& test, TestResult& testResult_,
                         const std::string& name_, const std::string& group_) {
  // assign from aliased source
  test.assign(test);
}

template <class String>
void clause11_21_4_6_3_h(String& test, TestResult& testResult_,
                         const std::string& name_, const std::string& group_) {
  // assign from aliased source
  test.assign(test, random(0, test.size()), random(0, maxString));
}

template <class String>
void clause11_21_4_6_3_i(String& test, TestResult& testResult_,
                         const std::string& name_, const std::string& group_) {
  // assign from aliased source
  test.assign(test.c_str(), random(0, test.size()));
}

template <class String>
void clause11_21_4_6_3_j(String& test, TestResult& testResult_,
                         const std::string& name_, const std::string& group_) {
  // assign from aliased source
  test.assign(test.c_str());
}

template <class String>
void clause11_21_4_6_3_k(String& test, TestResult& testResult_,
                         const std::string& name_, const std::string& group_) {
  // assign from initializer_list
  initializer_list<typename String::value_type> il{'a', 'b', 'c'};
  test.assign(il);
}

template <class String>
void clause11_21_4_6_4(String& test, TestResult& testResult_,
                       const std::string& name_, const std::string& group_) {
  // insert
  String s;
  randomString(&s, maxString);
  test.insert(random(0, test.size()), s);
  randomString(&s, maxString);
  test.insert(random(0, test.size()), s, random(0, s.size()),
              random(0, maxString));
  randomString(&s, maxString);
  test.insert(random(0, test.size()), s.c_str(), random(0, s.size()));
  randomString(&s, maxString);
  test.insert(random(0, test.size()), s.c_str());
  test.insert(random(0, test.size()), random(0, maxString), random('a', 'z'));
  typename String::size_type pos = random(0, test.size());
  typename String::iterator res =
      test.insert(test.begin() + pos, random('a', 'z'));
  ASSERT_EQ(res - test.begin(), pos);
  std::list<char> lst(RandomList(maxString));
  pos = random(0, test.size());
  // Uncomment below to see a bug in gcc
  /*res = */ test.insert(test.begin() + pos, lst.begin(), lst.end());
  // insert from initializer_list
  initializer_list<typename String::value_type> il{'a', 'b', 'c'};
  pos = random(0, test.size());
  // Uncomment below to see a bug in gcc
  /*res = */ test.insert(test.begin() + pos, il);

  // Test with actual input iterators
  std::stringstream ss;
  ss << "hello cruel world";
  auto i = std::istream_iterator<char>(ss);
  test.insert(test.begin(), i, std::istream_iterator<char>());
}

template <class String>
void clause11_21_4_6_5(String& test, TestResult& testResult_,
                       const std::string& name_, const std::string& group_) {
  // erase and pop_back
  if (!test.empty()) {
    test.erase(random(0, test.size()), random(0, maxString));
  }
  if (!test.empty()) {
    // TODO: is erase(end()) allowed?
    test.erase(test.begin() + random(0, test.size() - 1));
  }
  if (!test.empty()) {
    auto const i = test.begin() + random(0, test.size());
    if (i != test.end()) {
      test.erase(i, i + random(0, size_t(test.end() - i)));
    }
  }
  if (!test.empty()) {
    // Can't test pop_back with std::string, doesn't support it yet.
    // test.pop_back();
  }
}

template <class String>
void clause11_21_4_6_6(String& test, TestResult& testResult_,
                       const std::string& name_, const std::string& group_) {
  auto pos = random(0, test.size());
  if (avoidAliasing) {
    test.replace(pos, random(0, test.size() - pos), String(test));
  } else {
    test.replace(pos, random(0, test.size() - pos), test);
  }
  pos = random(0, test.size());
  String s;
  randomString(&s, maxString);
  test.replace(pos, pos + random(0, test.size() - pos), s);
  auto pos1 = random(0, test.size());
  auto pos2 = random(0, test.size());
  if (avoidAliasing) {
    test.replace(pos1, pos1 + random(0, test.size() - pos1), String(test), pos2,
                 pos2 + random(0, test.size() - pos2));
  } else {
    test.replace(pos1, pos1 + random(0, test.size() - pos1), test, pos2,
                 pos2 + random(0, test.size() - pos2));
  }
  pos1 = random(0, test.size());
  String str;
  randomString(&str, maxString);
  pos2 = random(0, str.size());
  test.replace(pos1, pos1 + random(0, test.size() - pos1), str, pos2,
               pos2 + random(0, str.size() - pos2));
  pos = random(0, test.size());
  if (avoidAliasing) {
    test.replace(pos, random(0, test.size() - pos), String(test).c_str(),
                 test.size());
  } else {
    test.replace(pos, random(0, test.size() - pos), test.c_str(), test.size());
  }
  pos = random(0, test.size());
  randomString(&str, maxString);
  test.replace(pos, pos + random(0, test.size() - pos), str.c_str(),
               str.size());
  pos = random(0, test.size());
  randomString(&str, maxString);
  test.replace(pos, pos + random(0, test.size() - pos), str.c_str());
  pos = random(0, test.size());
  test.replace(pos, random(0, test.size() - pos), random(0, maxString),
               random('a', 'z'));
  pos = random(0, test.size());
  if (avoidAliasing) {
    auto newString = String(test);
    test.replace(test.begin() + pos,
                 test.begin() + pos + random(0, test.size() - pos), newString);
  } else {
    test.replace(test.begin() + pos,
                 test.begin() + pos + random(0, test.size() - pos), test);
  }
  pos = random(0, test.size());
  if (avoidAliasing) {
    auto newString = String(test);
    test.replace(test.begin() + pos,
                 test.begin() + pos + random(0, test.size() - pos),
                 newString.c_str(), test.size() - random(0, test.size()));
  } else {
    test.replace(test.begin() + pos,
                 test.begin() + pos + random(0, test.size() - pos),
                 test.c_str(), test.size() - random(0, test.size()));
  }
  pos = random(0, test.size());
  auto const n = random(0, test.size() - pos);
  typename String::iterator b = test.begin();
  String str1;
  randomString(&str1, maxString);
  const String& str3 = str1;
  const typename String::value_type* ss = str3.c_str();
  test.replace(b + pos, b + pos + n, ss);
  pos = random(0, test.size());
  test.replace(test.begin() + pos,
               test.begin() + pos + random(0, test.size() - pos),
               random(0, maxString), random('a', 'z'));
}

template <class String>
void clause11_21_4_6_7(String& test, TestResult& testResult_,
                       const std::string& name_, const std::string& group_) {
  std::vector<typename String::value_type> vec(random(0, maxString));
  if (vec.empty()) {
    return;
  }
  test.copy(vec.data(), vec.size(), random(0, test.size()));
}

template <class String>
void clause11_21_4_6_8(String& test, TestResult& testResult_,
                       const std::string& name_, const std::string& group_) {
  String s;
  randomString(&s, maxString);
  s.swap(test);
}

template <class String>
void clause11_21_4_7_1(String& test, TestResult& testResult_,
                       const std::string& name_, const std::string& group_) {
  // 21.3.6 string operations
  // exercise c_str() and data()
  assert(test.c_str() == test.data());
  // exercise get_allocator()
  String s;
  randomString(&s, maxString);
  // DCHECK(test.get_allocator() == s.get_allocator());
}

template <class String>
void clause11_21_4_7_2_a(String& test, TestResult& testResult_,
                         const std::string& name_, const std::string& group_) {
  String str = test.substr(random(0, test.size()), random(0, test.size()));
  Num2String(test, test.find(str, random(0, test.size())));
}

template <class String>
void clause11_21_4_7_2_a1(String& test, TestResult& testResult_,
                          const std::string& name_, const std::string& group_) {
  String str =
      String(test).substr(random(0, test.size()), random(0, test.size()));
  Num2String(test, test.find(str, random(0, test.size())));
}

template <class String>
void clause11_21_4_7_2_a2(String& test, TestResult& testResult_,
                          const std::string& name_, const std::string& group_) {
  auto const& cTest = test;
  String str = cTest.substr(random(0, test.size()), random(0, test.size()));
  Num2String(test, test.find(str, random(0, test.size())));
}

template <class String>
void clause11_21_4_7_2_b(String& test, TestResult& testResult_,
                         const std::string& name_, const std::string& group_) {
  auto from = random(0, test.size());
  auto length = random(0, test.size() - from);
  String str = test.substr(from, length);
  Num2String(test, test.find(str.c_str(), random(0, test.size()),
                             random(0, str.size())));
}

template <class String>
void clause11_21_4_7_2_b1(String& test, TestResult& testResult_,
                          const std::string& name_, const std::string& group_) {
  auto from = random(0, test.size());
  auto length = random(0, test.size() - from);
  String str = String(test).substr(from, length);
  Num2String(test, test.find(str.c_str(), random(0, test.size()),
                             random(0, str.size())));
}

template <class String>
void clause11_21_4_7_2_b2(String& test, TestResult& testResult_,
                          const std::string& name_, const std::string& group_) {
  auto from = random(0, test.size());
  auto length = random(0, test.size() - from);
  const auto& cTest = test;
  String str = cTest.substr(from, length);
  Num2String(test, test.find(str.c_str(), random(0, test.size()),
                             random(0, str.size())));
}

template <class String>
void clause11_21_4_7_2_c(String& test, TestResult& testResult_,
                         const std::string& name_, const std::string& group_) {
  String str = test.substr(random(0, test.size()), random(0, test.size()));
  Num2String(test, test.find(str.c_str(), random(0, test.size())));
}

template <class String>
void clause11_21_4_7_2_c1(String& test, TestResult& testResult_,
                          const std::string& name_, const std::string& group_) {
  String str =
      String(test).substr(random(0, test.size()), random(0, test.size()));
  Num2String(test, test.find(str.c_str(), random(0, test.size())));
}

template <class String>
void clause11_21_4_7_2_c2(String& test, TestResult& testResult_,
                          const std::string& name_, const std::string& group_) {
  const auto& cTest = test;
  String str = cTest.substr(random(0, test.size()), random(0, test.size()));
  Num2String(test, test.find(str.c_str(), random(0, test.size())));
}

template <class String>
void clause11_21_4_7_2_d(String& test, TestResult& testResult_,
                         const std::string& name_, const std::string& group_) {
  Num2String(test, test.find(random('a', 'z'), random(0, test.size())));
}

template <class String>
void clause11_21_4_7_3_a(String& test, TestResult& testResult_,
                         const std::string& name_, const std::string& group_) {
  String str = test.substr(random(0, test.size()), random(0, test.size()));
  Num2String(test, test.rfind(str, random(0, test.size())));
}

template <class String>
void clause11_21_4_7_3_b(String& test, TestResult& testResult_,
                         const std::string& name_, const std::string& group_) {
  String str = test.substr(random(0, test.size()), random(0, test.size()));
  Num2String(test, test.rfind(str.c_str(), random(0, test.size()),
                              random(0, str.size())));
}

template <class String>
void clause11_21_4_7_3_c(String& test, TestResult& testResult_,
                         const std::string& name_, const std::string& group_) {
  String str = test.substr(random(0, test.size()), random(0, test.size()));
  Num2String(test, test.rfind(str.c_str(), random(0, test.size())));
}

template <class String>
void clause11_21_4_7_3_d(String& test, TestResult& testResult_,
                         const std::string& name_, const std::string& group_) {
  Num2String(test, test.rfind(random('a', 'z'), random(0, test.size())));
}

template <class String>
void clause11_21_4_7_4_a(String& test, TestResult& testResult_,
                         const std::string& name_, const std::string& group_) {
  String str;
  randomString(&str, maxString);
  Num2String(test, test.find_first_of(str, random(0, test.size())));
}

template <class String>
void clause11_21_4_7_4_b(String& test, TestResult& testResult_,
                         const std::string& name_, const std::string& group_) {
  String str;
  randomString(&str, maxString);
  Num2String(test, test.find_first_of(str.c_str(), random(0, test.size()),
                                      random(0, str.size())));
}

template <class String>
void clause11_21_4_7_4_c(String& test, TestResult& testResult_,
                         const std::string& name_, const std::string& group_) {
  String str;
  randomString(&str, maxString);
  Num2String(test, test.find_first_of(str.c_str(), random(0, test.size())));
}

template <class String>
void clause11_21_4_7_4_d(String& test, TestResult& testResult_,
                         const std::string& name_, const std::string& group_) {
  Num2String(test,
             test.find_first_of(random('a', 'z'), random(0, test.size())));
}

template <class String>
void clause11_21_4_7_5_a(String& test, TestResult& testResult_,
                         const std::string& name_, const std::string& group_) {
  String str;
  randomString(&str, maxString);
  Num2String(test, test.find_last_of(str, random(0, test.size())));
}

template <class String>
void clause11_21_4_7_5_b(String& test, TestResult& testResult_,
                         const std::string& name_, const std::string& group_) {
  String str;
  randomString(&str, maxString);
  Num2String(test, test.find_last_of(str.c_str(), random(0, test.size()),
                                     random(0, str.size())));
}

template <class String>
void clause11_21_4_7_5_c(String& test, TestResult& testResult_,
                         const std::string& name_, const std::string& group_) {
  String str;
  randomString(&str, maxString);
  Num2String(test, test.find_last_of(str.c_str(), random(0, test.size())));
}

template <class String>
void clause11_21_4_7_5_d(String& test, TestResult& testResult_,
                         const std::string& name_, const std::string& group_) {
  Num2String(test, test.find_last_of(random('a', 'z'), random(0, test.size())));
}

template <class String>
void clause11_21_4_7_6_a(String& test, TestResult& testResult_,
                         const std::string& name_, const std::string& group_) {
  String str;
  randomString(&str, maxString);
  Num2String(test, test.find_first_not_of(str, random(0, test.size())));
}

template <class String>
void clause11_21_4_7_6_b(String& test, TestResult& testResult_,
                         const std::string& name_, const std::string& group_) {
  String str;
  randomString(&str, maxString);
  Num2String(test, test.find_first_not_of(str.c_str(), random(0, test.size()),
                                          random(0, str.size())));
}

template <class String>
void clause11_21_4_7_6_c(String& test, TestResult& testResult_,
                         const std::string& name_, const std::string& group_) {
  String str;
  randomString(&str, maxString);
  Num2String(test, test.find_first_not_of(str.c_str(), random(0, test.size())));
}

template <class String>
void clause11_21_4_7_6_d(String& test, TestResult& testResult_,
                         const std::string& name_, const std::string& group_) {
  Num2String(test,
             test.find_first_not_of(random('a', 'z'), random(0, test.size())));
}

template <class String>
void clause11_21_4_7_7_a(String& test, TestResult& testResult_,
                         const std::string& name_, const std::string& group_) {
  String str;
  randomString(&str, maxString);
  Num2String(test, test.find_last_not_of(str, random(0, test.size())));
}

template <class String>
void clause11_21_4_7_7_b(String& test, TestResult& testResult_,
                         const std::string& name_, const std::string& group_) {
  String str;
  randomString(&str, maxString);
  Num2String(test, test.find_last_not_of(str.c_str(), random(0, test.size()),
                                         random(0, str.size())));
}

template <class String>
void clause11_21_4_7_7_c(String& test, TestResult& testResult_,
                         const std::string& name_, const std::string& group_) {
  String str;
  randomString(&str, maxString);
  Num2String(test, test.find_last_not_of(str.c_str(), random(0, test.size())));
}

template <class String>
void clause11_21_4_7_7_d(String& test, TestResult& testResult_,
                         const std::string& name_, const std::string& group_) {
  Num2String(test,
             test.find_last_not_of(random('a', 'z'), random(0, test.size())));
}

template <class String>
void clause11_21_4_7_8(String& test, TestResult& testResult_,
                       const std::string& name_, const std::string& group_) {
  test = test.substr(random(0, test.size()), random(0, test.size()));
}

template <class String>
void clause11_21_4_7_9_a(String& test, TestResult& testResult_,
                         const std::string& name_, const std::string& group_) {
  String s;
  randomString(&s, maxString);
  int tristate = test.compare(s);
  if (tristate > 0) {
    tristate = 1;
  } else if (tristate < 0) {
    tristate = 2;
  }
  Num2String(test, tristate);
}

template <class String>
void clause11_21_4_7_9_b(String& test, TestResult& testResult_,
                         const std::string& name_, const std::string& group_) {
  String s;
  randomString(&s, maxString);
  int tristate =
      test.compare(random(0, test.size()), random(0, test.size()), s);
  if (tristate > 0) {
    tristate = 1;
  } else if (tristate < 0) {
    tristate = 2;
  }
  Num2String(test, tristate);
}

template <class String>
void clause11_21_4_7_9_c(String& test, TestResult& testResult_,
                         const std::string& name_, const std::string& group_) {
  String str;
  randomString(&str, maxString);
  int tristate =
      test.compare(random(0, test.size()), random(0, test.size()), str,
                   random(0, str.size()), random(0, str.size()));
  if (tristate > 0) {
    tristate = 1;
  } else if (tristate < 0) {
    tristate = 2;
  }
  Num2String(test, tristate);
}

template <class String>
void clause11_21_4_7_9_d(String& test, TestResult& testResult_,
                         const std::string& name_, const std::string& group_) {
  String s;
  randomString(&s, maxString);
  int tristate = test.compare(s.c_str());
  if (tristate > 0) {
    tristate = 1;
  } else if (tristate < 0) {
    tristate = 2;
  }
  Num2String(test, tristate);
}

template <class String>
void clause11_21_4_7_9_e(String& test, TestResult& testResult_,
                         const std::string& name_, const std::string& group_) {
  String str;
  randomString(&str, maxString);
  int tristate = test.compare(random(0, test.size()), random(0, test.size()),
                              str.c_str(), random(0, str.size()));
  if (tristate > 0) {
    tristate = 1;
  } else if (tristate < 0) {
    tristate = 2;
  }
  Num2String(test, tristate);
}

template <class String>
void clause11_21_4_8_1_a(String& test, TestResult& testResult_,
                         const std::string& name_, const std::string& group_) {
  String s1;
  randomString(&s1, maxString);
  String s2;
  randomString(&s2, maxString);
  test = s1 + s2;
}

template <class String>
void clause11_21_4_8_1_b(String& test, TestResult& testResult_,
                         const std::string& name_, const std::string& group_) {
  String s1;
  randomString(&s1, maxString);
  String s2;
  randomString(&s2, maxString);
  test = move(s1) + s2;
}

template <class String>
void clause11_21_4_8_1_c(String& test, TestResult& testResult_,
                         const std::string& name_, const std::string& group_) {
  String s1;
  randomString(&s1, maxString);
  String s2;
  randomString(&s2, maxString);
  test = s1 + move(s2);
}

template <class String>
void clause11_21_4_8_1_d(String& test, TestResult& testResult_,
                         const std::string& name_, const std::string& group_) {
  String s1;
  randomString(&s1, maxString);
  String s2;
  randomString(&s2, maxString);
  test = move(s1) + move(s2);
}

template <class String>
void clause11_21_4_8_1_e(String& test, TestResult& testResult_,
                         const std::string& name_, const std::string& group_) {
  String s;
  randomString(&s, maxString);
  String s1;
  randomString(&s1, maxString);
  test = s.c_str() + s1;
}

template <class String>
void clause11_21_4_8_1_f(String& test, TestResult& testResult_,
                         const std::string& name_, const std::string& group_) {
  String s;
  randomString(&s, maxString);
  String s1;
  randomString(&s1, maxString);
  test = s.c_str() + move(s1);
}

template <class String>
void clause11_21_4_8_1_g(String& test, TestResult& testResult_,
                         const std::string& name_, const std::string& group_) {
  String s;
  randomString(&s, maxString);
  test = typename String::value_type(random('a', 'z')) + s;
}

template <class String>
void clause11_21_4_8_1_h(String& test, TestResult& testResult_,
                         const std::string& name_, const std::string& group_) {
  String s;
  randomString(&s, maxString);
  test = typename String::value_type(random('a', 'z')) + move(s);
}

template <class String>
void clause11_21_4_8_1_i(String& test, TestResult& testResult_,
                         const std::string& name_, const std::string& group_) {
  String s;
  randomString(&s, maxString);
  String s1;
  randomString(&s1, maxString);
  test = s + s1.c_str();
}

template <class String>
void clause11_21_4_8_1_j(String& test, TestResult& testResult_,
                         const std::string& name_, const std::string& group_) {
  String s;
  randomString(&s, maxString);
  String s1;
  randomString(&s1, maxString);
  test = move(s) + s1.c_str();
}

template <class String>
void clause11_21_4_8_1_k(String& test, TestResult& testResult_,
                         const std::string& name_, const std::string& group_) {
  String s;
  randomString(&s, maxString);
  test = s + typename String::value_type(random('a', 'z'));
}

template <class String>
void clause11_21_4_8_1_l(String& test, TestResult& testResult_,
                         const std::string& name_, const std::string& group_) {
  String s;
  randomString(&s, maxString);
  String s1;
  randomString(&s1, maxString);
  test = move(s) + s1.c_str();
}

// Numbering here is from C++11
template <class String>
void clause11_21_4_8_9_a(String& test, TestResult& testResult_,
                         const std::string& name_, const std::string& group_) {
  std::basic_stringstream<typename String::value_type> stst(test.c_str());
  String str;
  while (stst) {
    stst >> str;
    test += str + test;
  }
}

TEST_CASE(string, test_all_clauses) {
  // EXPECT_TRUE(1) << "Starting with seed: " << seed;
  std::string r;
  string c;
  int count = 0;

  auto l = [&](const char* const clause,
               void (*f_string)(std::string&, TestResult&, const std::string&,
                                const std::string&),
               void (*f_pstring)(string&, TestResult&, const std::string&,
                                 const std::string&),
               void (*f_wpstring)(basic_string<wchar_t>&, TestResult&,
                                  const std::string&, const std::string&)) {
    do {
      randomString(&r);
      c = r.data();
      ASSERT_EQ(c, r.data(), "c", c.c_str(), "r", r.c_str());
      auto localSeed = seed + count;
      rng = RandomT(localSeed);
      // println("test.size", r.size(), "disc", r.end() - r.begin(), "r",
      //      r.c_str());
      f_string(r, testResult_, name_, group_);
      rng = RandomT(localSeed);
      f_pstring(c, testResult_, name_, group_);

      ASSERT_EQ(r.data(), c, "Lengths: ", r.size(), " vs. ", c.size(),
                "\nReference: '", r, "'", "\nActual:    '", c.c_str(), "'",
                "clause", clause);
    } while (++count % 100 != 0);
  };

#define TEST_CLAUSE(x)                                   \
  l(#x, clause11_##x<std::string>, clause11_##x<string>, \
    clause11_##x<basic_string<wchar_t>>);

  TEST_CLAUSE(21_4_2_a);
  TEST_CLAUSE(21_4_2_b);
  TEST_CLAUSE(21_4_2_c);
  TEST_CLAUSE(21_4_2_d);
  TEST_CLAUSE(21_4_2_e);
  TEST_CLAUSE(21_4_2_f);
  TEST_CLAUSE(21_4_2_g);
  TEST_CLAUSE(21_4_2_h);
  TEST_CLAUSE(21_4_2_i);
  TEST_CLAUSE(21_4_2_j);
  TEST_CLAUSE(21_4_2_k);
  TEST_CLAUSE(21_4_2_l);
  TEST_CLAUSE(21_4_2_lprime);
  TEST_CLAUSE(21_4_2_m);
  TEST_CLAUSE(21_4_2_n);
  TEST_CLAUSE(21_4_3);
  TEST_CLAUSE(21_4_4);
  TEST_CLAUSE(21_4_5);
  TEST_CLAUSE(21_4_6_1);
  TEST_CLAUSE(21_4_6_2);
  TEST_CLAUSE(21_4_6_3_a);
  TEST_CLAUSE(21_4_6_3_b);
  TEST_CLAUSE(21_4_6_3_c);
  TEST_CLAUSE(21_4_6_3_d);
  TEST_CLAUSE(21_4_6_3_e);
  TEST_CLAUSE(21_4_6_3_f);
  TEST_CLAUSE(21_4_6_3_g);
  TEST_CLAUSE(21_4_6_3_h);
  TEST_CLAUSE(21_4_6_3_i);
  TEST_CLAUSE(21_4_6_3_j);
  TEST_CLAUSE(21_4_6_3_k);
  TEST_CLAUSE(21_4_6_4);
  TEST_CLAUSE(21_4_6_5);
  TEST_CLAUSE(21_4_6_6);
  TEST_CLAUSE(21_4_6_7);
  TEST_CLAUSE(21_4_6_8);
  TEST_CLAUSE(21_4_7_1);

  TEST_CLAUSE(21_4_7_2_a);
  TEST_CLAUSE(21_4_7_2_a1);
  TEST_CLAUSE(21_4_7_2_a2);
  TEST_CLAUSE(21_4_7_2_b);
  TEST_CLAUSE(21_4_7_2_b1);
  TEST_CLAUSE(21_4_7_2_b2);
  TEST_CLAUSE(21_4_7_2_c);
  TEST_CLAUSE(21_4_7_2_c1);
  TEST_CLAUSE(21_4_7_2_c2);
  TEST_CLAUSE(21_4_7_2_d);
  TEST_CLAUSE(21_4_7_3_a);
  TEST_CLAUSE(21_4_7_3_b);
  TEST_CLAUSE(21_4_7_3_c);
  TEST_CLAUSE(21_4_7_3_d);
  TEST_CLAUSE(21_4_7_4_a);
  TEST_CLAUSE(21_4_7_4_b);
  TEST_CLAUSE(21_4_7_4_c);
  TEST_CLAUSE(21_4_7_4_d);
  TEST_CLAUSE(21_4_7_5_a);
  TEST_CLAUSE(21_4_7_5_b);
  TEST_CLAUSE(21_4_7_5_c);
  TEST_CLAUSE(21_4_7_5_d);
  TEST_CLAUSE(21_4_7_6_a);
  TEST_CLAUSE(21_4_7_6_b);
  TEST_CLAUSE(21_4_7_6_c);
  TEST_CLAUSE(21_4_7_6_d);
  TEST_CLAUSE(21_4_7_7_a);
  TEST_CLAUSE(21_4_7_7_b);
  TEST_CLAUSE(21_4_7_7_c);
  TEST_CLAUSE(21_4_7_7_d);
  TEST_CLAUSE(21_4_7_8);
  TEST_CLAUSE(21_4_7_9_a);
  TEST_CLAUSE(21_4_7_9_b);
  TEST_CLAUSE(21_4_7_9_c);
  TEST_CLAUSE(21_4_7_9_d);
  TEST_CLAUSE(21_4_7_9_e);
  TEST_CLAUSE(21_4_8_1_a);
  TEST_CLAUSE(21_4_8_1_b);
  TEST_CLAUSE(21_4_8_1_c);
  TEST_CLAUSE(21_4_8_1_d);
  TEST_CLAUSE(21_4_8_1_e);
  TEST_CLAUSE(21_4_8_1_f);
  TEST_CLAUSE(21_4_8_1_g);
  TEST_CLAUSE(21_4_8_1_h);
  TEST_CLAUSE(21_4_8_1_i);
  TEST_CLAUSE(21_4_8_1_j);
  TEST_CLAUSE(21_4_8_1_k);
  TEST_CLAUSE(21_4_8_1_l);
  TEST_CLAUSE(21_4_8_9_a);
}

TEST_CASE(string, test_move_ctor) {
  auto size = random(100, 2000);
  string s(size, 'a');
  string test = std::move(s);
  ASSERT(s.empty());
  ASSERT_EQ(size, test.size());
}

TEST_CASE(string, test_move_assign) {
  auto size = random(100, 2000);
  string s(size, 'a');
  string test;
  test = std::move(s);
  ASSERT(s.empty());
  ASSERT_EQ(size, test.size());
}

TEST_CASE(string, test_move_operator_lhs) {
  auto size1 = random(100, 2000);
  auto size2 = random(100, 2000);
  string s1(size1, 'a');
  string s2(size2, 'b');
  string test;
  test = std::move(s1) + s2;
  ASSERT(s1.empty());
  ASSERT_EQ(size1 + size2, test.size());
}

TEST_CASE(string, test_move_operator_rhs) {
  auto size1 = random(100, 2000);
  auto size2 = random(100, 2000);
  string s1(size1, 'a');
  string s2(size2, 'b');
  string test;
  test = s1 + std::move(s2);
  ASSERT_EQ(size1 + size2, test.size());
}

TEST_CASE(string, test_find_with_npos) {
  string str("localhost:80");
  ASSERT_EQ(string::npos, str.find(":", string::npos));
}

TEST_CASE(string, test_front_back) {
  string str("hello");
  ASSERT_EQ(str.front(), 'h');
  ASSERT_EQ(str.back(), 'o');
  str.front() = 'H';
  ASSERT_EQ(str.front(), 'H');
  str.back() = 'O';
  ASSERT_EQ(str.back(), 'O');
  ASSERT_EQ(str, "HellO");
}

TEST_CASE(string, test_noexcept) {
  ASSERT(noexcept(string()));
  string x;
  ASSERT(!noexcept(string(x)));
  ASSERT(noexcept(string(std::move(x))));
  string y;
  ASSERT(!noexcept(x == y));
  ASSERT(noexcept(y = std::move(x)));
}

TEST_CASE(string, test_iomanip) {
  std::stringstream ss;
  string str("Hello");

  ss << std::setw(6) << str;
  ASSERT_EQ(ss.str(), " Hello");
  ss.str("");

  ss << std::left << std::setw(6) << str;
  ASSERT_EQ(ss.str(), "Hello ");
  ss.str("");

  ss << std::right << std::setw(6) << str;
  ASSERT_EQ(ss.str(), " Hello");
  ss.str("");

  ss << std::setw(4) << str;
  ASSERT_EQ(ss.str(), "Hello");
  ss.str("");

  ss << std::setfill('^') << std::setw(6) << str;
  ASSERT_EQ(ss.str(), "^Hello");
  ss.str("");
}

TEST_CASE(string, test_rvalue_iterators) {
  string s = "base";
  string r = "hello";
  r.replace(r.begin(), r.end(), std::make_move_iterator(s.begin()),
            std::make_move_iterator(s.end()));
  ASSERT_EQ("base", r);

  // The following test is probably not required by the standard.
  // i.e. this could be in the realm of undefined behavior.
  string b = "123abcXYZ";
  auto ait = b.begin() + 3;
  auto Xit = b.begin() + 6;
  b.replace(ait, b.end(), b.begin(), Xit);
  ASSERT_EQ("123123abc", b);  // if things go wrong, you'd get "123123123"
}

TEST_CASE(string, test_move_terminator) {
  // The source of a move must remain in a valid state
  string s(100, 'x');  // too big to be in-situ
  string k;
  k = std::move(s);

  ASSERT_EQ(0, s.size());
  ASSERT_EQ('\0', *s.c_str());
}

TEST_CASE(string, test_null_zero_construction) {
  char* p = nullptr;
  int n = 0;
  string s(p, n);
  ASSERT_EQ(s.size(), 0);
}

TEST_CASE(string, test_compare_to_string) {
  string a = "a";
  string aa = a;
  string b = "b";
  ASSERT(a == aa);
  ASSERT(!(a == b));
  ASSERT(a != b);
  ASSERT(a < b);
  ASSERT(b > a);
  ASSERT(a <= b);
  ASSERT(b >= a);
}

TEST_CASE(string, test_compare_to_cstring) {
  const char* a = "a";
  string aa = a;
  string b = "b";
  ASSERT(a == aa);
  ASSERT(aa == a);
  ASSERT(!(a == b));
  ASSERT(!(b == a));
  ASSERT(a < b);
  ASSERT(!(b < a));
  ASSERT(b > a);
  ASSERT(!(a > b));
  ASSERT(a <= b);
  ASSERT(!(b <= a));
  ASSERT(!(a >= b));
  ASSERT(b >= a);
}

UNITTEST_MAIN() {
  RUN_TEST(string, test_all_clauses);
  RUN_TEST(string, test_move_ctor);
  RUN_TEST(string, test_move_assign);
  RUN_TEST(string, test_move_operator_lhs);
  RUN_TEST(string, test_move_operator_rhs);
  RUN_TEST(string, test_find_with_npos);
  RUN_TEST(string, test_front_back);
  RUN_TEST(string, test_noexcept);
  RUN_TEST(string, test_iomanip);
  RUN_TEST(string, test_rvalue_iterators);
  RUN_TEST(string, test_null_zero_construction);
  RUN_TEST(string, test_compare_to_string);
}
