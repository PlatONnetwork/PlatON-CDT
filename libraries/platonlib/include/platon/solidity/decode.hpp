#pragma once
#include "platon/assert.hpp"
#include "platon/bigint.hpp"
#include "platon/common.h"
#include "platon/solidity/common.hpp"
namespace platon {
namespace solidity {
namespace abi {
void PrettyPrint(const vector_ref<uint8_t const> &e) {
  for (size_t i = 0; i < e.size(); i += 32) {
    size_t end = i + 32 > e.size() ? e.size() - i : 32;
    println(toHex(e.cropped(i, end)));
  }
}
void PrettyPrint(const std::string &name, const vector_ref<uint8_t const> &e) {
  println(name, "[", e.size(), "]");
  for (size_t i = 0; i < e.size(); i += 32) {
    size_t end = i + 32 > e.size() ? e.size() - i : 32;
    println(toHex(e.cropped(i, end)));
  }
}
template <typename T, std::size_t size>
bytesConstRef DecodeElement(std::array<T, size> &v, const bytesConstRef &input);

template <typename... Types>
bytesConstRef DecodeElement(std::tuple<Types...> &v,
                            const bytesConstRef &input);
size_t ReadLength(const bytesConstRef &input) {
  std::uint256_t length(input.cropped(0, 32), true);
  platon_assert(length < std::numeric_limits<size_t>::max());
  return size_t(length);
}

// decode integer
template <typename T,
          typename std::enable_if_t<
              std::numeric_limits<std::decay_t<T>>::is_integer, bool> = true>
bytesConstRef DecodeElement(T &t, const bytesConstRef &data) {
  auto buf = data.cropped(0, 32);
  if (!std::numeric_limits<std::decay_t<T>>::is_signed ||
      (std::numeric_limits<std::decay_t<T>>::is_signed && !(buf[0] & 0x80))) {
    std::uint256_t num(buf, true);
    t = T(num);
  } else {
    std::int256_t num;
    num.FromBigEndian(buf);
    num = -num;
    num += 1;
    u128 *n = (u128 *)num.Values();
    *n = ~*n;
    *n = ~*++n;
    t = T(num);
  }

  return data.cropped(32);
}

bytesConstRef DecodeElement(bool &t, const bytesConstRef &data) {
  t = data[31];
  return data.cropped(32);
}
bytesConstRef DecodeElement(std::string &t, const bytesConstRef &data) {
  size_t length = ReadLength(data);
  t.resize(length);
  memcpy(t.data(), data.begin() + 32, length);
  return data.cropped(32 + length);
}
bytesConstRef DecodeElement(std::vector<uint8_t> &t,
                            const bytesConstRef &data) {
  size_t length = ReadLength(data);
  t.resize(length);
  memcpy(t.data(), data.begin() + 32, length);
  return data.cropped(32 + length);
}

template <std::size_t size>
bytesConstRef DecodeElement(std::array<uint8_t, size> &t,
                            const bytesConstRef &data) {
  static_assert(size <= 32);
  memcpy(t.data(), data.begin() + 32 - size, size);
  return data.cropped(32);
}

bytesConstRef DecodeElement(h256 &t, const bytesConstRef &data) {
  memcpy(t.data(), data.begin(), t.size);
  return data.cropped(32);
}

bytesConstRef DecodeElement(Address &t, const bytesConstRef &data) {
  memcpy(t.data(), data.begin() + 32 - t.size, t.size);
  return data.cropped(32);
}
template <typename T,
          typename std::enable_if_t<!std::is_same_v<T, uint8_t>, bool> = true>
bytesConstRef DecodeElement(std::vector<T> &v, const bytesConstRef &input) {
  auto data = input;
  size_t size = ReadLength(data);
  data.retarget(data.begin() + 32, data.size() - 32);
  v.resize(size);

  auto origin = data;
  size_t origin_len = origin.size();
  bool dynamic = IsDynamicType(T());
  for (size_t i = 0; i < size; i++) {
    auto entry = data;
    if (dynamic) {
      size_t offset = ReadLength(data);
      platon_assert(offset <= origin_len);
      entry = origin.cropped(offset);
    }
    auto tail = DecodeElement(v[i], entry);
    if (!dynamic) {
      data = tail;
    } else {
      data.retarget(data.begin() + 32, data.size() - 32);
    }
  }
  return data;
}
template <typename... Types>
bytesConstRef DecodeElement(std::tuple<Types...> &v,
                            const bytesConstRef &input) {
  auto data = input;
  auto origin = data;
  size_t origin_len = data.size();
  auto decode_element = [&](auto &t) {
    auto entry = data;
    bool dynamic = IsDynamicType(t);
    if (dynamic) {
      size_t offset = ReadLength(data);
      platon_assert(offset <= origin_len);
      entry = origin.cropped(offset);
    }
    auto tail = DecodeElement(t, entry);
    if (!dynamic) {
      data = tail;
    } else {
      data.retarget(data.begin() + 32, data.size() - 32);
    }
  };
  std::apply([&](auto &&... args) { (decode_element(args), ...); }, v);
  return data;
}

template <typename T, typename std::enable_if_t<SupportSolV<T>, bool> = true>
bytesConstRef DecodeElement(T &v, const bytesConstRef &input) {
  auto data = input;
  auto origin = data;
  size_t origin_len = data.size();
  auto decode_element = [&](const auto &t) {
    auto entry = data;
    bool dynamic = IsDynamicType(t);
    if (dynamic) {
      size_t offset = ReadLength(data);
      platon_assert(offset <= origin_len);
      entry = origin.cropped(offset);
    }
    auto tail =
        DecodeElement(const_cast<std::decay_t<decltype(t)> &>(t), entry);
    if (!dynamic) {
      data = tail;
    } else {
      data.retarget(data.begin() + 32, data.size() - 32);
    }
  };
  v.ForEachElement(decode_element);
  return data;
}

template <typename T, std::size_t size>
bytesConstRef DecodeElement(std::array<T, size> &v,
                            const bytesConstRef &input) {
  auto data = input;
  auto origin = data;
  size_t origin_len = origin.size();
  bool dynamic = IsDynamicType(v[0]);
  for (size_t i = 0; i < size; i++) {
    auto entry = data;
    if (dynamic) {
      size_t offset = ReadLength(data);
      platon_assert(offset <= origin_len);
      entry = origin.cropped(offset);
    }
    auto tail = DecodeElement(v[i], entry);
    if (!dynamic) {
      data = tail;
    } else {
      data.retarget(data.begin() + 32, data.size() - 32);
    }
  }
  return data;
}

template <typename T>
void Decode(T &t, const bytesConstRef &input) {
  DecodeElement(t, input);
};
}  // namespace abi
}  // namespace solidity
}  // namespace platon