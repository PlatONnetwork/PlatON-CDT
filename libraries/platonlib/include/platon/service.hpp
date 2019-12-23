#pragma once
#include<vector>
#include<string>

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
        auto data = pack(t);
        ::platon_return(data.data(), data.size()); 
    } 

    std::vector<char> get_input(void) {
        std::vector<char> result;
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