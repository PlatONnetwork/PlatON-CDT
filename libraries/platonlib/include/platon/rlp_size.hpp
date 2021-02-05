#pragma once

#include <array>
#include <boost/fusion/include/for_each.hpp>
#include <iomanip>
#include <iosfwd>
#include <list>
#include <map>
#include <set>
#include <stack>
#include <unordered_set>
#include <vector>

#include "RLP.h"
#include "bigint.hpp"
#include "common.h"

namespace platon {

class RLPSize {
 public:
  enum ListFlag {
    Start,
    End,
  };

 public:
  explicit RLPSize(size_t init_size = 0) : size_(init_size), pending_() {}

  const size_t size() const { return size_; }

  template <class T>
  RLPSize& operator<<(T data) {
    return append(data);
  }

  RLPSize& operator<<(const bytes& s) { return append(s); }

  template <class T>
  RLPSize& operator<<(const std::vector<T>& s) {
    return append(s);
  }

  template <class T>
  RLPSize& operator<<(const std::list<T>& s) {
    return append(s);
  }

  template <class T, size_t N>
  RLPSize& operator<<(const std::array<T, N>& s) {
    return append(s);
  }

  template <class T>
  RLPSize& operator<<(const std::set<T>& s) {
    return append(s);
  }

  template <class T>
  RLPSize& operator<<(const std::unordered_set<T>& s) {
    return append(s);
  }

  template <class T, class U>
  RLPSize& operator<<(const std::map<T, U>& s) {
    return append(s);
  }

  template <typename... Args>
  RLPSize& operator<<(const std::tuple<Args...>& t) {
    return append(t);
  }

  RLPSize& operator<<(ListFlag flag) {
    if (flag == ListFlag::End) {
      assert(pending_.size() > 0);
      size_t list_size = pending_.top();
      pending_.pop();
      if (list_size < c_rlpListImmLenCount) {
        (list_size += 1);
      } else {
        list_size = list_size + bytesRequired(list_size) + 1;
      }

      if (pending_.size() > 0) {
        pending_.top() += list_size;
      } else {
        size_ += list_size;
      }
    } else {
      pending_.push(0);
    }
    return *this;
  }

  static ListFlag list_start() { return ListFlag::Start; }

  static ListFlag list_end() { return ListFlag::End; }

 private:
  RLPSize& append(bool b) {
    unsigned s = b ? 1 : 0;
    return append(bigint(s));
  }

  RLPSize& append(uint8_t s) { return append(bigint(s)); }

  RLPSize& append(uint16_t s) { return append(bigint(s)); }

  RLPSize& append(uint32_t s) { return append(bigint(s)); }

  RLPSize& append(uint64_t s) { return append(bigint(s)); }

  RLPSize& append(bigint s) {
    size_t size = 0;
    if (s == 0 || s < c_rlpDataImmLenStart) {
      size = 1;
    } else {
      size_t br = bytesRequired(s);
      if (br < c_rlpDataImmLenCount) {
        size = br + 1;
      } else {
        size = br + bytesRequired(br) + 1;
      }
    }

    if (pending_.size() > 0) {
      pending_.top() += size;
    } else {
      size_ += size;
    }
    return *this;
  }

  RLPSize& append(int8_t s) { return append(int128_t(s)); }

  RLPSize& append(int16_t s) { return append(int128_t(s)); }

  RLPSize& append(int c) { return append(int128_t(c)); }

  RLPSize& append(int32_t c) { return append(int128_t(c)); }

  RLPSize& append(int64_t c) { return append(int128_t(c)); }

  RLPSize& append(int128_t c) {
    uint128_t i = uint128_t((c << 1) ^ (c >> 127));
    return append(i);
  }

  RLPSize& append(float f) {
    uint32_t i = *reinterpret_cast<uint32_t*>(&f);
    return append(i);
  }

  RLPSize& append(double d) {
    uint64_t i = *reinterpret_cast<uint64_t*>(&d);
    return append(i);
  }

  template <size_t Bits, bool Signed>
  RLPSize& append(const std::WideInteger<Bits, Signed>& s) {
    if (!Signed) {
      size_ += s.ValidBytes();
    } else {
      size_t rsh = Bits - 1;
      std::WideInteger<Bits, Signed> r = (s << 1) ^ (s >> rsh);
      size_ += r.ValidBytes();
    }
    return *this;
  }

  template <
      typename T,
      typename = typename std::enable_if<
          std::is_same_v<T, bytes> || std::is_same_v<T, bytesConstRef> ||
          std::is_same_v<T, bytesRef> || std::is_same_v<T, std::string>>::type>
  RLPSize& append(const T& s) {
    size_t total = 0;
    if (s.empty()) {
      total = 1;
    } else {
      const byte* d = reinterpret_cast<const byte*>(s.data());
      size_t size = s.size();
      if (size == 1 && *d < c_rlpDataImmLenStart) {
        total = 1;
      } else if (size < c_rlpDataImmLenCount) {
        total = size + 1;
      } else {
        total = size + bytesRequired(size) + 1;
      }
    }

    if (pending_.size() > 0) {
      pending_.top() += total;
    } else {
      size_ += total;
    }
    return *this;
  }

  template <unsigned N>
  RLPSize& append(FixedHash<N> s) {
    size_t size = 0;
    if (!s) {
      size = 1;
    } else if (N < c_rlpDataImmLenCount) {
      size = N + 1;
    } else {
      size = N + bytesRequired(N) + 1;
    }

    if (pending_.size() > 0) {
      pending_.top() += size;
    } else {
      size_ += size;
    }
    return *this;
  }

  template <class T>
  RLPSize& append(const std::vector<T>& s) {
    if (s.empty()) {
      return AppendEmptyList();
    }

    *this << RLPSize::list_start();
    for (auto const& i : s) {
      *this << i;
    }
    *this << RLPSize::list_end();
    return *this;
  }

  template <class T, size_t N>
  RLPSize& append(const std::array<T, N>& s) {
    if (s.empty()) {
      return AppendEmptyList();
    }

    *this << RLPSize::list_start();
    for (const auto& i : s) {
      *this << i;
    }
    *this << RLPSize::list_end();
    return *this;
  }

  template <class T>
  RLPSize& append(const std::set<T>& s) {
    if (s.empty()) {
      return AppendEmptyList();
    }

    *this << RLPSize::list_start();
    for (const auto& i : s) {
      *this << i;
    }
    *this << RLPSize::list_end();
    return *this;
  }

  template <class T>
  RLPSize& append(const std::unordered_set<T>& s) {
    if (s.empty()) {
      return AppendEmptyList();
    }

    *this << RLPSize::list_start();
    for (const auto& i : s) {
      *this << i;
    }
    *this << RLPSize::list_end();
    return *this;
  }

  template <class T, class U>
  RLPSize& append(const std::pair<T, U>& s) {
    *this << RLPSize::list_start() << s.first << s.second
          << RLPSize::list_end();
    return *this;
  }

  template <class T, class U>
  RLPSize& append(const std::map<T, U>& s) {
    if (s.empty()) {
      return AppendEmptyList();
    }

    *this << RLPSize::list_start();
    for (const auto& i : s) {
      *this << i;
    }
    *this << RLPSize::list_end();
    return *this;
  }

  template <typename... Args>
  RLPSize& append(const std::tuple<Args...>& s) {
    *this << RLPSize::list_start();
    boost::fusion::for_each(s, [&](const auto& i) { *this << i; });
    *this << RLPSize::list_end();
    return *this;
  }

  template <class T>
  RLPSize& append(const std::list<T>& s) {
    if (s.empty()) {
      return AppendEmptyList();
    }
    *this << RLPSize::list_start();
    for (const auto& i : s) {
      *this << i;
    }
    *this << RLPSize::list_end();
    return *this;
  }

  RLPSize& AppendEmptyList() {
    if (pending_.size() > 0) {
      pending_.top()++;
    } else {
      size_ += 1;
    }
    return *this;
  }

 private:
  size_t size_;
  std::stack<size_t> pending_;
};

template <class T>
size_t pack_size(const T& t) {
  RLPSize rlps;
  rlps << t;
  return rlps.size();
}
}  // namespace platon
