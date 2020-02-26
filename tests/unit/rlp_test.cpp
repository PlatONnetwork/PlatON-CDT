#include "unit_test.hpp"
#include "platon/RLP.h"
#include "platon/rlp_extend.hpp"
#include "platon/rlp_serialize.hpp"
#include "platon/fixedhash.hpp"
#include "platon/print.hpp"
#include <stdio.h>

using namespace platon;

template<typename T>
void print_rlp_code(const std::string &prefix, const T &data, const std::vector<byte> &result){
  std::string info;
  char temp_info[100] = {};
  for(auto i : result){
    sprintf(temp_info, "%d ", int(i));
    info += temp_info;
  }
  platon::println(prefix, data, info);
}

class WhtType 
{
    public:
        WhtType(){}
        WhtType(std::string name, uint16_t age, uint16_t weight): m_name_(name), m_age_(age), m_weight_(weight) {}
        friend RLPStream& operator << ( RLPStream& rlp, const WhtType& one ){
            return rlp.appendList(3) << one.m_name_ << one.m_age_ << one.m_weight_;
        }
        friend void fetch(RLP rlp, WhtType& one){
            fetch(rlp[0], one.m_name_);
            fetch(rlp[1], one.m_age_);
            fetch(rlp[2], one.m_weight_);
        }
        friend bool operator == ( const WhtType& lhs, const WhtType& rhs ){
            return lhs.m_age_ == rhs.m_age_ && lhs.m_name_ == rhs.m_name_ && lhs.m_weight_ == rhs.m_weight_;
        }
    public:
	    std::string m_name_;
	    uint16_t m_age_;  
	    uint16_t m_weight_;
};

class WhtGroup
{
    public:
        WhtGroup(){}
        WhtGroup(std::string info, uint16_t number,WhtType member): m_info_(info), m_number_(number), m_member_(member) {}
        friend RLPStream& operator << ( RLPStream& rlp, const WhtGroup& group ){
            return rlp.appendList(3) << group.m_info_ << group.m_number_ << group.m_member_;
        }
        friend void fetch(RLP rlp, WhtGroup& group){
            fetch(rlp[0], group.m_info_);
            fetch(rlp[1], group.m_number_);
            fetch(rlp[2], group.m_member_);
        }
        friend bool operator == ( const WhtGroup& lhs, const WhtGroup& rhs ){
            return lhs.m_info_ == rhs.m_info_ && lhs.m_number_ == rhs.m_number_ 
            && lhs.m_member_ == rhs.m_member_;
        }
    public:
	    std::string m_info_;
	    uint16_t m_number_;
	    WhtType m_member_;
};

class Parent 
{   
    public:
        Parent(){}
        Parent(std::string info, uint16_t number, WhtType member): m_info_(info), m_number_(number), m_member_(member) {}
        PLATON_SERIALIZE(Parent, (m_info_)(m_number_)(m_member_))
        friend bool operator == ( const Parent& lhs, const Parent& rhs ){
            return lhs.m_info_ == rhs.m_info_ && lhs.m_number_ == rhs.m_number_ 
            && lhs.m_member_ == rhs.m_member_;
        }
    public:
	    std::string m_info_;
	    uint16_t m_number_;
	    WhtType m_member_;
};

class Derived : public Parent 
{
    public:
        Derived(){}
        Derived(std::string info, uint16_t number, WhtType member, std::string other): Parent(info, number, member), m_other_(other){}
        PLATON_SERIALIZE_DERIVED(Derived, Parent, (m_other_))
        friend bool operator == ( const Derived& lhs, const Derived& rhs ){
            return lhs.m_other_ == rhs.m_other_ && 
            dynamic_cast<const Parent &>(lhs) == dynamic_cast<const Parent&>(rhs);
        }
    public:
	    std::string m_other_;  
};

TEST_CASE(rlp, int8_t) {
  int8_t int8_t_data = -2; 
  RLPStream stream;
  stream << int8_t_data;
  std::vector<byte> result = stream.out();
  print_rlp_code("int8_t", int(int8_t_data), result);
  int8_t_data = 0;
  fetch(RLP(result), int8_t_data);
  ASSERT_EQ(int8_t_data, -2, int8_t_data);
}

TEST_CASE(rlp, int16_t) {
  int16_t int16_t_data = -3; 
  RLPStream stream;
  stream << int16_t_data;
  std::vector<byte> result = stream.out();
  print_rlp_code("int16_t", int16_t_data, result);
  int16_t_data = 0;
  fetch(RLP(result), int16_t_data);
  ASSERT_EQ(int16_t_data, -3, int16_t_data);
}

TEST_CASE(rlp, int) {
  int int_data = -4; 
  RLPStream stream;
  stream << int_data;
  std::vector<byte> result = stream.out();
  print_rlp_code("int", int_data, result);
  int_data = 0;
  fetch(RLP(result), int_data);
  ASSERT_EQ(int_data, -4, int_data);
}

TEST_CASE(rlp, int32_t) {
  int32_t int32_t_data = -5; 
  RLPStream stream;
  stream << int32_t_data;
  std::vector<byte> result = stream.out();
  print_rlp_code("int32_t", int32_t_data, result);
  int32_t_data = 0;
  fetch(RLP(result), int32_t_data);
  ASSERT_EQ(int32_t_data, -5, int32_t_data);
}

TEST_CASE(rlp, int64_t) {
  int64_t int64_t_data = 0x7fffffffffffffff; 
  RLPStream stream;
  stream << int64_t_data;
  std::vector<byte> result = stream.out();
  print_rlp_code("int64_t", int64_t_data, result);
  int64_t_data = 0;
  fetch(RLP(result), int64_t_data);
  platon::println(int64_t_data);
  ASSERT_EQ(int64_t_data, 9223372036854775807, int64_t_data);
}

TEST_CASE(rlp, bool) {
  bool bool_data = true; 
  RLPStream stream;
  stream << bool_data;
  std::vector<byte> result = stream.out();
  print_rlp_code("bool", bool_data, result);
  bool_data = false;
  fetch(RLP(result), bool_data);
  ASSERT_EQ(bool_data, true, bool_data);
}

TEST_CASE(rlp, uint8_t) {
  uint8_t uint8_t_data = 1; 
  RLPStream stream;
  stream << uint8_t_data;
  std::vector<byte> result = stream.out();
  print_rlp_code("uint8_t", uint8_t_data, result);
  uint8_t_data = 0;
  fetch(RLP(result), uint8_t_data);
  ASSERT_EQ(uint8_t_data, 1, uint8_t_data);
}

TEST_CASE(rlp, uint16_t) {
  uint16_t uint16_t_data = 2; 
  RLPStream stream;
  stream << uint16_t_data;
  std::vector<byte> result = stream.out();
  print_rlp_code("uint16_t", uint16_t_data, result);
  uint16_t_data = 0;
  fetch(RLP(result), uint16_t_data);
  ASSERT_EQ(uint16_t_data, 2, uint16_t_data);
}

TEST_CASE(rlp, uint32_t) {
  uint32_t uint32_t_data = 3; 
  RLPStream stream;
  stream << uint32_t_data;
  std::vector<byte> result = stream.out();
  print_rlp_code("uint32_t", uint32_t_data, result);
  uint32_t_data = 0;
  fetch(RLP(result), uint32_t_data);
  ASSERT_EQ(uint32_t_data, 3, uint32_t_data);
}

TEST_CASE(rlp, uint64_t) {
  uint64_t uint64_t_data = 4; 
  RLPStream stream;
  stream << uint64_t_data;
  std::vector<byte> result = stream.out();
  print_rlp_code("uint64_t", uint64_t_data, result);
  uint64_t_data = 0;
  fetch(RLP(result), uint64_t_data);
  ASSERT_EQ(uint64_t_data, 4, uint64_t_data);
}

TEST_CASE(rlp, float) {
  float float_data = 1.23; 
  RLPStream stream;
  stream << float_data;
  std::vector<byte> result = stream.out();
  print_rlp_code("float", float_data, result);
  float_data = 0;
  fetch(RLP(result), float_data);
  platon::println(float_data);
}


TEST_CASE(rlp, double) {
  double double_data = 4.56; 
  RLPStream stream;
  stream << double_data;
  std::vector<byte> result = stream.out();
  print_rlp_code("double", double_data, result);
  double_data = 0;
  fetch(RLP(result), double_data);
  platon::println(double_data);
}

TEST_CASE(rlp, u256) {
  u256 u256_data = 0; 
  u256_data.value()[0] = 0x01;
  u256_data.value()[1] = 0x02;
  u256_data.value()[2] = 0x03;
  u256_data.value()[3] = 0x04;
  u256_data.value()[4] = 0x05;
  u256_data.value()[5] = 0x06;
  u256_data.value()[6] = 0x07;
  u256_data.value()[7] = 0x08;
  u256_data.value()[8] = 0x09;
  u256_data.value()[9] = 0x0a;
  u256_data.value()[10] = 0x0b;
  RLPStream stream;
  stream << u256_data;
  std::vector<byte> result = stream.out();
  print_rlp_code("u256", u256_data, result);
  u256 fetch_data = 0;
  fetch(RLP(result), fetch_data);
  ASSERT_EQ(u256_data, fetch_data);
}

TEST_CASE(rlp, string) {
  std::string string_data = "abc"; 
  RLPStream stream;
  stream << string_data;
  std::vector<byte> result = stream.out();
  print_rlp_code("string", string_data, result);
  string_data = "";
  fetch(RLP(result), string_data);
  ASSERT_EQ(string_data, std::string("abc"), string_data);
}

TEST_CASE(rlp, address) {
  std::string data = "9443355c787c50b647c425f594b441d4bd751951c1";
  RLPStream stream;
  stream << Address("0x43355c787c50b647c425f594b441d4bd751951c1", true);
  std::string result = toHex(stream.out());
  ASSERT_EQ(result, data, result, data);
}

TEST_CASE(rlp, array) {
  std::string data =
      "aa3078343333353563373837633530623634376334323566353934623434316434626437"
      "35313935316331";
  RLPStream stream;
  stream << "0x43355c787c50b647c425f594b441d4bd751951c1";
  std::string result = toHex(stream.out());
  ASSERT_EQ(result, data, result, data);
}

TEST_CASE(rlp, list) {
  std::list<std::string> list_data = {"abc", "def", "ghi"};
  RLPStream stream;
  stream << list_data;
  std::vector<byte> result = stream.out();
  print_rlp_code("list", "list_data", result);
  std::list<std::string> fetch_data;
  fetch(RLP(result), fetch_data);
  ASSERT_EQ(fetch_data, list_data);
}

TEST_CASE(rlp, append){
  std::vector<uint32_t> vect_result;
  RLPStream stream(4);
  uint32_t data_32= 1*256*256*256 + 2*256*256 + 3*256 + 4;
  stream << data_32;
  vect_result.push_back(data_32);
  data_32= 5*256*256*256 + 6*256*256 + 7*256 + 8;
  stream << data_32;
  vect_result.push_back(data_32);
  data_32= 9*256*256*256 + 10*256*256 + 11*256 + 12;
  stream << data_32;
  vect_result.push_back(data_32);
  data_32= 13*256*256 + 14*256 + 15;
  stream << data_32;
  vect_result.push_back(data_32);
  std::vector<byte> result = stream.out();
  print_rlp_code("append", "test", result);
  std::vector<uint32_t> vect_fetch;
  fetch(RLP(result), vect_fetch);
  ASSERT_EQ(vect_result, vect_fetch);
}

TEST_CASE(rlp, struct) {
  WhtType one("jatel", 30, 160);
  RLPStream wht_stream;
  wht_stream << one;
  std::vector<byte> result = wht_stream.out();
  print_rlp_code("struct", "test", result);
  WhtType two;
  fetch(RLP(result), two);
  ASSERT_EQ(two, one);
}

TEST_CASE(rlp, group) {
  WhtType one("jatel", 30, 160);
  WhtGroup group("group", 3, one);
  RLPStream wht_stream;
  wht_stream << group;
  std::vector<byte> result = wht_stream.out();
  print_rlp_code("group", "test", result);
  WhtGroup one_group;
  fetch(RLP(result), one_group);
  ASSERT_EQ(one_group, group);
}

TEST_CASE(rlp, Derived) {
  WhtType one("jatel", 30, 160);
  Derived test_derived("group", 3, one, "jatel_derived");
  RLPStream wht_stream;
  wht_stream << test_derived;
  std::vector<byte> result = wht_stream.out();
  print_rlp_code("Derived", "test", result);
  Derived one_test_derived;
  fetch(RLP(result), one_test_derived);
  ASSERT_EQ(test_derived, one_test_derived);
}

UNITTEST_MAIN() {
  RUN_TEST(rlp, int8_t)
  RUN_TEST(rlp, int16_t)
  RUN_TEST(rlp, int)
  RUN_TEST(rlp, int32_t)
  RUN_TEST(rlp, int64_t)
  RUN_TEST(rlp, bool)
  RUN_TEST(rlp, uint8_t)
  RUN_TEST(rlp, uint16_t)
  RUN_TEST(rlp, uint32_t)
  RUN_TEST(rlp, uint64_t)
  RUN_TEST(rlp, float)
  RUN_TEST(rlp, double)
  RUN_TEST(rlp, u256)
  RUN_TEST(rlp, string)
  RUN_TEST(rlp, address)
  RUN_TEST(rlp, array)
  RUN_TEST(rlp, list)
  RUN_TEST(rlp, append)
  RUN_TEST(rlp, struct)
  RUN_TEST(rlp, group)
  RUN_TEST(rlp, Derived)
}