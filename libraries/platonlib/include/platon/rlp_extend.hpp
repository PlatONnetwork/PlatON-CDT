#pragma once

#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/include/std_tuple.hpp>
#include "boost/fusion/algorithm/iteration/for_each.hpp"
#include "boost/preprocessor/seq/for_each.hpp"

#include <boost/mp11/tuple.hpp>

#include <tuple>
#include "platon/RLP.h"
#include "panic.hpp"

namespace platon {

inline RLPStream& RLPStream::operator<<(bytes const& _s) {
  append(_s);
  return *this;
}

template <class _T>
inline RLPStream& RLPStream::operator<<(std::vector<_T> const& _s) {
  appendList(_s.size());
  for (auto const& i : _s) {
    *this << i;
  }
  return *this;
}

template <class _T>
inline RLPStream& RLPStream::operator<<(std::list<_T> const& _s) {
  appendList(_s.size());
  for (auto const& i : _s) {
    *this << i;
  }
  return *this;
}

template <class _T, size_t S>
inline RLPStream& RLPStream::operator<<(std::array<_T, S> const& _s) {
  appendList(_s.size());
  for (auto const& i : _s) {
    *this << i;
  }
  return *this;
}

template <class _T>
inline RLPStream& RLPStream::operator<<(std::set<_T> const& _s) {
  appendList(_s.size());
  for (auto const& i : _s) {
    *this << i;
  }
  return *this;
}

template <class _T>
inline RLPStream& RLPStream::operator<<(std::unordered_set<_T> const& _s) {
  appendList(_s.size());
  for (auto const& i : _s) {
    *this << i;
  }
  return *this;
}

template <class T, class U>
inline RLPStream& RLPStream::operator<<(std::pair<T, U> const& _s) {
  appendList(2);
  *this << _s.first;
  *this << _s.second;
  return *this;
}

template <class T, class U>
inline RLPStream& RLPStream::operator<<(std::map<T, U> const& _s) {
  appendList(_s.size());
  for (auto const& i : _s) {
    *this << i;
  }
  return *this;
}

template <typename... Args>
inline RLPStream& RLPStream::operator<<(const std::tuple<Args...>& t) {
  size_t num = sizeof...(Args);
  appendList(num);
  boost::fusion::for_each(t, [&](const auto& i) { *this << i; });
  return *this;
}

// get data from the RLP instance
template <class T>
inline void fetch(const RLP& rlp, T& value) {
  value = rlp.operator T();
}

inline void fetch(const RLP& rlp, bool& value) { value = rlp.toBool(); }

inline void fetch(const RLP& rlp, bytes& value) { value = rlp.toBytes(); }

inline void fetch(const RLP& rlp, bytesConstRef& value) { value = rlp.toBytesConstRef(); }


template <class T>
inline void fetch(const RLP& rlp, std::vector<T>& ret) {
  if (rlp.isList()) {
    ret.reserve(rlp.itemCount());
    for (auto const& i : rlp) {
      T one;
      fetch(i, one);
      ret.emplace_back(std::move(one));
    }
  } else {
    internal::platon_throw("bad cast");
  }
}

template <class T>
inline void fetch(const RLP& rlp, std::list<T>& ret) {
  if (rlp.isList()) {
    for (auto const& i : rlp) {
      T one;
      fetch(i, one);
      ret.emplace_back(std::move(one));
    }
  } else {
    internal::platon_throw("bad cast");
  }
}

template <class T>
inline void fetch(const RLP& rlp, std::set<T>& ret) {
  if (rlp.isList()) {
    for (auto const& i : rlp) {
      T one;
      fetch(i, one);
      ret.insert(std::move(one));
    }
  } else {
    internal::platon_throw("bad cast");
  }
}

template <class T>
inline void fetch(const RLP& rlp, std::unordered_set<T>& ret) {
  if (rlp.isList()) {
    for (auto const& i : rlp) {
      T one;
      fetch(i, one);
      ret.insert(std::move(one));
    }
  } else {
    internal::platon_throw("bad cast");
  }
}

template <class T, class U>
inline void fetch(const RLP& rlp, std::pair<T, U>& ret) {
  if (rlp.itemCountStrict() != 2) {
    internal::platon_throw("bad cast");
  }
  T one;
  fetch(rlp[0], one);
  ret.first = std::move(one);
  U two;
  fetch(rlp[1], two);
  ret.second = std::move(two);
}

template <class T, size_t N>
inline void fetch(const RLP& rlp, std::array<T, N>& ret) {
  if (rlp.itemCountStrict() != N) {
    internal::platon_throw("bad cast");
  }
  for (size_t i = 0; i < N; ++i) {
    T one;
    fetch(rlp[i], one);
    ret[i] = std::move(one);
  }
}

template <class T, class U>
inline void fetch(const RLP& rlp, std::map<T, U>& ret) {
  if (rlp.isList()) {
    for (auto const& i : rlp) {
      std::pair<T, U> one;
      fetch(i, one);
      ret.insert(std::move(one));
    }
  } else {
    internal::platon_throw("bad cast");
  }
}

template <typename... Args>
inline void fetch(const RLP& rlp, std::tuple<Args...>& t) {
  int vect_index = 0;
  boost::fusion::for_each(t, [&](auto& i) {
    fetch(rlp[vect_index], i);
    vect_index++;
  });
}

}  // namespace platon
