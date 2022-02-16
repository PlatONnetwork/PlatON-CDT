#pragma once

#include <stdio.h>
#include <string>

namespace std {
string to_string(uint128_t value) {
  const char* charmap = "0123456789";
  string result;
  result.reserve(40);  // max. 40 digits possible ( uint64_t has 20)
  uint128_t helper = value;
  do {
    result += charmap[helper % 10];
    helper /= 10;
  } while (helper);
  reverse(result.begin(), result.end());
  return result;
}

string to_string(int128_t value) {
  std::string prefix;
  uint128_t absolute_value = value;
  if (value < 0) {
    absolute_value = static_cast<uint128_t>(0 - value);
    prefix = "-";
  }
  return prefix + to_string(absolute_value);
}

}  // namespace std

namespace platon {

inline void print(std::string& all_info, const char* ptr) { all_info += ptr; }

inline void print(std::string& all_info, std::string info) { all_info += info; }

template <typename T,
          class = typename std::enable_if<
              std::numeric_limits<std::decay_t<T>>::is_integer ||
              std::numeric_limits<std::decay_t<T>>::is_iec559>::type>
inline void print(std::string& all_info, const T num) {
  if constexpr (std::is_same<T, char>::value) {
    all_info += num;
  } else if constexpr (std::is_same<T, bool>::value) {
    all_info += num ? "true" : "false";
  } else if constexpr (std::is_floating_point<T>::value) {
    double double_data = static_cast<double>(num);
    char temp_info[100] = {};
    sprintf(temp_info, "%lf", double_data);
    all_info += std::string(temp_info);
  } else {
    all_info += std::to_string(num);
  }
}

template <typename Arg, typename... Args>
void print(std::string& all_info, Arg&& a, Args&&... args) {
  print(all_info, std::forward<Arg>(a));
  print(all_info, " ");
  print(all_info, std::forward<Args>(args)...);
}

template <typename Arg, typename... Args>
void println(Arg&& a, Args&&... args) {
#ifndef NDEBUG
  std::string all_info;
  print(all_info, std::forward<Arg>(a), std::forward<Args>(args)...);
  printf("%s\t\n", all_info.c_str());
#endif
}
#ifndef NDEBUG
#define DEBUG(...)                                                      \
  platon::println("line", __LINE__, "file", __FILE__, "func", __func__, \
                  ##__VA_ARGS__);
#else
#define DEBUG(...)
#endif
}  // namespace platon