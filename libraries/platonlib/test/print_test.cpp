#include "platon/print.hpp"
#include <iostream>

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

int main(int argc, char **argv) {
    char test_char = 'a';
    platon::println(test_char);
    DEBUG(test_char)
    int test_int = -123;
    platon::println(test_int);
    DEBUG(test_int)
    float test_float = 1.23;
    platon::println(test_float);
    DEBUG(test_float)
    double test_double = -4.56;
    platon::println(test_double);
    DEBUG(test_double)
}