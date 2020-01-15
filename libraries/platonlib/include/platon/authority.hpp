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

static const size_t address_len = 20;

void platon_caller(Address &platon_address){
    bytes address_bytes;
    address_bytes.resize(address_len);
    ::platon_caller(address_bytes.data());
    platon_address = Address(address_bytes);
}

void platon_origin_caller(Address &platon_address){
    bytes address_bytes;
    address_bytes.resize(address_len);
    ::platon_origin(address_bytes.data());
    platon_address = Address(address_bytes);
}

void platon_contract_address(Address &platon_address){
    bytes address_bytes;
    address_bytes.resize(address_len);
    ::platon_address(address_bytes.data());
    platon_address = Address(address_bytes);
}

}