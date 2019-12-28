//
// Created by zhou.yang on 2018/10/22.
//
#pragma once

#include "common.h"
#include "RLP.h"
#include <string>
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif
    void platon_set_state(const uint8_t* key, size_t klen, const uint8_t *value, size_t vlen);
    size_t platon_get_state_length(const uint8_t* key, size_t klen);
    void platon_get_state(const uint8_t* key, size_t klen, uint8_t *value, size_t vlen);
#ifdef __cplusplus
}
#endif


namespace platon {
    /**
     * @brief Set the State object
     * 
     * @tparam KEY Key type
     * @tparam VALUE Value type
     * @param key Key
     * @param value Value
     */
    template <typename KEY, typename VALUE>
    inline void setState(const KEY &key, const VALUE &value) {
        RLPStream state_stream;
        state_stream << key;
        std::vector<byte> vect_key = state_stream.out();

        state_stream.clear();
        state_stream << value;
        std::vector<byte> vect_value = state_stream.out();
        ::platon_set_state(vect_key.data(), vect_key.size(),  vect_value.data(), vect_value.size());
    }
    /**
     * @brief Get the State object
     * 
     * @tparam KEY Key type
     * @tparam VALUE Value type
     * @param key Key
     * @param value Value
     * @return size_t Get the length of the data
     */
    template <typename KEY, typename VALUE>
    inline size_t getState(const KEY &key, VALUE &value) {
        RLPStream state_stream;
        state_stream << key;
        std::vector<byte> vect_key = state_stream.out();
        size_t len = ::platon_get_state_length(vect_key.data(), vect_key.size());
        if (len == 0){ return 0; }
        std::vector<byte> result;
        result.resize(len);
        ::platon_get_state(vect_key.data(), vect_key.size(), result.data(), result.size());
        fetch(RLP(result), value);
        return len;
    }

    /**
     * @brief delete State Object
     * 
     * @tparam KEY Key type
     * @param key Key
     */
    template <typename KEY>
    inline void delState(const KEY &key) {
        RLPStream state_stream;
        state_stream << key;
        std::vector<byte> vect_key = state_stream.out();
        byte del = 0;
        ::platon_set_state(vect_key.data(), vect_key.size(),  (const byte*)&del, 0);
    }

}
