#include <stdio.h>
#include <string>

std::string all_info;
char temp_info[500];

inline void print() { printf("\n"); }

inline void print( const char* ptr ) { sprintf(temp_info, ptr); all_info += std::string(temp_info);}

inline void print(std::string info) { sprintf(temp_info, info.c_str()); all_info += std::string(temp_info);}

inline void print( float num ) { sprintf(temp_info, "%f", num); all_info += std::string(temp_info);}

inline void print( double num ) { sprintf(temp_info, "%lf", num); all_info += std::string(temp_info);}

inline void print( long double num ) { sprintf(temp_info, "ll%f", num); all_info += std::string(temp_info);}

inline void print(char num) { sprintf(temp_info, "%c", num); all_info += std::string(temp_info);}

inline void print(short num) { sprintf(temp_info, "%d", num); all_info += std::string(temp_info);}

inline void print(int num) { sprintf(temp_info, "%d", num); all_info += std::string(temp_info);}

inline void print(long num) { sprintf(temp_info, "%ld", num); all_info += std::string(temp_info);}

inline void print(long long num) { sprintf(temp_info, "%lld", num); all_info += std::string(temp_info);}

inline void print(uint32_t num) { sprintf(temp_info, "%lu", num); all_info += std::string(temp_info);}

inline void print(uint64_t num) { sprintf(temp_info, "%llu", num); all_info += std::string(temp_info);}

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
    print(std::forward<Args>(args)...);
}

template<typename Arg, typename... Args>
void println( Arg&& a, Args&&... args ) {
    all_info = "";
    print(std::forward<Arg>(a), std::forward<Args>(args)...);
    printf(all_info.c_str());
    printf("\n");
}