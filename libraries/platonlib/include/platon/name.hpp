#pragma once
#include <string>
#include "platon/panic.hpp"
#include "platon/rlp_serialize.hpp"
#include "print.hpp"

namespace platon {
namespace detail {
template <char... Str>
struct to_const_char_arr {
  static constexpr const char value[] = {Str...};
};
}  // namespace detail
constexpr uint64_t kOffset64 = 14695981039346656037U;
constexpr uint64_t kPrime64 = 1099511628211U;
/**
 * @defgroup name
 * @ingroup core
 * @ingroup types
 * @brief PlatON Name Type
 */

/**
 * Wraps a %uint64_t to ensure it is only passed to methods that expect a %name.
 * Ensures value is only passed to methods that expect a %name and that no
 * mathematical operations occur.  Also enables specialization of print
 *
 * @ingroup name
 */
struct Name {
 public:
  enum class Raw : uint64_t {};

  /**
   * Construct a new name
   *
   * @brief Construct a new name object defaulting to a value of 0
   *
   */
  constexpr Name() : value(0) {}

  /**
   * Construct a new name given a unit64_t value
   *
   * @brief Construct a new name object initialising value with v
   * @param v - The unit64_t value
   *
   */
  constexpr explicit Name(uint64_t v) : value(v) {}

  /**
   * Construct a new name given a scoped enumerated type of raw (uint64_t).
   *
   * @brief Construct a new name object initialising value with r
   * @param r - The raw value which is a scoped enumerated type of unit64_t
   *
   */
  constexpr explicit Name(Name::Raw r) : value(static_cast<uint64_t>(r)) {}

  /**
   * Construct a new name given an string.
   *
   * @brief Construct a new name object initialising value with str
   * @param str - The string value which validated then converted to unit64_t
   *
   */
  constexpr explicit Name(std::string_view str) : value(0) {
    size_t n = str.length();
    uint64_t hash = kOffset64;
    for (size_t i = 0; i < n; i++) {
      hash *= kPrime64;
      hash ^= (uint64_t)str[i];
    }
    value = hash;
  }

  /**
   * Casts a name to raw
   *
   * @return Returns an instance of raw based on the value of a name
   */
  constexpr operator Raw() const { return Raw(value); }

  /**
   * Explicit cast to bool of the uint64_t value of the name
   *
   * @return Returns true if the name is set to the default value of 0 else
   * true.
   */
  constexpr explicit operator bool() const { return value != 0; }

  /// @cond INTERNAL

  /**
   * Equivalency operator. Returns true if a == b (are the same)
   *
   * @return boolean - true if both provided %name values are the same
   */
  friend constexpr bool operator==(const Name& a, const Name& b) {
    return a.value == b.value;
  }

  /**
   * Inverted equivalency operator. Returns true if a != b (are different)
   *
   * @return boolean - true if both provided %name values are not the same
   */
  friend constexpr bool operator!=(const Name& a, const Name& b) {
    return a.value != b.value;
  }

  /**
   * Less than operator. Returns true if a < b.
   *
   * @return boolean - true if %name `a` is less than `b`
   */
  friend constexpr bool operator<(const Name& a, const Name& b) {
    return a.value < b.value;
  }

  /// @endcond

  uint64_t value = 0;

  PLATON_SERIALIZE(Name, (value))
};
inline constexpr uint64_t name_value(char const* str,
                                     uint64_t last_value = kOffset64) {
  return *str ? name_value(str + 1, (kPrime64 * last_value) ^ *str)
              : last_value;
}
}  // namespace platon

/**
 * string-literal operator
 *
 * @brief "foo"_n representing a constant of type `const char*` and value is
 * "foo"
 */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-string-literal-operator-template"
template <typename T, T... Str>
inline constexpr platon::Name operator""_n() {
  constexpr auto x = platon::Name{std::string_view{
      platon::detail::to_const_char_arr<Str...>::value, sizeof...(Str)}};
  return x;
}



#pragma clang diagnostic pop
