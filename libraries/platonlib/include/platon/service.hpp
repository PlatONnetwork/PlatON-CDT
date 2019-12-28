#pragma once
#include<vector>
#include<string>

#ifdef __cplusplus
extern "C" {
#endif

void platon_return(const uint8_t *value, const size_t len);
size_t platon_get_input_length(void);
void platon_get_input(const uint8_t *value);
void platon_debug(uint8_t *dst, size_t len);
void platon_panic(void);

#ifdef __cplusplus
}
#endif

namespace platon {

    inline std::vector<byte> get_input(void) {
        std::vector<byte> result;
        size_t len = ::platon_get_input_length();
        result.resize(len);
        ::platon_get_input(result.data());
        return result;
    }

   inline void platon_throw(const std::string msg){
      size_t len = msg.length();
      ::platon_debug((uint8_t *)msg.c_str(), len);
      ::platon_panic();
   }

}