#pragma once

#include <algorithm>
#include <array>
#include <climits>
#include <cstring>
#include <string>
#include "chain.hpp"
#include "common.h"
#include <string_view>
namespace std {
template <size_t Bits, bool Signed>
class WideInteger {
 private:
  using value_type = WideInteger<Bits, Signed>;

  template <typename WideInteger1, typename WideInteger2>
  class WideType {
    constexpr static bool first_type() {
      if constexpr (WideInteger1::bits > WideInteger2::bits) {
        return true;
      } else if constexpr (WideInteger1::bits == WideInteger2::bits &&
                           WideInteger1::signed_info &&
                           !WideInteger2::signed_info) {
        return true;
      } else {
        return false;
      }
    }

   public:
    using type = typename std::conditional<first_type(), WideInteger1,
                                           WideInteger2>::type;
  };

  template <size_t Bits2, bool Signed2>
  using result_type =
      typename WideType<value_type, WideInteger<Bits2, Signed2>>::type;

 public:
  static constexpr size_t bits = Bits;
  static constexpr bool signed_info = Signed;
  static constexpr size_t arr_size = Bits / 8;

 public:
  WideInteger() = default;

  template <size_t Bits2, bool Signed2>
  WideInteger(const WideInteger<Bits2, Signed2> &rhs) {
    size_t copy_size = std::min(arr_size, rhs.arr_size);

    // The left and right operands have the same sign
    if (Signed == Signed2) {
      negative_ = rhs.negative_;
      std::copy(rhs.arr_.end() - copy_size, rhs.arr_.end(),
                arr_.end() - copy_size);
    }

    // Unsigned on the left, signed on the right
    if (!Signed && Signed2) {
      if (rhs.negative_) {
        WideInteger<Bits2, Signed2> complement = (~rhs) + 1;
        std::copy(complement.arr_.end() - copy_size, complement.arr_.end(),
                  arr_.end() - copy_size);
      } else {
        std::copy(rhs.arr_.end() - copy_size, rhs.arr_.end(),
                  arr_.end() - copy_size);
      }
    }

    // Signed on the left, unsigned on the right
    if (Signed && !Signed2) {
      std::copy(rhs.arr_.begin() - copy_size, rhs.arr_.end(),
                arr_.end() - copy_size);
    }
  }

  template <typename T,
            class = typename std::enable_if<
                std::numeric_limits<std::decay_t<T>>::is_integer ||
                std::numeric_limits<std::decay_t<T>>::is_iec559>::type>
  constexpr WideInteger(const T &num) {
    uint128_t temp_value = 0;
    if constexpr (Signed) {
      if (num < 0) {
        int128_t neg_value = num;
        negative_ = true;
        temp_value = -neg_value;
      } else {
        temp_value = num;
      }
    } else {
      temp_value = num;
    }

    for (int i = 1; temp_value != 0; temp_value >>= 8, i++) {
      arr_[arr_size - i] = temp_value & 0xff;
    }
  }

  // Type conversion function
  template <typename T,
            class = typename std::enable_if<
                std::numeric_limits<std::decay_t<T>>::is_integer ||
                std::numeric_limits<std::decay_t<T>>::is_iec559>::type>
  constexpr explicit operator T() const {
    uint128_t temp_value = 0;
    size_t copy_size = std::min(arr_size, size_t(16));
    for (int i = arr_size - copy_size; i < arr_size; i++) {
      temp_value = temp_value * 256 + arr_[i];
    }

    T result;
    if (*this < 0) {
      result = -temp_value;
    } else {
      result = temp_value;
    }

    return result;
  }

  explicit operator bool() const { return 0 != *this; }

  // Convert string to big integer
  WideInteger(const char *str, size_t str_len) {
    uint32_t operator_result = string_convert_operator(
        reinterpret_cast<const uint8_t *>(str), str_len, &(arr_[0]), arr_size);
    negative_ = BigintResultFlag::NEGATIVE & operator_result;
    overflow_ = BigintResultFlag::OVERFLOW & operator_result;
  }

  // Conversion between byte streams
  template <typename container>
  constexpr WideInteger(const container &bytes, bool big_endian) {
    if (big_endian) {
      this->FromBigEndian(bytes);
    } else {
      this->FromLittleEndian(bytes);
    }
  }

  constexpr WideInteger(const std::string_view &hex) {
    platon::fromHex<arr_size>(hex, arr_);
  }

  // Conversion between byte streams(little endian)
  template <typename container>
  constexpr value_type &FromLittleEndian(const container &bytes) {
    static_assert(!Signed, "Only unsigned numbers can do this");
    arr_.fill(0);
    auto it = bytes.begin();
    for (int i = arr_size - 1; i >= 0 && it != bytes.end(); ++it, --i) {
      arr_[i] = *it;
    }

    return *this;
  }

  template <typename container, typename functor>
  constexpr void ToLittleEndian(container &result, const functor &func) const {
    static_assert(!Signed, "Only unsigned numbers can do this");
    for (int i = arr_ - 1; i >= 0; --i) {
      func(result, arr_[i]);
    }
  }

  // Conversion between byte streams(big endian)
  template <typename container>
  constexpr value_type &FromBigEndian(const container &bytes) {
    //    static_assert(!Signed, "Only unsigned numbers can do this");
    arr_.fill(0);
    size_t len = bytes.size();
    if (len >= arr_size) {
      auto it = bytes.begin();
      if (len - arr_size > 0) std::advance(it, len - arr_size);
      for (int i = 0; i < arr_size && it != bytes.end(); ++i, ++it) {
        arr_[i] = *it;
      }
    }

    if (len < arr_size) {
      auto it = bytes.begin();
      for (int i = arr_size - len; i < arr_size && it != bytes.end();
           ++i, ++it) {
        arr_[i] = *it;
      }
    }

    return *this;
  }

  template <typename container, typename functor>
  constexpr void ToBigEndian(container &result, const functor &func) const {
    static_assert(!Signed, "Only unsigned numbers can do this");
    for (int i = 0; i < arr_size; ++i) {
      func(result, arr_[i]);
    }
  }

// macro
#define BIGINT_BINARY_OPERATOR(OPERATOR)                                  \
  bigint_binary_operator(&(arr_[0]), negative_, arr_size, &(rhs.arr_[0]), \
                         rhs.negative_, rhs.arr_size, &(result.arr_[0]),  \
                         result.arr_size, OPERATOR);

#define WIDEINTEGER_BUILT_IN_TYPE_OPERATOR(OPERATOR, RETURN_TYPE)       \
  template <typename T,                                                 \
            class = typename std::enable_if<                            \
                std::numeric_limits<std::decay_t<T>>::is_integer ||     \
                std::numeric_limits<std::decay_t<T>>::is_iec559>::type> \
  RETURN_TYPE OPERATOR(const T &value) const {                          \
    return this->OPERATOR(value_type(value));                           \
  }

// assign macro
#define BIGINT_BINARY_SELF_OPERATOR(OPERATOR)                               \
  bigint_binary_operator(&(arr_[0]), negative_, arr_size, &(rhs.arr_[0]),   \
                         rhs.negative_, rhs.arr_size, &(arr_[0]), arr_size, \
                         OPERATOR);

#define WIDEINTEGER_ASSIGN_BUILT_IN_TYPE_OPERATOR(OPERATOR, RETURN_TYPE) \
  template <typename T,                                                  \
            class = typename std::enable_if<                             \
                std::numeric_limits<std::decay_t<T>>::is_integer ||      \
                std::numeric_limits<std::decay_t<T>>::is_iec559>::type>  \
  RETURN_TYPE OPERATOR(const T &value) {                                 \
    return this->OPERATOR(value_type(value));                            \
  }

  // Addition operator
  template <size_t Bits2, bool Signed2>
  result_type<Bits2, Signed2> operator+(
      const WideInteger<Bits2, Signed2> &rhs) const {
    result_type<Bits2, Signed2> result;
    uint32_t operator_result = BIGINT_BINARY_OPERATOR(BinaryOperator::ADD);
    result.negative_ = BigintResultFlag::NEGATIVE & operator_result;
    result.overflow_ = BigintResultFlag::OVERFLOW & operator_result;
    return result;
  }
  WIDEINTEGER_BUILT_IN_TYPE_OPERATOR(operator+, value_type)

  template <size_t Bits2, bool Signed2>
  value_type &operator+=(const WideInteger<Bits2, Signed2> &rhs) {
    uint32_t operator_result = BIGINT_BINARY_SELF_OPERATOR(BinaryOperator::ADD);
    negative_ = BigintResultFlag::NEGATIVE & operator_result;
    overflow_ = BigintResultFlag::OVERFLOW & operator_result;
    return *this;
  }
  WIDEINTEGER_ASSIGN_BUILT_IN_TYPE_OPERATOR(operator+=, value_type &)

  // Subtraction operator
  template <size_t Bits2, bool Signed2>
  result_type<Bits2, Signed2> operator-(
      const WideInteger<Bits2, Signed2> &rhs) const {
    result_type<Bits2, Signed2> result;
    uint32_t operator_result = BIGINT_BINARY_OPERATOR(BinaryOperator::SUB);
    result.negative_ = BigintResultFlag::NEGATIVE & operator_result;
    result.overflow_ = BigintResultFlag::OVERFLOW & operator_result;
    return result;
  }
  WIDEINTEGER_BUILT_IN_TYPE_OPERATOR(operator-, value_type)

  template <size_t Bits2, bool Signed2>
  value_type &operator-=(const WideInteger<Bits2, Signed2> &rhs) {
    uint32_t operator_result = BIGINT_BINARY_SELF_OPERATOR(BinaryOperator::SUB);
    negative_ = BigintResultFlag::NEGATIVE & operator_result;
    overflow_ = BigintResultFlag::OVERFLOW & operator_result;
    return *this;
  }
  WIDEINTEGER_ASSIGN_BUILT_IN_TYPE_OPERATOR(operator-=, value_type &)

  constexpr value_type operator-() const {
    //    static_assert(Signed, "Only signed numbers can do this");
    value_type result = *this;
    result.Opposite();
    return result;
  }

  // Multiplication operator
  template <size_t Bits2, bool Signed2>
  result_type<Bits2, Signed2> operator*(
      const WideInteger<Bits2, Signed2> &rhs) const {
    result_type<Bits2, Signed2> result;
    uint32_t operator_result = BIGINT_BINARY_OPERATOR(BinaryOperator::MUL);
    result.negative_ = BigintResultFlag::NEGATIVE & operator_result;
    result.overflow_ = BigintResultFlag::OVERFLOW & operator_result;
    return result;
  }
  WIDEINTEGER_BUILT_IN_TYPE_OPERATOR(operator*, value_type)

  template <size_t Bits2, bool Signed2>
  value_type &operator*=(const WideInteger<Bits2, Signed2> &rhs) {
    uint32_t operator_result = BIGINT_BINARY_SELF_OPERATOR(BinaryOperator::MUL);
    negative_ = BigintResultFlag::NEGATIVE & operator_result;
    overflow_ = BigintResultFlag::OVERFLOW & operator_result;
    return *this;
  }
  WIDEINTEGER_ASSIGN_BUILT_IN_TYPE_OPERATOR(operator*=, value_type &)

  // Division operator
  template <size_t Bits2, bool Signed2>
  result_type<Bits2, Signed2> operator/(
      const WideInteger<Bits2, Signed2> &rhs) const {
    result_type<Bits2, Signed2> result;
    uint32_t operator_result = BIGINT_BINARY_OPERATOR(BinaryOperator::DIV);
    result.negative_ = BigintResultFlag::NEGATIVE & operator_result;
    result.overflow_ = BigintResultFlag::OVERFLOW & operator_result;
    return result;
  }
  WIDEINTEGER_BUILT_IN_TYPE_OPERATOR(operator/, value_type)

  template <size_t Bits2, bool Signed2>
  value_type &operator/=(const WideInteger<Bits2, Signed2> &rhs) {
    uint32_t operator_result = BIGINT_BINARY_SELF_OPERATOR(BinaryOperator::DIV);
    negative_ = BigintResultFlag::NEGATIVE & operator_result;
    overflow_ = BigintResultFlag::OVERFLOW & operator_result;
    return *this;
  }
  WIDEINTEGER_ASSIGN_BUILT_IN_TYPE_OPERATOR(operator/=, value_type &)

  // Modulo operator
  template <size_t Bits2, bool Signed2>
  result_type<Bits2, Signed2> operator%(
      const WideInteger<Bits2, Signed2> &rhs) const {
    result_type<Bits2, Signed2> result;
    uint32_t operator_result = BIGINT_BINARY_OPERATOR(BinaryOperator::MOD);
    result.negative_ = BigintResultFlag::NEGATIVE & operator_result;
    result.overflow_ = BigintResultFlag::OVERFLOW & operator_result;
    return result;
  }
  WIDEINTEGER_BUILT_IN_TYPE_OPERATOR(operator%, value_type)

  template <size_t Bits2, bool Signed2>
  value_type &operator%=(const WideInteger<Bits2, Signed2> &rhs) {
    uint32_t operator_result = BIGINT_BINARY_SELF_OPERATOR(BinaryOperator::MOD);
    negative_ = BigintResultFlag::NEGATIVE & operator_result;
    overflow_ = BigintResultFlag::OVERFLOW & operator_result;
    return *this;
  }
  WIDEINTEGER_ASSIGN_BUILT_IN_TYPE_OPERATOR(operator%=, value_type &)

  // And operator
  template <size_t Bits2, bool Signed2>
  result_type<Bits2, Signed2> operator&(
      const WideInteger<Bits2, Signed2> &rhs) const {
    result_type<Bits2, Signed2> result;
    uint32_t operator_result = BIGINT_BINARY_OPERATOR(BinaryOperator::AND);
    result.negative_ = BigintResultFlag::NEGATIVE & operator_result;
    result.overflow_ = BigintResultFlag::OVERFLOW & operator_result;
    return result;
  }
  WIDEINTEGER_BUILT_IN_TYPE_OPERATOR(operator&, value_type)

  template <size_t Bits2, bool Signed2>
  value_type &operator&=(const WideInteger<Bits2, Signed2> &rhs) noexcept {
    uint32_t operator_result = BIGINT_BINARY_SELF_OPERATOR(BinaryOperator::AND);
    negative_ = BigintResultFlag::NEGATIVE & operator_result;
    overflow_ = BigintResultFlag::OVERFLOW & operator_result;
    return *this;
  }
  WIDEINTEGER_ASSIGN_BUILT_IN_TYPE_OPERATOR(operator&=, value_type)

  // OR operator
  template <size_t Bits2, bool Signed2>
  result_type<Bits2, Signed2> operator|(
      const WideInteger<Bits2, Signed2> &rhs) const {
    result_type<Bits2, Signed2> result;
    uint32_t operator_result = BIGINT_BINARY_OPERATOR(BinaryOperator::OR);
    result.negative_ = BigintResultFlag::NEGATIVE & operator_result;
    result.overflow_ = BigintResultFlag::OVERFLOW & operator_result;
    return result;
  }
  WIDEINTEGER_BUILT_IN_TYPE_OPERATOR(operator|, value_type)

  template <size_t Bits2, bool Signed2>
  value_type &operator|=(const WideInteger<Bits2, Signed2> &rhs) {
    uint32_t operator_result = BIGINT_BINARY_SELF_OPERATOR(BinaryOperator::OR);
    negative_ = BigintResultFlag::NEGATIVE & operator_result;
    overflow_ = BigintResultFlag::OVERFLOW & operator_result;
    return *this;
  }
  WIDEINTEGER_ASSIGN_BUILT_IN_TYPE_OPERATOR(operator|=, value_type)

  // XOR operator
  template <size_t Bits2, bool Signed2>
  result_type<Bits2, Signed2> operator^(
      const WideInteger<Bits2, Signed2> &rhs) const {
    result_type<Bits2, Signed2> result;
    uint32_t operator_result = BIGINT_BINARY_OPERATOR(BinaryOperator::XOR);
    result.negative_ = BigintResultFlag::NEGATIVE & operator_result;
    result.overflow_ = BigintResultFlag::OVERFLOW & operator_result;
    return result;
  }
  WIDEINTEGER_BUILT_IN_TYPE_OPERATOR(operator^, value_type)

  template <size_t Bits2, bool Signed2>
  value_type &operator^=(const WideInteger<Bits2, Signed2> &rhs) {
    uint32_t operator_result = BIGINT_BINARY_SELF_OPERATOR(BinaryOperator::XOR);
    negative_ = BigintResultFlag::NEGATIVE & operator_result;
    overflow_ = BigintResultFlag::OVERFLOW & operator_result;
    return *this;
  }
  WIDEINTEGER_ASSIGN_BUILT_IN_TYPE_OPERATOR(operator^=, value_type)

  // Negation operator
  value_type operator~() const {
    value_type result;
    if constexpr (Signed) {
      value_type temp = -1;
      result = *this | temp;
    } else {
      value_type temp;
      temp.Fill(0xff);
      result = *this | temp;
    }
    return result;
  }

// macro
#define BIGINT_CMP()                                                         \
  bigint_cmp(&(arr_[0]), negative_, arr_size, &(rhs.arr_[0]), rhs.negative_, \
             rhs.arr_size)

  // Parity operator
  template <size_t Bits2, bool Signed2>
  bool operator==(const WideInteger<Bits2, Signed2> &rhs) const {
    int result = BIGINT_CMP();
    return 0 == result;
  }
  WIDEINTEGER_BUILT_IN_TYPE_OPERATOR(operator==, bool)

  template <size_t Bits2, bool Signed2>
  bool operator!=(const WideInteger<Bits2, Signed2> &rhs) const {
    return !(*this == rhs);
  }
  WIDEINTEGER_BUILT_IN_TYPE_OPERATOR(operator!=, bool)

  // Comparison operator
  template <size_t Bits2, bool Signed2>
  bool operator<(const WideInteger<Bits2, Signed2> &rhs) const {
    int result = BIGINT_CMP();
    return -1 == result;
  }
  WIDEINTEGER_BUILT_IN_TYPE_OPERATOR(operator<, bool)

  template <size_t Bits2, bool Signed2>
  bool operator>(const WideInteger<Bits2, Signed2> &rhs) const {
    return !(*this == rhs) && !(*this < rhs);
  }
  WIDEINTEGER_BUILT_IN_TYPE_OPERATOR(operator>, bool)

  template <size_t Bits2, bool Signed2>
  bool operator>=(const WideInteger<Bits2, Signed2> &rhs) const {
    return *this == rhs || *this > rhs;
  }
  WIDEINTEGER_BUILT_IN_TYPE_OPERATOR(operator>=, bool)

  template <size_t Bits2, bool Signed2>
  bool operator<=(const WideInteger<Bits2, Signed2> &rhs) const {
    return *this == rhs || *this < rhs;
  }
  WIDEINTEGER_BUILT_IN_TYPE_OPERATOR(operator<=, bool)

  // Increment operator
  value_type &operator++() {
    *this += value_type(1);
    return *this;
  };

  value_type &operator++(int) {
    value_type result = *this;
    *this += value_type(1);
    return result;
  };

  // Decrement operator
  value_type &operator--() {
    *this -= value_type(1);
    return *this;
  };

  value_type &operator--(int) {
    value_type result = *this;
    *this -= value_type(1);
    return result;
  };

// macro
#define BIGINT_SH(DIRECTION)                                                 \
  bigint_sh(&(arr_[0]), negative_, arr_size, &(result.arr_[0]), arr_size, n, \
            DIRECTION)

#define BIGINT_SELF_SH(DIRECTION) \
  bigint_sh(&(arr_[0]), negative_, arr_size, &(arr_[0]), arr_size, n, DIRECTION)

  // Left shift operator
  value_type operator<<(uint32_t n) const {
    value_type result;
    uint32_t operator_result = BIGINT_SH(ShiftDirection::LEFT);
    result.negative_ = BigintResultFlag::NEGATIVE & operator_result;
    result.overflow_ = BigintResultFlag::OVERFLOW & operator_result;
    return result;
  }

  value_type &operator<<=(uint32_t n) {
    uint32_t operator_result = BIGINT_SELF_SH(ShiftDirection::LEFT);
    negative_ = BigintResultFlag::NEGATIVE & operator_result;
    overflow_ = BigintResultFlag::OVERFLOW & operator_result;
    return *this;
  }

  // Right shift operator
  value_type operator>>(uint32_t n) const {
    value_type result;
    uint32_t operator_result = BIGINT_SH(ShiftDirection::RIGHT);
    result.negative_ = BigintResultFlag::NEGATIVE & operator_result;
    result.overflow_ = BigintResultFlag::OVERFLOW & operator_result;
    return result;
  }

  value_type &operator>>=(uint32_t n) {
    uint32_t operator_result = BIGINT_SELF_SH(ShiftDirection::RIGHT);
    negative_ = BigintResultFlag::NEGATIVE & operator_result;
    overflow_ = BigintResultFlag::OVERFLOW & operator_result;
    return *this;
  }

  // zk algorithm custom operator
  template <size_t Bits2, bool Signed2, size_t Bits3, bool Signed3>
  friend auto Exp(const value_type &left,
                  const WideInteger<Bits2, Signed2> &right,
                  const WideInteger<Bits3, Signed3> &mod) ->
      typename WideType<result_type<Bits2, Signed2>,
                        WideInteger<Bits3, Signed3>>::type {
    typename WideType<result_type<Bits2, Signed2>,
                      WideInteger<Bits3, Signed3>>::type result;
    uint32_t operator_result = bigint_exp_mod(
        &(left.arr_[0]), left.negative_, left.arr_size, &(right.arr_[0]),
        right.negative_, right.arr_size, &(mod.arr_[0]), mod.negative_,
        mod.arr_size, &(result.arr_[0]), result.arr_size);
    result.negative_ = BigintResultFlag::NEGATIVE & operator_result;
    result.overflow_ = BigintResultFlag::OVERFLOW & operator_result;
    return result;
  }

  // Function to get members
  constexpr bool Overflow() const { return overflow_; }

  constexpr bool Negative() const { return negative_; }
  constexpr void SetNegative(bool negative) { negative_ = negative; }
  const uint8_t *Values() const { return arr_.data(); }
  uint8_t *Values() { return arr_.data(); }
  constexpr std::array<uint8_t, arr_size> Value() const { return arr_; }

  size_t ValidBytes() const {
    size_t num = 0;
    if (arr_.size() <= 4) {
      for (auto a : arr_) {
        if (a != 0) {
          break;
        }
        ++num;
      }
      return arr_.size() - num;
    }
    size_t step = 4;
    for (size_t i = 0; i + step <= arr_.size(); i += step) {
      uint32_t val = *(uint32_t *)(arr_.data() + i);
      if (val != 0) {
        for (; i < arr_.size(); i++) {
          if (arr_[i] != 0) {
            break;
          }
          ++num;
        }
        break;
      }
      num += 4;
    }
    if (num + 1 < arr_.size() && arr_[num] != 0) {
      for (size_t i = num + 1; i < arr_.size(); i++) {
        if (arr_[i] != 0) {
          break;
        }
        ++num;
      }
    }
    return arr_.size() - num;
  }

 private:
  void Opposite() { negative_ = !negative_; }

  void Fill(uint8_t value) { arr_.fill(value); }

 public:
  bool negative_ = false;
  bool overflow_ = false;
  std::array<uint8_t, arr_size> arr_ = {};
};

// Type instance
template <size_t Bits>
using WideInt = WideInteger<Bits, true>;

template <size_t Bits>
using WideUint = WideInteger<Bits, false>;

using int256_t = WideInt<256>;
using uint256_t = WideUint<256>;

using int512_t = WideInt<512>;
using uint512_t = WideUint<512>;

template <size_t Bits, bool Signed>
string to_string(const WideInteger<Bits, Signed> &value) {
  const char *charmap = "0123456789";
  string result;
  WideInteger<Bits, Signed> helper = value;
  if (helper < 0) helper = WideInteger<Bits, Signed>(0) - helper;
  do {
    result += charmap[int(helper % 10)];
    helper /= 10;
  } while (helper != 0);
  reverse(result.begin(), result.end());

  if (value < 0) result = "-" + result;
  return result;
}

template <size_t Bits, bool Signed>
class numeric_limits<WideInteger<Bits, Signed>> {
  // static constexpr int Bits =256;
  //  static const bool Signed =false;
  typedef WideInteger<Bits, Signed> type;

 public:
  static const bool is_specialized = true;
  static type min() noexcept { return is_signed ? type(-1) << digits : 0; }
  static type max() noexcept {
    if (is_signed) {
      return ((type(1) << digits) - 1);
    } else {
      std::vector<uint8_t> value_array(Bits / 8, 0xFF);
      return type().FromBigEndian(value_array);
    }
  }
  static type lowest() _NOEXCEPT { return min(); }

  static constexpr int digits = static_cast<int>(Bits - Signed);
  static constexpr int digits10 = digits * 3 / 10;
  static constexpr int max_digits10 = 0;
  static constexpr bool is_signed = Signed;
  static constexpr bool is_integer = true;
  static constexpr bool is_exact = true;
  static constexpr int radix = 2;
  static type epsilon() _NOEXCEPT { return 0; }
  static type round_error() _NOEXCEPT { return 0; }

  static constexpr int min_exponent = 0;
  static constexpr int min_exponent10 = 0;
  static constexpr int max_exponent = 0;
  static constexpr int max_exponent10 = 0;

  static constexpr bool has_infinity = false;
  static constexpr bool has_quiet_NaN = false;
  static constexpr bool has_signaling_NaN = false;
  static const float_denorm_style has_denorm =
      float_denorm_style::denorm_absent;
  static constexpr bool has_denorm_loss = false;
  static type infinity() _NOEXCEPT { return 0; }
  static type quiet_NaN() _NOEXCEPT { return 0; }
  static type signaling_NaN() _NOEXCEPT { return 0; }
  static type denorm_min() _NOEXCEPT { return 0; }

  static constexpr bool is_iec559 = false;
  static constexpr bool is_bounded = true;
  static constexpr bool is_modulo = !Signed;

  static constexpr bool traps = true;
  static constexpr bool tinyness_before = false;
  static constexpr float_round_style round_style =
      float_round_style::round_toward_zero;
};
}  // namespace std

inline std::int256_t operator"" _int256(const char *str_value, size_t n) {
  std::int256_t result(str_value, n);
  return result;
}

inline std::int512_t operator"" _int512(const char *str_value, size_t n) {
  std::int512_t result(str_value, n);
  return result;
}

inline std::uint256_t operator"" _uint256(const char *str_value, size_t n) {
  std::uint256_t result(str_value, n);
  return result;
}

inline std::uint512_t operator"" _uint512(const char *str_value, size_t n) {
  std::uint512_t result(str_value, n);
  return result;
}
