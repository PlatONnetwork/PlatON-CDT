#include "platon/contract.hpp"
#include "platon/rlp_serialize.hpp"
#include "platon/dispatcher.hpp"
#include <iostream>
#include <string>
#include <vector>

using namespace platon;

std::vector<byte> input_result;

#ifdef __cplusplus
extern "C" {
#endif

void platon_return(const uint8_t *value, const size_t len){
    std::cout << "platon_return" << std::endl;
}

size_t platon_get_input_length(void){
    return input_result.size();
}

void platon_get_input(uint8_t *value){
    for (auto one :input_result) {
        *value = one;
        value++;
    }
}

void platon_debug(uint8_t *dst, size_t len){
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

struct Info {
    uint64_t number;
    uint64_t age;
    uint8_t married;
    std::string desc;
    std::vector<std::string> children;
    friend std::ostream &operator<<( std::ostream &output, const Info &one_info){
        output << one_info.number << ' ' << one_info.age << ' ' << one_info.married << ' ' << one_info.desc << ' ';
        for (auto one_children : one_info.children){
            output << one_children << ' ';
        }
        return output;
    }
    PLATON_SERIALIZE(Info, (number)(age)(married)(desc)(children))
};

CONTRACT input_test : public platon::Contract{
    public:
        ACTION void init(uint64_t serial, Info info){
            std::cout << "serial " << serial << std::endl;
             std::cout << "info " << info << std::endl;
        }
};

PLATON_DISPATCH(input_test, (init))

int main(int argc, char **argv) {
    std::vector<byte> one_test = {0xd9, 0x84, 0x69, 0x6e, 0x69, 0x74, 0x62, 0xd2, 0x65, 0x57, 0x80, 0x85, 0x31, 0x32, 0x33, 0x34, 0x35, 0xc8, 0x83, 0x31, 0x32, 0x33, 0x83, 0x34, 0x35, 0x36};
    input_result = one_test;
    invoke();
}