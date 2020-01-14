#pragma once
#include "fixedhash.hpp"

#ifdef __cplusplus
extern "C" {
#endif

void platon_origin(uint8_t addr[20]);
void platon_caller(uint8_t addr[20]);
void platon_address(uint8_t addr[20]);

#ifdef __cplusplus
}
#endif


namespace platon{

void platon_caller(Address &platon_address){
    uint8_t addr[20] = {};
    ::platon_caller(addr);
    const char * addres_ptr = (const char *)addr;
    platon_address = Address(addres_ptr);
}

void platon_origin_caller(Address &platon_address){
    uint8_t addr[20] = {};
    ::platon_origin(addr);
    const char * addres_ptr = (const char *)addr;
    platon_address = Address(addres_ptr);
}

void platon_contract_address(Address &platon_address){
    uint8_t addr[20] = {};
    ::platon_address(addr);
    const char * addres_ptr = (const char *)addr;
    platon_address = Address(addres_ptr);
}

}