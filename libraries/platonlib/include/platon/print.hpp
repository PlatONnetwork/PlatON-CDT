#pragma once

#include <stdio.h>
#include <string>
#include "bigint.hpp"

namespace platon {

std::string all_info;
char temp_info[500];

inline void print( const char* ptr ) { sprintf(temp_info, "%s", ptr); all_info += std::string(temp_info);}

inline void print(std::string info) { sprintf(temp_info, "%s", info.c_str()); all_info += std::string(temp_info);}

inline void print( float num ) { sprintf(temp_info, "%f", num); all_info += std::string(temp_info);}

inline void print( double num ) { sprintf(temp_info, "%lf", num); all_info += std::string(temp_info);}

inline void print( long double num ) { sprintf(temp_info, "%Lf", num); all_info += std::string(temp_info);}

inline void print(char num) { sprintf(temp_info, "%c", num); all_info += std::string(temp_info);}

inline void print(short num) { sprintf(temp_info, "%d", num); all_info += std::string(temp_info);}

inline void print(int num) { sprintf(temp_info, "%d", num); all_info += std::string(temp_info);}

inline void print(long num) { sprintf(temp_info, "%ld", num); all_info += std::string(temp_info);}

inline void print(long long num) { sprintf(temp_info, "%lld", num); all_info += std::string(temp_info);}

inline void print(uint32_t num) { sprintf(temp_info, "%lu", num); all_info += std::string(temp_info);}

inline void print(uint64_t num) { sprintf(temp_info, "%llu", num); all_info += std::string(temp_info);}

template <size_t Bits, typename Signed>
inline void print(const std::wide_integer<Bits, Signed> num) { all_info += std::to_string(num);}

inline void print( bool is_true ) { 
    if (is_true) {
        sprintf(temp_info, "true");
    } else {
        sprintf(temp_info, "false");
    }
    all_info += std::string(temp_info);
}

template<typename Arg, typename... Args>
void print( Arg&& a, Args&&... args ) {
    print(std::forward<Arg>(a));
    print(' ');
    print(std::forward<Args>(args)...);
}

template<typename Arg, typename... Args>
void println( Arg&& a, Args&&... args ) {
    all_info = "";
    print(std::forward<Arg>(a), std::forward<Args>(args)...);
    printf("%s\n", all_info.c_str());
}

#define DEBUG(...)\
platon::println("line", __LINE__, "file", __FILE__, "func", __func__, ##__VA_ARGS__); 

}