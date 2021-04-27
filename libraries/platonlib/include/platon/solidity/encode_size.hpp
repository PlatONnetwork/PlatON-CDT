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
class SolEncodeSize {
 public:
  SolEncodeSize() : size_(0) {}

  size_t Size() const { return size_; }
  // unsigned
  template <typename T,
            typename std::enable_if_t<
                std::numeric_limits<std::decay_t<T>>::is_integer, bool> = true>
  void Encode(const T &v) {
    size_ += 32;
  }

  // bool
  void Encode(bool v) { Encode(uint8_t(v)); }

  // bytes
  void Encode(const std::vector<uint8_t> &v) {
    Encode(v.size());
    RightPad32(v);
  }
  void Encode(const std::tuple<> &v) {}
  // std::string
  void Encode(const std::string &v) {
    Encode(v.size());
    size_ += (v.size() + 31) / 32 * 32;
  }
  //<type>[]
  template <typename T,
            typename std::enable_if_t<!std::is_same_v<T, uint8_t>, bool> = true>
  void Encode(const std::vector<T> &v) {
    Encode(v.size());
    // dynamic
    bool dynamic = !v.empty() & IsDynamicType(v[0]);
    size_t offset = dynamic ? GetTypeSize(v[0]) * v.size() : 0;
    for (size_t i = 0; i < v.size(); i++) {
      if (!dynamic) {
        Encode(v[i]);
      } else {
        Encode(offset);
        Encode(v[i]);
      }
    }
  }

  // bytes<M>
  template <std::size_t size>
  void Encode(const std::array<uint8_t, size> &v) {
    static_assert(size <= 32);
    size_ += 32;
  }

  //<type>[M]
  template <typename T, std::size_t size>
  void Encode(const std::array<T, size> &v) {
    // dynamic
    bool dynamic = IsDynamicType(v[0]);
    size_t offset = dynamic ? GetTypeSize(v[0]) * v.size() : 0;
    for (size_t i = 0; i < v.size(); i++) {
      if (!dynamic) {
        Encode(v[i]);
      } else {
        Encode(offset);
        Encode(v[i]);
      }
    }
  }

  // tuple<T1,...,Tn>
  template <typename... Types>
  void Encode(const std::tuple<Types...> &v) {
    size_t offset = 0;
    auto encode_element = [&](auto t) {
      bool dynamic = IsDynamicType(t);
      if (!dynamic) {
        Encode(t);
      } else {
        Encode(offset);
        Encode(t);
      }
    };
    std::apply([&](auto &&... args) { (encode_element(args), ...); }, v);
  }

  // tuple<T1,...,Tn>
  template <typename T, typename std::enable_if_t<SupportSolV<T>, bool> = true>
  void Encode(const T &v) {
    size_t offset = 0;
    auto encode_element = [&](auto t) {
      bool dynamic = IsDynamicType(t);
      if (!dynamic) {
        Encode(t);
      } else {
        Encode(offset);
        Encode(t);
      }
    };
    v.ForEachElement(encode_element);
  }

  // address
  void Encode(const Address &v) { size_ += 32; }

  // h256
  void Encode(const h256 &v) { size_ += 32; }

  template <typename T>
  SolEncodeSize &operator<<(const T &v) {
    Encode(v);
    return *this;
  }

  void LeftPad32(const std::vector<uint8_t> &v) {
    size_t len = (v.size() + 31) / 32 * 32;
    size_ += len;
  }

  void RightPad32(const std::vector<uint8_t> &v) {
    size_t len = (v.size() + 31) / 32 * 32;
    size_ += len;
  }

  void Reset() { size_ = 0; }

 private:
  size_t size_;
};
}  // namespace abi
}  // namespace solidity
}  // namespace platon
