//
// Created by zhou.yang on 2018/11/16.
//

#pragma once
#include "platon/name.hpp"
#include "platon/storage.hpp"

#include <string>

#include <array>
#include <deque>
#include <map>
#include <set>
#include <tuple>
#include <vector>

namespace platon {

/**
 * @brief Basic type package
 *
 * @tparam *Name Element value name, in the same contract, the name needs to be
 * unique
 * @tparam T Element type
 */
template <Name::Raw StorageName, typename T>
class StorageType {
 public:
  /**
   * @brief Construct a new Storage Type object
   *
   */
  StorageType() { Init(); }

  /**
   * @brief Construct a new Storage Type object
   *
   * @param d Element
   */
  StorageType(const T &d) : default_(d) { Init(); }

  StorageType(const StorageType<StorageName, T> &) = delete;
  StorageType(const StorageType<StorageName, T> &&) = delete;
  /**
   * @brief Destroy the Storage Type object. Refresh to blockchain
   *
   */
  ~StorageType() { Flush(); }

  T &operator=(const T &t) { t_ = t; }

  template <typename P>
  bool operator==(const P &t) const {
    return t_ == t;
  }
  template <typename P>
  bool operator!=(const P &t) const {
    return t_ == t;
  }
  template <typename P>
  bool operator<(const P &t) const {
    return t_ < t;
  }
  template <typename P>
  bool operator>=(const P &t) const {
    return t_ >= t;
  }
  template <typename P>
  bool operator<=(const P &t) const {
    return t_ <= t;
  }
  template <typename P>
  bool operator>(const P &t) const {
    return t_ > t;
  }

  template <typename P>
  T &operator^=(const P &t) const {
    t_ ^= t;
    return t_;
  }
  template <typename P>
  T operator^(const P &t) const {
    return t_ ^ t;
  }
  template <typename P>
  T &operator|=(const P &t) const {
    t_ |= t;
    return t_;
  }
  template <typename P>
  T operator|(const P &t) const {
    return t_ | t;
  }
  template <typename P>
  T &operator&=(const P &t) const {
    t_ &= t;
    return t_;
  }
  template <typename P>
  T operator&(const P &t) const {
    return t_ & t;
  }

  T operator~() const { return ~t_; }

  T &operator<<(int offset) {
    t_ << offset;
    return t_;
  }
  T &operator>>(int offset) {
    t_ >> offset;
    return t_;
  }

  T &operator++() { return ++t_; }
  T operator++(int) { return ++t_; }

  T &operator[](int i) { return t_[i]; }
  template <typename P>
  T &operator+=(const P &p) {
    t_ += p;
    return t_;
  }
  template <typename P>
  T &operator-=(const P &p) {
    t_ -= p;
    return t_;
  }
  T &operator*() { return t_; }
  T *operator->() { return &t_; }

  operator bool() const { return t_ ? true : false; }

  T get() const { return t_; }
  T &self() { return t_; }

 private:
  /**
   * @brief Load from blockchain
   *
   */
  void Init() {
    if (get_state(name_, t_) == 0) {
      t_ = default_;
    }
  }
  /**
   * @brief Refresh to blockchain
   *
   */
  void Flush() { set_state(name_, t_); }
  T default_;
  const std::string name_ =
      std::string("_s_") + std::to_string(uint64_t(StorageName));
  T t_;
};

template <Name::Raw name>
using Uint8 = class StorageType<name, uint8_t>;

template <Name::Raw name>
using Int8 = class StorageType<name, int8_t>;

template <Name::Raw name>
using Uint16 = class StorageType<name, uint16_t>;

template <Name::Raw name>
using Int16 = class StorageType<name, int16_t>;

template <Name::Raw name>
using Uint = class StorageType<name, uint32_t>;

template <Name::Raw name>
using Int = class StorageType<name, int32_t>;

template <Name::Raw name>
using Uint64 = class StorageType<name, uint64_t>;

template <Name::Raw name>
using Int64 = class StorageType<name, int64_t>;

template <Name::Raw name>
using Float = class StorageType<name, float>;

template <Name::Raw name>
using Double = class StorageType<name, double>;

template <Name::Raw name>
using String = class StorageType<name, std::string>;

template <Name::Raw name, typename T>
using Vector = class StorageType<name, std::vector<T>>;

template <Name::Raw name, typename T>
using Set = class StorageType<name, std::set<T>>;

template <Name::Raw name, typename K, typename V>
using Map = class StorageType<name, std::map<K, V>>;

template <Name::Raw name, typename T, size_t N>
using Array = class StorageType<name, std::array<T, N>>;

template <Name::Raw name, typename... Types>
using Tuple = class StorageType<name, std::tuple<Types...>>;

template <Name::Raw name, typename T>
using Deque = class StorageType<name, std::deque<T>>;

//    template <Name::Raw name,  typename T>
//    using Queue = class StorageType<name, std::queue<T>>;
//
//
//    template <Name::Raw name,  typename T>
//    using Stack = class StorageType<name, std::stack<T>>;

//    template <Name::Raw name,  typename T>
//    using List = class StorageType<name, std::list<T>>;
//
//    template <Name::Raw name,  typename K, typename V>
//    using UnorderedMap = class StorageType<name, std::unordered_map<K, V>>;
//
//    template <Name::Raw name, typename T>
//    using UnorderedSet = class StorageType<name, std::unordered_set<T>>;

}  // namespace platon
//    example
//    Double<name> dou;
//    dou = 2.4;
//    dou += 1.2;
