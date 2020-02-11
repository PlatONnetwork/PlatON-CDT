#include "platon/RLP.h"
#include "platon/rlp_extend.hpp"
#include <iostream>
#include <fstream>

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
    std::ifstream rlp_file("rlpdata", std::ios::binary);
    rlp_file.seekg(0, std::ios_base::end);
    int byte_len = rlp_file.tellg();
    rlp_file.seekg(0, std::ios_base::beg);
    bytes arg_bytes;
    arg_bytes.resize(byte_len);
    rlp_file.read((char *)arg_bytes.data(), sizeof(byte) * byte_len);
    rlp_file.close();

    RLP rlp(arg_bytes);
    std::string method;
    fetch(rlp[0], method);
    std::cout << method << std::endl;

    bytes init_arg;
    fetch(rlp[1], init_arg);

    uint64_t transfer_value;
    fetch(rlp[2], transfer_value);
    std::cout << transfer_value << std::endl;

    uint64_t gas_value;
    fetch(rlp[3], gas_value);
    std::cout << gas_value << std::endl;

    std::vector<byte> test_Data = std::vector<byte>{138, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    bytes test_bytes;
    fetch(RLP(test_Data), test_bytes);
    for (auto one : test_bytes) {
        std::cout << int(one) << ' ';
    }
    std::cout << std::endl;
}