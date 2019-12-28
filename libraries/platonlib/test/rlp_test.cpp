#include "platon/RLP.h"
#include "platon/rlp_extend.hpp"
#include "platon/rlp_serialize.hpp"
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
# include <map>

using namespace platon;

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
    std::cout <<  std::endl;
}
