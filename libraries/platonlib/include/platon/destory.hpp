#pragma once
#include <stdint.h>
#include "fixedhash.hpp"
#include "cross_call.hpp"

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

    template<typename value_type, typename gas_type, typename... Args>
    int32_t platon_migrate(Address &platon_address, const bytes &code,
    const value_type &value, const gas_type &gas, const Args &... args) {
        bytes init_args = cross_call_args("init", args...);
        
        // encode
        RLPStream stream;
        stream.appendList(2) << code << init_args;
        bytes deploy_args = stream.out();

        // add magic number
        bytes real_args;
        for (int i = 0; i < 4; i++) {
            real_args.push_back(code[i]);
        }
        std::copy(deploy_args.begin(), deploy_args.end(), std::back_inserter(real_args));

        // value and gas
        bytes value_bytes = value_to_bytes(value);
        bytes gas_bytes = value_to_bytes(gas);

        // call platon_migrate
        uint8_t addr[20] = {};
        ::platon_migrate(addr, real_args.data(), real_args.size(), value_bytes.data(), 
        value_bytes.size(), gas_bytes.data(), gas_bytes.size());

        //set address
        const char * addres_ptr = (const char *)addr;
        platon_address = Address(addres_ptr);
    }
}