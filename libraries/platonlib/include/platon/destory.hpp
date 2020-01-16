#pragma once
#include <stdint.h>
#include "fixedhash.hpp"
#include "cross_call.hpp"
#include "authority.hpp"

#ifdef __cplusplus
extern "C" {
#endif

int32_t platon_destroy();
int32_t platon_migrate(uint8_t newAddr[20], const uint8_t* args, size_t argsLen, 
    const uint8_t* value, size_t valueLen, const uint8_t* callCost, size_t callCostLen);

#ifdef __cplusplus
}
#endif

namespace platon {

    template<typename value_type, typename gas_type>
    int32_t platon_migrate_contract(Address &platon_address, const bytes &init_args, 
        value_type value, gas_type gas) {   
        // value and gas
        bytes value_bytes = value_to_bytes(value);
        bytes gas_bytes = value_to_bytes(gas);

        // call platon_migrate
        bytes address_bytes;
        address_bytes.resize(address_len);
        ::platon_migrate(address_bytes.data(), init_args.data(), init_args.size(), value_bytes.data(), 
        value_bytes.size(), gas_bytes.data(), gas_bytes.size());

        //set address
        platon_address =  Address(address_bytes);
        return 0;
    }
}