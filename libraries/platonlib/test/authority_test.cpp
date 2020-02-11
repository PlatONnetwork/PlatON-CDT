#include "platon/fixedhash.hpp"
#include "platon/RLP.h"
#include "platon/rlp_extend.hpp"
#include "platon/print.hpp"
#include <iostream>

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
    std::cout << "platon panic" << std::endl;
}

#ifdef __cplusplus
}
#endif


int main(int argc, char **argv) {
    Address test_address("0x43355c787c50b647c425f594b441d4bd751951c1");
    RLPStream wht_stream;
    wht_stream << test_address;
    bytes result = wht_stream.out();

    Address result_address;
    fetch(RLP(result), result_address);
    std::cout << result_address.toString() << std::endl;
    std::cout << (test_address == result_address) << std::endl;
    DEBUG("result_address", result_address.toString())
}