#pragma once
#include <boost/hana.hpp>
#include <tuple>
#include "platon/bigint.hpp"
#include "platon/bytes_buffer.hpp"
#include "platon/fixedhash.hpp"
#include "platon/solidity/common.hpp"
namespace platon {
namespace solidity {
namespace abi {

class SolEncode {
 public:
  SolEncode() {}

  SolEncode(size_t len, size_t reserved = 0) {
    out_.resize(len);
    out_.relocate(reserved);
  }

  // unsigned
  template <typename T,
            typename std::enable_if_t<
                std::numeric_limits<std::decay_t<T>>::is_integer, bool> = true>
  void Encode(const T &v) {
    if (!std::numeric_limits<std::decay_t<T>>::is_signed) {
      std::uint256_t num = v;
      out_.append(num.Values(), num.Values() + num.arr_size);
    } else {
      std::int256_t num = v;
      if (num < 0) {
        u128 *n = (u128 *)num.Values();
        *n = ~*n;
        *n = ~*++n;
        num -= 1;
      }
      out_.append(num.Values(), num.Values() + num.arr_size);
    }
  }
  void Encode(const std::tuple<> &v) {}
  // bool
  void Encode(bool v) { Encode(uint8_t(v)); }

  // bytes
  void Encode(const std::vector<uint8_t> &v) {
    Encode(v.size());
    RightPad32(v);
  }
  // std::string
  void Encode(const std::string &v) {
    Encode(v.size());
    size_t len = (v.size() + 31) / 32 * 32;
    out_.append((const uint8_t *)v.data(),
                (const uint8_t *)v.data() + v.size());
    out_.append(zeros.data(), zeros.data() + len - v.size());
  }

  //<type>[]
  template <typename T,
            typename std::enable_if_t<!std::is_same_v<T, uint8_t>, bool> = true>
  void Encode(const std::vector<T> &v) {
    Encode(v.size());
    // dynamic
    bool dynamic = !v.empty() & IsDynamicType(v[0]);
    size_t offset = dynamic ? GetTypeSize(v[0]) * v.size() : 0;
    size_t origin = out_.size();
    for (size_t i = 0; i < v.size(); i++) {
      if (!dynamic) {
        Encode(v[i]);
      } else {
        out_.relocate(origin + i * 32);
        Encode(offset);
        size_t start = origin + offset;
        out_.relocate(start);
        Encode(v[i]);
        size_t end = out_.size();
        offset += end - start;
      }
    }
    if (dynamic) {
      out_.relocate(origin + offset);
    }
  }

  // bytes<M>
  template <std::size_t size>
  void Encode(const std::array<uint8_t, size> &v) {
    static_assert(size <= 32);
    out_.append(v.data(), v.data() + v.size());
    out_.append(zeros.data(), zeros.data() + 32 - v.size());
  }

  //<type>[M]
  template <typename T, std::size_t size>
  void Encode(const std::array<T, size> &v) {
    // dynamic
    bool dynamic = IsDynamicType(v[0]);
    size_t offset = dynamic ? GetTypeSize(v[0]) * v.size() : 0;
    size_t origin = out_.size();
    for (size_t i = 0; i < v.size(); i++) {
      if (!dynamic) {
        Encode(v[i]);
      } else {
        out_.relocate(origin + i * 32);
        Encode(offset);
        size_t start = origin + offset;
        out_.relocate(start);
        Encode(v[i]);
        size_t end = out_.size();
        offset += end - start;
      }
    }
    if (dynamic) {
      out_.relocate(origin + offset);
    }
  }

  // tuple<T1,...,Tn>
  template <typename... Types>
  void Encode(const std::tuple<Types...> &v) {
    size_t offset = ElementsTypeTotalSize(v);
    size_t origin = out_.size();
    size_t dynamic_origin = origin;
    size_t i = 0;

    auto encode_element = [&](auto t) {
      bool dynamic = IsDynamicType(t);
      if (!dynamic) {
        size_t end = out_.size();
        out_.relocate(dynamic_origin);
        Encode(t);
        dynamic_origin = out_.size();
        if (out_.size() < end) {
          out_.relocate(end);
        }
      } else {
        out_.relocate(dynamic_origin);
        Encode(offset);
        size_t start = origin + offset;
        out_.relocate(start);
        Encode(t);
        size_t end = out_.size();
        offset += end - start;
        dynamic_origin += 32;
      }
    };
    std::apply([&](auto &&... args) { (encode_element(args), ...); }, v);
    out_.relocate(origin + offset);
  }

  // tuple<T1,...,Tn>
  template <typename T, typename std::enable_if_t<SupportSolV<T>, bool> = true>
  void Encode(const T &v) {
    size_t offset = ElementsTypeTotalSize(v);
    size_t origin = out_.size();
    size_t dynamic_origin = origin;
    size_t i = 0;
    auto encode_element = [&](auto t) {
      bool dynamic = IsDynamicType(t);
      if (!dynamic) {
        size_t end = out_.size();
        out_.relocate(dynamic_origin);
        Encode(t);
        dynamic_origin = out_.size();
        if (out_.size() < end) {
          out_.relocate(end);
        }
      } else {
        out_.relocate(dynamic_origin);
        Encode(offset);
        size_t start = origin + offset;
        out_.relocate(start);
        Encode(t);
        size_t end = out_.size();
        offset += end - start;
        dynamic_origin += 32;
      }
    };
    v.ForEachElement(encode_element);
    out_.relocate(origin + offset);
  }

  // address
  void Encode(const Address &v) {
    out_.append(zeros.data(), zeros.data() + (32 - v.size));
    out_.append(v.data(), v.data() + v.size);
  }

  // h256
  void Encode(const h256 &v) { out_.append(v.data(), v.data() + v.size); }

  template <typename T>
  SolEncode &operator<<(const T &v) {
    Encode(v);
    return *this;
  }

  const BytesBuffer &Out() const { return out_; }
  BytesBuffer &&Invalidate() { return std::move(out_); }
  void Reset(size_t reserve) {
    out_.clear();
    out_.reserve(reserve);
  }

 private:
  void LeftPad32(const std::vector<uint8_t> &v) {
    size_t len = (v.size() + 31) / 32 * 32;
    out_.append(zeros.data(), zeros.data() + len - v.size());
    out_.append(v.data(), zeros.data() + v.size());
  }

  void RightPad32(const std::vector<uint8_t> &v) {
    size_t len = (v.size() + 31) / 32 * 32;
    out_.append(v.data(), v.data() + v.size());
    out_.append(zeros.data(), zeros.data() + len - v.size());
  }

  BytesBuffer out_;
  static constexpr std::array<uint8_t, 32> zeros = {
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
};
}  // namespace abi
}  // namespace solidity
}  // namespace platon
