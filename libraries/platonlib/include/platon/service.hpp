#pragma once
#include<vector>
#include<string>
#include "platon/common.h"
#include "platon/RLP.h"

#ifdef __cplusplus
extern "C" {
#endif

void platon_return(const void *res, const size_t len);
size_t platon_input_length(void);
void platon_get_input(const void *inputptr);
void platon_panic( const char* cstr, const uint32_t len);

#ifdef __cplusplus
}
#endif

namespace platon {

    template<typename T>
    void platon_return(const T &t) {
        RLPStream rlp_stream;
        rlp_stream << t;
        std::vector<byte> result = rlp_stream.out();
        ::platon_return(result.data(), result.size()); 
    } 

    std::vector<byte> get_input(void) {
        std::vector<byte> result;
        size_t len = ::platon_input_length();
        result.resize(len);
        ::platon_get_input(result.data());
        return result;
    }

    inline void platon_assert(bool pred, const char* msg) {
      if (!pred) {
		 ::platon_panic(msg, strlen(msg));
      }
   }

   inline void platon_assert(bool pred, const std::string& msg) {
      if (!pred) {
		 ::platon_panic((char *)msg.c_str(), msg.size());
      }
   }

   inline void platon_assert(bool pred, std::string&& msg) {
      if (!pred) {
		 ::platon_panic((char *)msg.c_str(), msg.size());
      }
   }

}