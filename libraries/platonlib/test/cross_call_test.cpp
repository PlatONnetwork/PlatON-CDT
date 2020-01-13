#include "platon/cross_call.hpp"
#include <iostream>

#ifdef __cplusplus
extern "C" {
#endif

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

int main(int argc, char **argv) {
    platon::bytes paras = platon::cross_call_args("init", std::string("jatel"), 56);
    std::tuple<std::string, std::string, uint32_t> call_paras;
    platon::fetch(platon::RLP(paras), call_paras);
    std::cout << std::get<0>(call_paras) << ' ' << std::get<1>(call_paras) << ' ' << std::get<2>(call_paras) << std::endl;
    uint32_t value = 4 + 256*3 + 256*256*2 + 256*256*256*1;
    platon::bytes value_byte = platon::value_to_bytes(value);
    for(auto one_byte : value_byte){
        std::cout << uint32_t(one_byte) << std::endl;
    }

}