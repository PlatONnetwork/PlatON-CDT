
// old rlp
//#define OLD
#undef NDEBUG
#define TESTNET
#include "platon/RLP.h"

#include <stdio.h>
#include "platon/platon.hpp"
#include "unit_test.hpp"

using namespace platon;


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

template <typename T>
void print_rlp_code(const std::string& prefix, const T& data,
                    const bytesRef& result) {
  // std::string info;
  // char temp_info[100] = {};
  // for (auto i : result) {
  //   sprintf(temp_info, "%d, ", int(i));
  //   info += temp_info;
  // }
  // platon::println(prefix, data, info);
}

class WhtType {
 public:
  WhtType() {}
  WhtType(std::string name, uint16_t age, uint16_t weight)
      : m_name_(name), m_age_(age), m_weight_(weight) {}
  friend RLPStream& operator<<(RLPStream& rlp, const WhtType& one) {
    return rlp.appendList(3) << one.m_name_ << one.m_age_ << one.m_weight_;
  }

  friend RLPSize& operator<<(RLPSize& rlps, const WhtType& one) {
    return rlps << RLPSize::list_start() << one.m_name_ << one.m_age_
                << one.m_weight_ << RLPSize::list_end();
  }

  friend void fetch(RLP rlp, WhtType& one) {
    fetch(rlp[0], one.m_name_);
    fetch(rlp[1], one.m_age_);
    fetch(rlp[2], one.m_weight_);
  }
  friend bool operator==(const WhtType& lhs, const WhtType& rhs) {
    return lhs.m_age_ == rhs.m_age_ && lhs.m_name_ == rhs.m_name_ &&
           lhs.m_weight_ == rhs.m_weight_;
  }

 public:
  std::string m_name_;
  uint16_t m_age_;
  uint16_t m_weight_;
};

class WhtGroup {
 public:
  WhtGroup() {}
  WhtGroup(std::string info, uint16_t number, WhtType member)
      : m_info_(info), m_number_(number), m_member_(member) {}
  friend RLPStream& operator<<(RLPStream& rlp, const WhtGroup& group) {
    return rlp.appendList(3)
           << group.m_info_ << group.m_number_ << group.m_member_;
  }

  friend RLPSize& operator<<(RLPSize& rlps, const WhtGroup& group) {
    return rlps << RLPSize::list_start() << group.m_info_ << group.m_number_
                << group.m_member_ << RLPSize::list_end();
  }

  friend void fetch(RLP rlp, WhtGroup& group) {
    fetch(rlp[0], group.m_info_);
    fetch(rlp[1], group.m_number_);
    fetch(rlp[2], group.m_member_);
  }
  friend bool operator==(const WhtGroup& lhs, const WhtGroup& rhs) {
    return lhs.m_info_ == rhs.m_info_ && lhs.m_number_ == rhs.m_number_ &&
           lhs.m_member_ == rhs.m_member_;
  }

 public:
  std::string m_info_;
  uint16_t m_number_;
  WhtType m_member_;
};

class Parent {
 public:
  Parent() {}
  Parent(std::string info, uint16_t number, WhtType member)
      : m_info_(info), m_number_(number), m_member_(member) {}
  PLATON_SERIALIZE(Parent, (m_info_)(m_number_)(m_member_))

  friend bool operator==(const Parent& lhs, const Parent& rhs) {
    return lhs.m_info_ == rhs.m_info_ && lhs.m_number_ == rhs.m_number_ &&
           lhs.m_member_ == rhs.m_member_;
  }

 public:
  std::string m_info_;
  uint16_t m_number_;
  WhtType m_member_;
};

class Derived : public Parent {
 public:
  Derived() {}
  Derived(std::string info, uint16_t number, WhtType member, std::string other)
      : Parent(info, number, member), m_other_(other) {}
  PLATON_SERIALIZE_DERIVED(Derived, Parent, (m_other_))
  friend bool operator==(const Derived& lhs, const Derived& rhs) {
    return lhs.m_other_ == rhs.m_other_ &&
           dynamic_cast<const Parent&>(lhs) == dynamic_cast<const Parent&>(rhs);
  }

 public:
  std::string m_other_;
};

TEST_CASE(rlp, int8_t) {
  const char* fn = "int8_t";
  int8_t int8_t_data = -2;
  size_t size = pack_size(int8_t_data);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  RLPStream stream;
  stream << int8_t_data;
  bytesRef result = stream.out();
  bytes old_result = {3};
  ASSERT_EQ(bytes(result.begin(), result.end()), old_result);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  ASSERT_EQ(size, result.size(), size, result.size());
  print_rlp_code("int8_t", int(int8_t_data), result);
  int8_t_data = 0;
  fetch(RLP(result), int8_t_data);
  ASSERT_EQ(int8_t_data, -2, int8_t_data);
}

TEST_CASE(rlp, int8_t_reserve) {
  const char* fn = "int8_t_reserve";
  int8_t int8_t_data = -2;
  size_t size = pack_size(int8_t_data);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  RLPStream stream;
  stream.reserve(size);
  stream << int8_t_data;
  bytesRef result = stream.out();
  bytes old_result = {3};
  ASSERT_EQ(bytes(result.begin(), result.end()), old_result);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  ASSERT_EQ(size, result.size());
  print_rlp_code("int8_t", int(int8_t_data), result);
  int8_t_data = 0;
  fetch(RLP(result), int8_t_data);
  ASSERT_EQ(int8_t_data, -2, int8_t_data);
}

TEST_CASE(rlp, int16_t) {
  const char* fn = "int16_t";
  int16_t int16_t_data = -3;
  size_t size = pack_size(int16_t_data);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  RLPStream stream;
  stream << int16_t_data;
  bytesRef result = stream.out();
  bytes old_result = {5};
  ASSERT_EQ(bytes(result.begin(), result.end()), old_result);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  ASSERT_EQ(size, result.size());
  print_rlp_code("int16_t", int16_t_data, result);
  int16_t_data = 0;
  fetch(RLP(result), int16_t_data);
  ASSERT_EQ(int16_t_data, -3, int16_t_data);
}

TEST_CASE(rlp, int16_t_reserve) {
  const char* fn = "int16_t_reserve";
  int16_t int16_t_data = -3;
  size_t size = pack_size(int16_t_data);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  RLPStream stream;
  stream.reserve(size);
  stream << int16_t_data;
  bytesRef result = stream.out();
  bytes old_result = {5};
  ASSERT_EQ(bytes(result.begin(), result.end()), old_result);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  ASSERT_EQ(size, result.size());
  print_rlp_code("int16_t", int16_t_data, result);
  int16_t_data = 0;
  fetch(RLP(result), int16_t_data);
  ASSERT_EQ(int16_t_data, -3, int16_t_data);
}

TEST_CASE(rlp, int) {
  const char* fn = "int";
  int int_data = -4;
  size_t size = pack_size(int_data);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  RLPStream stream;
  stream << int_data;
  bytesRef result = stream.out();
  bytes old_result = {7};
  ASSERT_EQ(bytes(result.begin(), result.end()), old_result);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  ASSERT_EQ(size, result.size());
  print_rlp_code("int", int_data, result);
  int_data = 0;
  fetch(RLP(result), int_data);
  ASSERT_EQ(int_data, -4, int_data);
}

TEST_CASE(rlp, int_reserve) {
  const char* fn = "int_reserve";
  int int_data = -4;
  size_t size = pack_size(int_data);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  RLPStream stream;
  stream.reserve(size);
  stream << int_data;
  bytesRef result = stream.out();
  bytes old_result = {7};
  ASSERT_EQ(bytes(result.begin(), result.end()), old_result);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  ASSERT_EQ(size, result.size());
  print_rlp_code("int", int_data, result);
  int_data = 0;
  fetch(RLP(result), int_data);
  ASSERT_EQ(int_data, -4, int_data);
}

TEST_CASE(rlp, int32_t) {
  const char* fn = "int32_t";
  int32_t int32_t_data = -5;
  size_t size = pack_size(int32_t_data);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  RLPStream stream;
  stream << int32_t_data;
  bytesRef result = stream.out();
  bytes old_result = {9};
  ASSERT_EQ(bytes(result.begin(), result.end()), old_result);
  platon_debug_gas(__LINE__, __func__, strlen(__func__));
  ASSERT_EQ(size, result.size());
  print_rlp_code("int32_t", int32_t_data, result);
  int32_t_data = 0;
  fetch(RLP(result), int32_t_data);
  ASSERT_EQ(int32_t_data, -5, int32_t_data);
}

TEST_CASE(rlp, int32_t_reserve) {
  const char* fn = "int32_t_reserve";
  int32_t int32_t_data = -5;
  size_t size = pack_size(int32_t_data);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  RLPStream stream;
  stream.reserve(size);
  stream << int32_t_data;
  bytesRef result = stream.out();
  bytes old_result = {9};
  ASSERT_EQ(bytes(result.begin(), result.end()), old_result);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  ASSERT_EQ(size, result.size());
  print_rlp_code("int32_t", int32_t_data, result);
  int32_t_data = 0;
  fetch(RLP(result), int32_t_data);
  ASSERT_EQ(int32_t_data, -5, int32_t_data);
}

TEST_CASE(rlp, int64_t) {
  const char* fn = "int64_t";
  int64_t int64_t_data = 0x7fffffffffffffff;
  size_t size = pack_size(int64_t_data);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  RLPStream stream;
  stream << int64_t_data;
  bytesRef result = stream.out();
  bytes old_result = {136, 255, 255, 255, 255, 255, 255, 255, 254};
  ASSERT_EQ(bytes(result.begin(), result.end()), old_result);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  ASSERT_EQ(size, result.size());
  print_rlp_code("int64_t", int64_t_data, result);
  int64_t_data = 0;
  fetch(RLP(result), int64_t_data);
  platon::println(int64_t_data);
  ASSERT_EQ(int64_t_data, 9223372036854775807, int64_t_data);
}

TEST_CASE(rlp, int64_t_reserve) {
  const char* fn = "int64_t_reserve";
  int64_t int64_t_data = 0x7fffffffffffffff;
  size_t size = pack_size(int64_t_data);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  RLPStream stream;
  stream.reserve(size);
  stream << int64_t_data;
  bytesRef result = stream.out();
  bytes old_result = {136, 255, 255, 255, 255, 255, 255, 255, 254};
  ASSERT_EQ(bytes(result.begin(), result.end()), old_result);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  ASSERT_EQ(size, result.size());
  print_rlp_code("int64_t", int64_t_data, result);
  int64_t_data = 0;
  fetch(RLP(result), int64_t_data);
  platon::println(int64_t_data);
  ASSERT_EQ(int64_t_data, 9223372036854775807, int64_t_data);
}

TEST_CASE(rlp, int128_t) {
  const char* fn = "int128_t";
  int128_t int128_t_data = 0x7fffffffffffffff;
  size_t size = pack_size(int128_t_data);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  RLPStream stream;
  stream << int128_t_data;
  bytesRef result = stream.out();
  bytes old_result = {136, 255, 255, 255, 255, 255, 255, 255, 254};
  ASSERT_EQ(bytes(result.begin(), result.end()), old_result);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  ASSERT_EQ(size, result.size());
  print_rlp_code("int128_t", int128_t_data, result);
  int128_t_data = 0;
  fetch(RLP(result), int128_t_data);
  platon::println(int128_t_data);
  ASSERT_EQ(int128_t_data, 9223372036854775807, int128_t_data);
}

TEST_CASE(rlp, int128_t_reserve) {
  const char* fn = "int128_t_reserve";
  int128_t int128_t_data = 0x7fffffffffffffff;
  size_t size = pack_size(int128_t_data);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  RLPStream stream;
  stream.reserve(size);
  stream << int128_t_data;
  bytesRef result = stream.out();
  bytes old_result = {136, 255, 255, 255, 255, 255, 255, 255, 254};
  ASSERT_EQ(bytes(result.begin(), result.end()), old_result);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  ASSERT_EQ(size, result.size());
  print_rlp_code("int128_t", int128_t_data, result);
  int128_t_data = 0;
  fetch(RLP(result), int128_t_data);
  platon::println(int128_t_data);
  ASSERT_EQ(int128_t_data, 9223372036854775807, int128_t_data);
}

TEST_CASE(rlp, bool) {
  const char* fn = "bool";
  bool bool_data = true;
  size_t size = pack_size(bool_data);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  RLPStream stream;
  stream << bool_data;
  bytesRef result = stream.out();
  bytes old_result = {1};
  ASSERT_EQ(bytes(result.begin(), result.end()), old_result);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  ASSERT_EQ(size, result.size());
  print_rlp_code("bool", bool_data, result);
  bool_data = false;
  fetch(RLP(result), bool_data);
  ASSERT_EQ(bool_data, true, bool_data);
}

TEST_CASE(rlp, bool_reserve) {
  const char* fn = "bool_reserve";
  bool bool_data = true;
  size_t size = pack_size(bool_data);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  RLPStream stream;
  stream.reserve(size);
  stream << bool_data;
  bytesRef result = stream.out();
  bytes old_result = {1};
  ASSERT_EQ(bytes(result.begin(), result.end()), old_result);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  ASSERT_EQ(size, result.size());
  print_rlp_code("bool", bool_data, result);
  bool_data = false;
  fetch(RLP(result), bool_data);
  ASSERT_EQ(bool_data, true, bool_data);
}

TEST_CASE(rlp, uint8_t) {
  const char* fn = "uint8_t";
  uint8_t uint8_t_data = 1;
  auto size = pack_size(uint8_t_data);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  RLPStream stream;
  stream << uint8_t_data;
  bytesRef result = stream.out();
  bytes old_result = {1};
  ASSERT_EQ(bytes(result.begin(), result.end()), old_result);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  ASSERT_EQ(size, result.size());
  print_rlp_code("uint8_t", uint8_t_data, result);
  uint8_t_data = 0;
  fetch(RLP(result), uint8_t_data);
  ASSERT_EQ(uint8_t_data, 1, uint8_t_data);
}

TEST_CASE(rlp, uint8_t_reserve) {
  const char* fn = "uint8_t_reserve";
  uint8_t uint8_t_data = 1;
  auto size = pack_size(uint8_t_data);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  RLPStream stream;
  stream.reserve(size);
  stream << uint8_t_data;
  bytesRef result = stream.out();
  bytes old_result = {1};
  ASSERT_EQ(bytes(result.begin(), result.end()), old_result);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  ASSERT_EQ(size, result.size());
  print_rlp_code("uint8_t", uint8_t_data, result);
  uint8_t_data = 0;
  fetch(RLP(result), uint8_t_data);
  ASSERT_EQ(uint8_t_data, 1, uint8_t_data);
}

TEST_CASE(rlp, uint16_t) {
  const char* fn = "uint16_t";
  uint16_t uint16_t_data = 2;
  auto size = pack_size(uint16_t_data);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  RLPStream stream;
  stream << uint16_t_data;
  bytesRef result = stream.out();
  bytes old_result = {2};
  ASSERT_EQ(bytes(result.begin(), result.end()), old_result);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  ASSERT_EQ(size, result.size());
  print_rlp_code("uint16_t", uint16_t_data, result);
  uint16_t_data = 0;
  fetch(RLP(result), uint16_t_data);
  ASSERT_EQ(uint16_t_data, 2, uint16_t_data);
}

TEST_CASE(rlp, uint16_t_reserve) {
  const char* fn = "uint16_t_reserve";
  uint16_t uint16_t_data = 2;
  auto size = pack_size(uint16_t_data);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  RLPStream stream;
  stream.reserve(size);
  stream << uint16_t_data;
  bytesRef result = stream.out();
  bytes old_result = {2};
  ASSERT_EQ(bytes(result.begin(), result.end()), old_result);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  ASSERT_EQ(size, result.size());
  print_rlp_code("uint16_t", uint16_t_data, result);
  uint16_t_data = 0;
  fetch(RLP(result), uint16_t_data);
  ASSERT_EQ(uint16_t_data, 2, uint16_t_data);
}

TEST_CASE(rlp, uint32_t) {
  const char* fn = "uint32_t";
  uint32_t uint32_t_data = 3;
  auto size = pack_size(uint32_t_data);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  RLPStream stream;
  stream << uint32_t_data;
  bytesRef result = stream.out();
  bytes old_result = {3};
  ASSERT_EQ(bytes(result.begin(), result.end()), old_result);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  ASSERT_EQ(size, result.size());
  print_rlp_code("uint32_t", uint32_t_data, result);
  uint32_t_data = 0;
  fetch(RLP(result), uint32_t_data);
  ASSERT_EQ(uint32_t_data, 3, uint32_t_data);
}

TEST_CASE(rlp, uint32_t_reserve) {
  const char* fn = "uint32_t_reserve";
  uint32_t uint32_t_data = 3;
  auto size = pack_size(uint32_t_data);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  RLPStream stream;
  stream.reserve(size);
  stream << uint32_t_data;
  bytesRef result = stream.out();
  bytes old_result = {3};
  ASSERT_EQ(bytes(result.begin(), result.end()), old_result);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  ASSERT_EQ(size, result.size());
  print_rlp_code("uint32_t", uint32_t_data, result);
  uint32_t_data = 0;
  fetch(RLP(result), uint32_t_data);
  ASSERT_EQ(uint32_t_data, 3, uint32_t_data);
}

TEST_CASE(rlp, uint64_t) {
  const char* fn = "uint64_t";
  uint64_t uint64_t_data = 4;
  auto size = pack_size(uint64_t_data);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  RLPStream stream;
  stream << uint64_t_data;
  bytesRef result = stream.out();
  bytes old_result = {4};
  ASSERT_EQ(bytes(result.begin(), result.end()), old_result);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  ASSERT_EQ(size, result.size());
  print_rlp_code("uint64_t", uint64_t_data, result);
  uint64_t_data = 0;
  fetch(RLP(result), uint64_t_data);
  ASSERT_EQ(uint64_t_data, 4, uint64_t_data);
}

TEST_CASE(rlp, uint64_t_reserve) {
  const char* fn = "uint64_t_reserve";
  uint64_t uint64_t_data = 4;
  auto size = pack_size(uint64_t_data);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  RLPStream stream;
  stream.reserve(size);
  stream << uint64_t_data;
  bytesRef result = stream.out();
  bytes old_result = {4};
  ASSERT_EQ(bytes(result.begin(), result.end()), old_result);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  ASSERT_EQ(size, result.size());
  print_rlp_code("uint64_t", uint64_t_data, result);
  uint64_t_data = 0;
  fetch(RLP(result), uint64_t_data);
  ASSERT_EQ(uint64_t_data, 4, uint64_t_data);
}

// TEST_CASE(rlp, float) {
//   const char* fn = "float";
//   float float_data = 1.23;
//   auto size = pack_size(float_data);
//   platon_debug_gas(__LINE__, fn, strlen(fn));
//   RLPStream stream;
//   stream << float_data;
//   bytesRef result = stream.out();
//   platon_debug_gas(__LINE__, fn, strlen(fn));
//   ASSERT_EQ(size, result.size());
//   print_rlp_code("float", float_data, result);
//   float_data = 0;
//   fetch(RLP(result), float_data);
//   platon::println(float_data);
// }

// TEST_CASE(rlp, float_reserve) {
//   const char* fn = "float_reserve";
//   float float_data = 1.23;
//   auto size = pack_size(float_data);
//   platon_debug_gas(__LINE__, fn, strlen(fn));
//   RLPStream stream;
//   stream.reserve(size);
//   stream << float_data;
//   bytesRef result = stream.out();
//   platon_debug_gas(__LINE__, fn, strlen(fn));
//   ASSERT_EQ(size, result.size());
//   print_rlp_code("float", float_data, result);
//   float_data = 0;
//   fetch(RLP(result), float_data);
//   platon::println(float_data);
// }

// TEST_CASE(rlp, double) {
//   const char* fn = "double";
//   double double_data = 4.56;
//   auto size = pack_size(double_data);
//   platon_debug_gas(__LINE__, fn, strlen(fn));
//   RLPStream stream;
//   stream << double_data;
//   bytesRef result = stream.out();
//   platon_debug_gas(__LINE__, fn, strlen(fn));
//   ASSERT_EQ(size, result.size());
//   print_rlp_code("double", double_data, result);
//   double_data = 0;
//   fetch(RLP(result), double_data);
//   platon::println(double_data);
// }

// TEST_CASE(rlp, double_reserve) {
//   const char* fn = "double_reserve";
//   double double_data = 4.56;
//   auto size = pack_size(double_data);
//   platon_debug_gas(__LINE__, fn, strlen(fn));
//   RLPStream stream;
//   stream.reserve(size);
//   stream << double_data;
//   bytesRef result = stream.out();
//   platon_debug_gas(__LINE__, fn, strlen(fn));
//   ASSERT_EQ(size, result.size());
//   print_rlp_code("double", double_data, result);
//   double_data = 0;
//   fetch(RLP(result), double_data);
//   platon::println(double_data);
// }

TEST_CASE(rlp, u128) {
  const char* fn = "u128";
  u128 u128_data = 123456789012345678;
  platon_debug_gas(__LINE__, fn, strlen(fn));
  auto size = pack_size(u128_data);
  RLPStream stream;
  stream << u128_data;
  bytesRef result = stream.out();
  bytes old_result = {136, 1, 182, 155, 75, 166, 48, 243, 78};
  ASSERT_EQ(bytes(result.begin(), result.end()), old_result);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  ASSERT_EQ(size, result.size());
  print_rlp_code("u128", u128_data, result);
  u128 fetch_data = 0;
  fetch(RLP(result), fetch_data);
  ASSERT_EQ(u128_data, fetch_data);
}

TEST_CASE(rlp, u128_reserve) {
  const char* fn = "u128_reserve";
  u128 u128_data = 123456789012345678;
  platon_debug_gas(__LINE__, fn, strlen(fn));
  auto size = pack_size(u128_data);
  RLPStream stream;
  stream.reserve(size);
  stream << u128_data;
  bytesRef result = stream.out();
  bytes old_result = {136, 1, 182, 155, 75, 166, 48, 243, 78};
  ASSERT_EQ(bytes(result.begin(), result.end()), old_result);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  ASSERT_EQ(size, result.size());
  print_rlp_code("u128", u128_data, result);
  u128 fetch_data = 0;
  fetch(RLP(result), fetch_data);
  ASSERT_EQ(u128_data, fetch_data);
}

TEST_CASE(rlp, string) {
  const char* fn = "string";
  std::string string_data = "abc";
  auto size = pack_size(string_data);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  RLPStream stream;
  stream << string_data;
  bytesRef result = stream.out();
  bytes old_result = {131, 97, 98, 99};
  ASSERT_EQ(bytes(result.begin(), result.end()), old_result);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  ASSERT_EQ(size, result.size());
  print_rlp_code("string", string_data, result);
  string_data = "";
  fetch(RLP(result), string_data);
  ASSERT_EQ(string_data, std::string("abc"), string_data);
}

TEST_CASE(rlp, string_reserve) {
  const char* fn = "string_reserve";
  std::string string_data = "abc";
  auto size = pack_size(string_data);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  RLPStream stream;
  stream.reserve(size);
  stream << string_data;
  bytesRef result = stream.out();
  bytes old_result = {131, 97, 98, 99};
  ASSERT_EQ(bytes(result.begin(), result.end()), old_result);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  ASSERT_EQ(size, result.size());
  print_rlp_code("string", string_data, result);
  string_data = "";
  fetch(RLP(result), string_data);
  ASSERT_EQ(string_data, std::string("abc"), string_data);
}

TEST_CASE(rlp, address) {
  const char* fn = "address";
  auto address_info = make_address("lax10jc0t4ndqarj4q6ujl3g3ycmufgc77epxg02lt");
  auto size = pack_size(address_info.first);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  RLPStream stream;
  stream << address_info.first;
  bytesRef result = stream.out();
  bytes old_result = {148, 124, 176, 245, 214, 109, 7, 71, 42, 131, 92, 151, 226, 136, 147, 27, 226, 81, 143, 123, 33};
  ASSERT_EQ(bytes(result.begin(), result.end()), old_result);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  ASSERT_EQ(size, result.size());
  print_rlp_code("address", address_info.first.toString(), result);
  Address addr1;
  fetch(RLP(result), addr1);
  ASSERT_EQ(address_info.first, addr1, address_info.first.toString(), addr1.toString());
}

TEST_CASE(rlp, address_reserve) {
  const char* fn = "address_reserve";
  auto address_info = make_address("lax10jc0t4ndqarj4q6ujl3g3ycmufgc77epxg02lt");
  auto size = pack_size(address_info.first);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  RLPStream stream;
  stream.reserve(size);
  stream << address_info.first;
  bytesRef result = stream.out();
  bytes old_result = {148, 124, 176, 245, 214, 109, 7, 71, 42, 131, 92, 151, 226, 136, 147, 27, 226, 81, 143, 123, 33};
  ASSERT_EQ(bytes(result.begin(), result.end()), old_result);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  ASSERT_EQ(size, result.size());
  print_rlp_code("address", address_info.first.toString(), result);
  Address addr1;
  fetch(RLP(result), addr1);
  ASSERT_EQ(address_info.first, addr1, address_info.first.toString(), addr1.toString());
}

TEST_CASE(rlp, array) {
  const char* fn = "array";
  std::array<char, 10> arr = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j'};
  auto size = pack_size(arr);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  RLPStream stream;
  stream << arr;
  bytesRef result = stream.out();
  bytes old_result = {212, 129, 194, 129, 196, 129, 198, 129, 200, 129, 202, 129, 204, 129, 206, 129, 208, 129, 210, 129, 212};
  ASSERT_EQ(bytes(result.begin(), result.end()), old_result);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  print_rlp_code("array", "abcdefghij", result);
  std::array<char, 10> arr1;
  fetch(RLP(result), arr1);
  ASSERT_EQ(arr, arr1);
}

TEST_CASE(rlp, array_reserve) {
  const char* fn = "array_reserve";
  std::array<char, 10> arr = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j'};
  auto size = pack_size(arr);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  RLPStream stream;
  stream.reserve(size);
  stream << arr;
  bytesRef result = stream.out();
  bytes old_result = {212, 129, 194, 129, 196, 129, 198, 129, 200, 129, 202, 129, 204, 129, 206, 129, 208, 129, 210, 129, 212};
  ASSERT_EQ(bytes(result.begin(), result.end()), old_result);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  print_rlp_code("array", "abcdefghij", result);
  std::array<char, 10> arr1;
  fetch(RLP(result), arr1);
  ASSERT_EQ(arr, arr1);
}

TEST_CASE(rlp, list) {
  const char* fn = "list";
  std::list<std::string> list_data = {"abc", "def", "ghi"};
  auto size = pack_size(list_data);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  RLPStream stream;
  stream << list_data;
  bytesRef result = stream.out();
  bytes old_result = {204, 131, 97, 98, 99, 131, 100, 101, 102, 131, 103, 104, 105};
  ASSERT_EQ(bytes(result.begin(), result.end()), old_result);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  ASSERT_EQ(size, result.size());
  print_rlp_code("list", "list_data", result);
  std::list<std::string> fetch_data;
  fetch(RLP(result), fetch_data);
  ASSERT_EQ(fetch_data, list_data);
}

TEST_CASE(rlp, list_reserve) {
  const char* fn = "list_reserve";
  std::list<std::string> list_data = {"abc", "def", "ghi"};
  auto size = pack_size(list_data);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  RLPStream stream;
  stream.reserve(size);
  stream << list_data;
  bytesRef result = stream.out();
  bytes old_result = {204, 131, 97, 98, 99, 131, 100, 101, 102, 131, 103, 104, 105};
  ASSERT_EQ(bytes(result.begin(), result.end()), old_result);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  ASSERT_EQ(size, result.size());
  print_rlp_code("list", "list_data", result);
  std::list<std::string> fetch_data;
  fetch(RLP(result), fetch_data);
  ASSERT_EQ(fetch_data, list_data);
}

TEST_CASE(rlp, append) {
  std::vector<uint32_t> vect_result;
  RLPStream stream(4);
  uint32_t data_32 = 1 * 256 * 256 * 256 + 2 * 256 * 256 + 3 * 256 + 4;
  stream << data_32;
  vect_result.push_back(data_32);
  data_32 = 5 * 256 * 256 * 256 + 6 * 256 * 256 + 7 * 256 + 8;
  stream << data_32;
  vect_result.push_back(data_32);
  data_32 = 9 * 256 * 256 * 256 + 10 * 256 * 256 + 11 * 256 + 12;
  stream << data_32;
  vect_result.push_back(data_32);
  data_32 = 13 * 256 * 256 + 14 * 256 + 15;
  stream << data_32;
  vect_result.push_back(data_32);
  auto size = pack_size(vect_result);
  bytesRef result = stream.out();
  bytes old_result = {211, 132, 1, 2, 3, 4, 132, 5, 6, 7, 8, 132, 9, 10, 11, 12, 131, 13, 14, 15};
  ASSERT_EQ(bytes(result.begin(), result.end()), old_result);
  ASSERT_EQ(size, result.size());
  print_rlp_code("append", "test", result);
  std::vector<uint32_t> vect_fetch;
  fetch(RLP(result), vect_fetch);
  ASSERT_EQ(vect_result, vect_fetch);
}

TEST_CASE(rlp, map) {
  const char* fn = "map";
  std::map<std::string, std::string> m = {
      {"abc", "abc"}, {"abc1", "abc1"}, {"abc2", "abc2"}};
  platon_debug_gas(__LINE__, fn, strlen(fn));
  auto size = pack_size(m);
  RLPStream stream;
  stream << m;
  auto result = stream.out();
  bytes old_result = {223, 200, 131, 97, 98, 99, 131, 97, 98, 99, 202, 132, 97, 98, 99, 49, 132, 97, 98, 99, 49, 202, 132, 97, 98, 99, 50, 132, 97, 98, 99, 50};
  ASSERT_EQ(bytes(result.begin(), result.end()), old_result);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  ASSERT_EQ(size, result.size());
  print_rlp_code("map", "test", result);
  std::map<std::string, std::string> m1;
  fetch(RLP(result), m1);
  ASSERT_EQ(m.size(), m1.size());
  ASSERT_EQ(m["abc"], m1["abc"]);
  ASSERT_EQ(m["abc1"], m1["abc1"]);
  ASSERT_EQ(m["abc2"], m1["abc2"]);
}

TEST_CASE(rlp, map_reserve) {
  const char* fn = "map_reserve";
  std::map<std::string, std::string> m = {
      {"abc", "abc"}, {"abc1", "abc1"}, {"abc2", "abc2"}};
  platon_debug_gas(__LINE__, fn, strlen(fn));
  auto size = pack_size(m);
  RLPStream stream;
  stream.reserve(size);
  stream << m;
  auto result = stream.out();
  bytes old_result = {223, 200, 131, 97, 98, 99, 131, 97, 98, 99, 202, 132, 97, 98, 99, 49, 132, 97, 98, 99, 49, 202, 132, 97, 98, 99, 50, 132, 97, 98, 99, 50};
  ASSERT_EQ(bytes(result.begin(), result.end()), old_result);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  ASSERT_EQ(size, result.size());
  print_rlp_code("map_reserve", "test", result);
  std::map<std::string, std::string> m1;
  fetch(RLP(result), m1);
  ASSERT_EQ(m.size(), m1.size());
  ASSERT_EQ(m["abc"], m1["abc"]);
  ASSERT_EQ(m["abc1"], m1["abc1"]);
  ASSERT_EQ(m["abc2"], m1["abc2"]);
}

TEST_CASE(rlp, struct) {
  const char* fn = "struct";
  WhtType one("jatel", 30, 160);
  auto size = pack_size(one);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  RLPStream wht_stream;
  wht_stream << one;
  bytesRef result = wht_stream.out();
  bytes old_result = {201, 133, 106, 97, 116, 101, 108, 30, 129, 160};
  ASSERT_EQ(bytes(result.begin(), result.end()), old_result);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  ASSERT_EQ(size, result.size());
  print_rlp_code("struct", "test", result);
  WhtType two;
  fetch(RLP(result), two);
  ASSERT_EQ(two, one);
}

TEST_CASE(rlp, struct_reserve) {
  const char* fn = "struct_reserve";
  WhtType one("jatel", 30, 160);
  auto size = pack_size(one);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  RLPStream wht_stream;
  wht_stream.reserve(size);
  wht_stream << one;
  bytesRef result = wht_stream.out();
  bytes old_result = {201, 133, 106, 97, 116, 101, 108, 30, 129, 160};
  ASSERT_EQ(bytes(result.begin(), result.end()), old_result);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  ASSERT_EQ(size, result.size());
  print_rlp_code("struct_reserve", "test", result);
  WhtType two;
  fetch(RLP(result), two);
  ASSERT_EQ(two, one);
}

TEST_CASE(rlp, group) {
  const char* fn = "group";
  WhtType one("jatel", 30, 160);
  WhtGroup group("group", 3, one);
  auto size = pack_size(group);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  RLPStream wht_stream;
  wht_stream << group;
  bytesRef result = wht_stream.out();
  bytes old_result = {209, 133, 103, 114, 111, 117, 112, 3, 201, 133, 106, 97, 116, 101, 108, 30, 129, 160};
  ASSERT_EQ(bytes(result.begin(), result.end()), old_result);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  ASSERT_EQ(size, result.size(), size, result.size());
  print_rlp_code("group", "test", result);
  WhtGroup one_group;
  fetch(RLP(result), one_group);
  ASSERT_EQ(one_group, group);
}

TEST_CASE(rlp, group_reserve) {
  const char* fn = "group_reserve";
  WhtType one("jatel", 30, 160);
  WhtGroup group("group", 3, one);
  auto size = pack_size(group);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  RLPStream wht_stream;
  wht_stream.reserve(size);
  wht_stream << group;
  bytesRef result = wht_stream.out();
  bytes old_result = {209, 133, 103, 114, 111, 117, 112, 3, 201, 133, 106, 97, 116, 101, 108, 30, 129, 160};
  ASSERT_EQ(bytes(result.begin(), result.end()), old_result);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  ASSERT_EQ(size, result.size(), size, result.size());
  print_rlp_code("group_reserve", "test", result);
  WhtGroup one_group;
  fetch(RLP(result), one_group);
  ASSERT_EQ(one_group, group);
}

TEST_CASE(rlp, Derived) {
  const char* fn = "Derived";
  WhtType one("jatel", 30, 160);
  Derived test_derived("group", 3, one, "jatel_derived");
  platon_debug_gas(__LINE__, fn, strlen(fn));
  auto size = pack_size(test_derived);
  RLPStream wht_stream;
  wht_stream << test_derived;
  bytesRef result = wht_stream.out();
  bytes old_result = {224, 209, 133, 103, 114, 111, 117, 112, 3, 201, 133, 106, 97, 116, 101, 108, 30, 129, 160, 141, 106, 97, 116, 101, 108, 95, 100, 101, 114, 105, 118, 101, 100};
  ASSERT_EQ(bytes(result.begin(), result.end()), old_result);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  ASSERT_EQ(size, result.size());
  print_rlp_code("Derived", "test", result);
  Derived one_test_derived;
  fetch(RLP(result), one_test_derived);
  ASSERT_EQ(test_derived, one_test_derived);
}

TEST_CASE(rlp, Derived_reserve) {
  const char* fn = "Derived_reserve";
  WhtType one("jatel", 30, 160);
  Derived test_derived("group", 3, one, "jatel_derived");
  platon_debug_gas(__LINE__, fn, strlen(fn));
  auto size = pack_size(test_derived);
  RLPStream wht_stream;
  wht_stream.reserve(size);
  wht_stream << test_derived;
  bytesRef result = wht_stream.out();
  bytes old_result = {224, 209, 133, 103, 114, 111, 117, 112, 3, 201, 133, 106, 97, 116, 101, 108, 30, 129, 160, 141, 106, 97, 116, 101, 108, 95, 100, 101, 114, 105, 118, 101, 100};
  ASSERT_EQ(bytes(result.begin(), result.end()), old_result);
  platon_debug_gas(__LINE__, fn, strlen(fn));
  ASSERT_EQ(size, result.size());
  print_rlp_code("Derived_reserve", "test", result);
  Derived one_test_derived;
  fetch(RLP(result), one_test_derived);
  ASSERT_EQ(test_derived, one_test_derived);
}

TEST_CASE(rlp, bigint_limit){
  uint128_t left = 0;
  uint128_t right = 0xffffffffffffffffULL;
  right <<= 64;
  right += 0xffffffffffffffffULL;
  RLPStream stream;
  stream << left;
  bytesRef result = stream.out();
  bytes old_result = {128};
  ASSERT_EQ(bytes(result.begin(), result.end()), old_result);
  print_rlp_code("bigint_limit", "left", result);
  stream.clear();
  stream << right;
  result = stream.out();
  old_result = {144, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};
  ASSERT_EQ(bytes(result.begin(), result.end()), old_result);
  print_rlp_code("bigint_limit", "right", result);
}

TEST_CASE(rlp, bytes_limit){
  bytes left;
  bytes right = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    RLPStream stream;
  stream << left;
  bytesRef result = stream.out();
  bytes old_result = {128};
  ASSERT_EQ(bytes(result.begin(), result.end()), old_result);
  print_rlp_code("bytes_limit", "left", result);
  stream.clear();
  stream << right;
  result = stream.out();
  old_result = {137, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ASSERT_EQ(bytes(result.begin(), result.end()), old_result);
  print_rlp_code("bytes_limit", "right", result);
}

TEST_CASE(rlp, list_limit){
  bytes left;
  bytes right = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  RLPStream stream;
  stream.appendList(1);
  stream << left;
  bytesRef result = stream.out();
  bytes old_result = {193, 128};
  ASSERT_EQ(bytes(result.begin(), result.end()), old_result);
  print_rlp_code("list_limit", "left", result);
  stream.clear();
  stream.appendList(1);
  stream << right;
  result = stream.out();
  old_result = {202, 137, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ASSERT_EQ(bytes(result.begin(), result.end()), old_result);
  print_rlp_code("list_limit", "right", result);
}


UNITTEST_MAIN() {
  RUN_TEST(rlp, int8_t);
  RUN_TEST(rlp, int8_t_reserve);
  RUN_TEST(rlp, int16_t);
  RUN_TEST(rlp, int16_t_reserve);
  RUN_TEST(rlp, int);
  RUN_TEST(rlp, int_reserve);
  RUN_TEST(rlp, int32_t);
  RUN_TEST(rlp, int32_t_reserve);
  RUN_TEST(rlp, int64_t);
  RUN_TEST(rlp, int64_t_reserve);
  RUN_TEST(rlp, int128_t);
  RUN_TEST(rlp, int128_t_reserve);
  RUN_TEST(rlp, bool);
  RUN_TEST(rlp, bool_reserve);
  RUN_TEST(rlp, uint8_t);
  RUN_TEST(rlp, uint8_t_reserve);
  RUN_TEST(rlp, uint16_t);
  RUN_TEST(rlp, uint16_t_reserve);
  RUN_TEST(rlp, uint32_t);
  RUN_TEST(rlp, uint32_t_reserve);
  RUN_TEST(rlp, uint64_t);
  RUN_TEST(rlp, uint64_t_reserve);
  // RUN_TEST(rlp, float);
  // RUN_TEST(rlp, float_reserve);
  // RUN_TEST(rlp, double);
  // RUN_TEST(rlp, double_reserve);
  RUN_TEST(rlp, u128);
  RUN_TEST(rlp, u128_reserve);
  RUN_TEST(rlp, string);
  RUN_TEST(rlp, string_reserve);
  RUN_TEST(rlp, address);
  RUN_TEST(rlp, address_reserve);
  RUN_TEST(rlp, array);
  RUN_TEST(rlp, array_reserve);
  RUN_TEST(rlp, list);
  RUN_TEST(rlp, list_reserve);
  RUN_TEST(rlp, append);
  RUN_TEST(rlp, map);
  RUN_TEST(rlp, map_reserve);
  RUN_TEST(rlp, struct);
  RUN_TEST(rlp, struct_reserve);
  RUN_TEST(rlp, group);
  RUN_TEST(rlp, group_reserve);
  RUN_TEST(rlp, Derived);
  RUN_TEST(rlp, Derived_reserve);
  RUN_TEST(rlp, bigint_limit);
  RUN_TEST(rlp, bytes_limit);
  RUN_TEST(rlp, list_limit);
}
