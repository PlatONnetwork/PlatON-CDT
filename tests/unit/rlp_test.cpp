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
        WhtType(std::string name, uint16_t age, uint16_t weight): m_name(name), m_age(age), m_weight(weight) {}
        friend RLPStream& operator << ( RLPStream& rlp, const WhtType& one ){
            return rlp.appendList(3) << one.m_name << one.m_age << one.m_weight;
        }
        friend void fetch(RLP rlp, WhtType& one){
            fetch(rlp[0], one.m_name);
            fetch(rlp[1], one.m_age);
            fetch(rlp[2], one.m_weight);
        }
        friend bool operator == ( const WhtType& lhs, const WhtType& rhs ){
            return lhs.m_age == rhs.m_age && lhs.m_name == rhs.m_name && lhs.m_weight == rhs.m_weight;
        }
    public:
	    std::string m_name;
	    uint16_t m_age;  
	    uint16_t m_weight;
};

class WhtGroup
{
    public:
        WhtGroup(){}
        WhtGroup(std::string info, uint16_t number,WhtType member): m_info(info), m_number(number), m_member(member) {}
        friend RLPStream& operator << ( RLPStream& rlp, const WhtGroup& group ){
            return rlp.appendList(3) << group.m_info << group.m_number << group.m_member;
        }
        friend void fetch(RLP rlp, WhtGroup& group){
            fetch(rlp[0], group.m_info);
            fetch(rlp[1], group.m_number);
            fetch(rlp[2], group.m_member);
        }
        friend bool operator == ( const WhtGroup& lhs, const WhtGroup& rhs ){
            return lhs.m_info == rhs.m_info && lhs.m_number == rhs.m_number 
            && lhs.m_member == rhs.m_member;
        }
    public:
	    std::string m_info;
	    uint16_t m_number;
	    WhtType m_member;
};

class parent 
{   
    public:
        parent(){}
        parent(std::string info, uint16_t number, WhtType member): m_info(info), m_number(number), m_member(member) {}
        PLATON_SERIALIZE(parent, (m_info)(m_number)(m_member))
        friend bool operator == ( const parent& lhs, const parent& rhs ){
            return lhs.m_info == rhs.m_info && lhs.m_number == rhs.m_number 
            && lhs.m_member == rhs.m_member;
        }
    public:
	    std::string m_info;
	    uint16_t m_number;
	    WhtType m_member;
};

class derived : public parent 
{
    public:
        derived(){}
        derived(std::string info, uint16_t number, WhtType member, std::string other): parent(info, number, member), m_other(other){}
        PLATON_SERIALIZE_DERIVED(derived, parent, (m_other))
        friend bool operator == ( const derived& lhs, const derived& rhs ){
            return lhs.m_other == rhs.m_other && 
            dynamic_cast<const parent &>(lhs) == dynamic_cast<const parent&>(rhs);
        }
    public:
	    std::string m_other;  
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

TEST_CASE(rlp, derived) {
  WhtType one("jatel", 30, 160);
  derived test_derived("group", 3, one, "jatel_derived");
  RLPStream wht_stream;
  wht_stream << test_derived;
  std::vector<byte> result = wht_stream.out();
  print_rlp_code("derived", "test", result);
  derived one_test_derived;
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
  RUN_TEST(rlp, string)
  RUN_TEST(rlp, address)
  RUN_TEST(rlp, array)
  RUN_TEST(rlp, append)
  RUN_TEST(rlp, struct)
  RUN_TEST(rlp, group)
  RUN_TEST(rlp, derived)
}