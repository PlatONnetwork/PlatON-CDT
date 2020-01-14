#include "platon/cross_call.hpp"
#include <iostream>

platon::bytes global_bytes;

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

size_t platon_get_call_output_length(){
    return global_bytes.size();
}

void platon_get_call_output(uint8_t *value){
    for (auto one :global_bytes) {
        *value = one;
        value++;
    }
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

    global_bytes = paras;
    std::tuple<std::string, std::string, uint32_t> call_result;
    platon::get_call_output(call_result);
    std::cout << std::get<0>(call_result) << ' ' << std::get<1>(call_result) << ' ' << std::get<2>(call_result) << std::endl;
}