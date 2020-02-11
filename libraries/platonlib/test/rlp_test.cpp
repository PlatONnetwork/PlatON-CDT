#include "platon/RLP.h"
#include "platon/rlp_extend.hpp"
#include "platon/rlp_serialize.hpp"
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
# include <map>
#include <iomanip>
#include <stdio.h>

using namespace platon;

#ifdef __cplusplus
extern "C" {
#endif

void platon_debug(const uint8_t *dst, size_t len){
    for (size_t i = 0; i < len; i++){   
        std::cout << *(char*)(dst + i);
    }
    std::cout << std::endl;
}

void platon_panic(){
    std::cout << "platon panic"<< std::endl;
}

#ifdef __cplusplus
}
#endif

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
        friend std::ostream &operator<<( std::ostream &output, const WhtType &one){
            return output << one.m_name << ' ' << one.m_age << ' ' << one.m_weight;
        }
    private:
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
        friend std::ostream &operator<<( std::ostream &output, const WhtGroup &group){
            return output << group.m_info << ' ' << group.m_number << ' ' << group.m_member;
        }
    private:
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
        friend std::ostream &operator<<( std::ostream &output, const parent &group){
            return output << group.m_info << ' ' << group.m_number << ' ' << group.m_member;
        }
    private:
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
        friend std::ostream &operator<<( std::ostream &output, const derived &group){
            return output << static_cast<const parent&>(group) << ' ' << group.m_other;
        }
    private:
	    std::string m_other;  
};

int main(int argc, char **argv) {
    // 定义字节输出函数
    auto print = [](const byte& n) { std::cout << int(n) << ' '; };

    // 定义变量
    WhtType one("jatel", 30, 160);
    WhtGroup group("group", 3, one);
    RLPStream wht_stream;

    // 测试 WhtType
    wht_stream << one;
    std::vector<byte> result = wht_stream.out();
    std::for_each(result.begin(), result.end(), print);
    std::cout <<  std::endl;

    WhtType two;
    fetch(RLP(result), two);
    std::cout << two;
    std::cout <<  std::endl;

    // 测试 WhtGroup
    wht_stream.clear();
    wht_stream << group;
    result = wht_stream.out();
    std::for_each(result.begin(), result.end(), print);
    std::cout <<  std::endl;

    WhtGroup one_group;
    fetch(RLP(result), one_group);
    std::cout << one_group;
    std::cout <<  std::endl;

    // 测试 parent
    parent test_parent("group", 3, one);
    wht_stream.clear();
    wht_stream << test_parent;
    result = wht_stream.out();
    std::for_each(result.begin(), result.end(), print);
    std::cout <<  std::endl;

    parent one_test_parent;
    fetch(RLP(result), one_test_parent);
    std::cout << one_test_parent;
    std::cout <<  std::endl;

    // 测试 derived
    derived test_derived("group", 3, one, "jatel_derived");
    wht_stream.clear();
    wht_stream << test_derived;
    result = wht_stream.out();
    std::for_each(result.begin(), result.end(), print);
    std::cout <<  std::endl;

    derived one_test_derived;
    fetch(RLP(result), one_test_derived);
    std::cout << one_test_derived;
    std::cout <<  std::endl;

    // 测试 std::map
    std::map<std::string, derived> map_test;
    map_test["wht_1"] = test_derived;
    map_test["wht_2"] = derived("group_map", 3, one, "jatel_derived_map");
    wht_stream.clear();
    wht_stream << map_test;
    result = wht_stream.out();

    std::map<std::string, derived> map_test_one;
    fetch(RLP(result), map_test_one);
    for( auto i : map_test_one) {
        std::cout << i.first << ' ' << i.second << ' ';
    }
    std::cout << std::endl;

    // 测试追加数据
    wht_stream.clear();
    wht_stream.appendList(4);
    uint32_t data_32= 1*256*256*256 + 2*256*256 + 3*256 + 4;
    wht_stream << data_32;
    data_32= 5*256*256*256 + 6*256*256 + 7*256 + 8;
    wht_stream << data_32;
    data_32= 9*256*256*256 + 10*256*256 + 11*256 + 12;
    wht_stream << data_32;
    data_32= 13*256*256 + 14*256 + 15;
    wht_stream << data_32;
    result = wht_stream.out();
    for(auto i : result){
        std::cout << int(i) << ' ';
    }
    std::cout << std::endl;

    // 负数 rlp 编码
    wht_stream.clear();
    int negative_data = -1;
    long long_data = long(negative_data);
    uint64_t u64_data = uint64_t((long_data << 1) ^ (long_data >> 63));
    wht_stream << u64_data;
    result = wht_stream.out();
    for(auto i : result){
        std::cout << int(i) << ' ';
    }
    std::cout << std::endl;
    int negative_result = 0;
    u64_data = 0;
    fetch(RLP(result), u64_data);
    long_data = long((u64_data >> 1) ^ -(u64_data & 1));
    negative_result = int(long_data);
    std::cout << negative_result << std::endl;

    // 字符串 rlp 编码
    wht_stream.clear();
    std::string strdata = "abc";
    wht_stream << strdata;
    result = wht_stream.out();
    for(auto i : result){
        std::cout << int(i) << ' ';
    }
    std::cout << std::endl;

    // float 类型编码
    float float_data = -1.2;
    uint32_t u32_data = *(uint32_t *)(&float_data);
    wht_stream.clear();
    wht_stream << u32_data;
    result = wht_stream.out();
    for(auto i : result){
        std::cout << int(i) << ' ';
    }
    std::cout << std::endl;

    u32_data = 0;
    fetch(RLP(result), u32_data);
    float float_result = *(float *)(&u32_data);
    std::cout << float_result << std::endl;

    // double 类型编码
    double double_data = -1.23;
    std::cout << sizeof(double_data) << std::endl;
    u64_data = *(uint64_t *)(&double_data);
    wht_stream.clear();
    wht_stream << u64_data;
    result = wht_stream.out();
    for(auto i : result){
        std::cout << int(i) << ' ';
    }
    std::cout << std::endl;

    u64_data = 0;
    fetch(RLP(result), u64_data);
    double double_result = *(double *)(&u64_data);
    printf("%lf\n", double_result);

    // bool 类型 
    bool bool_data  = true;
    wht_stream.clear();
    wht_stream << bool_data;
    result = wht_stream.out();
    for(auto i : result){
        std::cout << int(i) << ' ';
    }
    std::cout << std::endl;

    bool_data = false;
    fetch(RLP(result), bool_data);
    std::cout.setf(std::ios_base::boolalpha);
    std::cout << bool_data << std::endl;

    // char
    signed char char_data  = -1;
    wht_stream.clear();
    wht_stream << char_data;
    result = wht_stream.out();
    for(auto i : result){
        std::cout << int(i) << ' ';
    }
    std::cout << std::endl;

    char_data = 0;
    fetch(RLP(result), char_data);
    std::cout << int(char_data) << std::endl;

    // short
    short short_data  = -2;
    wht_stream.clear();
    wht_stream << short_data;
    result = wht_stream.out();
    for(auto i : result){
        std::cout << int(i) << ' ';
    }
    std::cout << std::endl;

    short_data = 0;
    fetch(RLP(result), short_data);
    std::cout << short_data << std::endl;

    // int 
    short int_data  = -3;
    wht_stream.clear();
    wht_stream << int_data;
    result = wht_stream.out();
    for(auto i : result){
        std::cout << int(i) << ' ';
    }
    std::cout << std::endl;

    int_data = 0;
    fetch(RLP(result), int_data);
    std::cout << int_data << std::endl;

    // long
    long_data  = -4;
    wht_stream.clear();
    wht_stream << long_data;
    result = wht_stream.out();
    for(auto i : result){
        std::cout << int(i) << ' ';
    }
    std::cout << std::endl;

    long_data = 0;
    fetch(RLP(result), long_data);
    std::cout << long_data << std::endl;

    // float
    float_data  = 2.3;
    wht_stream.clear();
    wht_stream << float_data;
    result = wht_stream.out();
    for(auto i : result){
        std::cout << int(i) << ' ';
    }
    std::cout << std::endl;

    float_data = 0;
    fetch(RLP(result), float_data);
    std::cout << float_data << std::endl;

    // double
    double_data  = 3.4;
    wht_stream.clear();
    wht_stream << double_data;
    result = wht_stream.out();
    for(auto i : result){
        std::cout << int(i) << ' ';
    }
    std::cout << std::endl;

    double_data = 0;
    fetch(RLP(result), double_data);
    std::cout << double_data << std::endl;
}
