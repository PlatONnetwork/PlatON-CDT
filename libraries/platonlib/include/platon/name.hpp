#ifndef __NAME_H_
#define __NAME_H_

#include <string>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-string-literal-operator-template"

namespace platon {
namespace detail {
template <char... Str>
struct to_const_char_arr {
  static constexpr const char value[] = {Str..., '\0'};
};
}  // namespace detail
}  // namespace platon

/**
 * string-literal operator
 *
 * @brief "foo"_n representing a constant of type `const char*` and value is "foo"
 */
template <typename T, T... Str>
inline constexpr const char* operator""_n() {
  constexpr const char* ss = platon::detail::to_const_char_arr<Str...>::value;
  return ss;
}

#endif  // __NAME_H_
