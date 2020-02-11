#pragma once

#include <map>
#include "platon/assert.hpp"
#include "platon/name.hpp"
#include "platon/storage.hpp"

namespace platon {
namespace db {
/**
 * @brief Implement Array function
 *
 * @tparam *Name Array name, in the same contract, the name should be unique
 * @tparam Key Array element type
 * @tparam Size Array length
 */
template <Name::Raw TableName, typename Key, unsigned N>
class Array {
 public:
  /**
   * @brief Iterator
   *
   */
  class iterator : public std::iterator<std::bidirectional_iterator_tag, Key> {
   public:
    friend bool operator==(const iterator& a, const iterator& b) {
      return a.array_ == b.array_ && a.pos_ = b.pos_;
    }
    friend bool operator!=(const iterator& a, const iterator& b) {
      return a.array_ != b.array_ || a.pos_ != b.pos_;
    }

   public:
    /**
     * @brief Construct a new iterator object
     *
     * @param array Array
     * @param pos position
     */
    iterator(Array<TableName, Key, N>* array, size_t pos)
        : array_(array), pos_(pos) {}

    /**
     * @brief Get the element value
     *
     * @return Key&
     */
    Key& operator*() const { return array_->at(pos_ - 1); }

    /**
     * @brief Get the element value
     *
     * @return Key&
     */
    Key& operator->() const { return array_->at(pos_ - 1); }

    iterator& operator--() {
      pos_--;
      return *this;
    }

    iterator operator--(int) {
      platon_assert(pos_ > 0, "pos can't be negative");
      iterator tmp(array_, pos_--);
      --tmp;
      return tmp;
    }

    iterator& operator++() {
      pos_++;
      return *this;
    }

    iterator operator++(int) {
      iterator tmp(array_, pos_++);
      ++tmp;
      return tmp;
    }

   private:
    Array<TableName, Key, N>* array_;
    size_t pos_;
  };

  /**
   * @brief Constant iterator
   *
   */
  class const_iterator
      : public std::iterator<std::bidirectional_iterator_tag, const Key> {
   public:
    friend bool operator==(const const_iterator& a, const const_iterator& b) {
      return a.array_ == b.array_ && a.pos_ = b.pos_;
    }
    friend bool operator!=(const const_iterator& a, const const_iterator& b) {
      return a.array_ != b.array_ || a.pos_ != b.pos_;
    }

   public:
    /**
     * @brief Construct a new Const iterator object
     *
     * @param array Array
     * @param pos position
     */
    const_iterator(Array<TableName, Key, N>* array, size_t pos)
        : array_(array), pos_(pos) {}

    /**
     * @brief Get the element value
     *
     * @return Key&
     */
    const Key& operator*() {
      key_ = array_->get_const(pos_ - 1);
      return key_;
    }

    /**
     * @brief Get the element value
     *
     * @return Key&
     */
    const Key& operator->() {
      key_ = array_->get_const(pos_ - 1);
      return key_;
    }

    const_iterator& operator--() {
      pos_--;
      return *this;
    }

    const_iterator operator--(int) {
      platon_assert(pos_ > 0, "pos can't be negative");
      const_iterator tmp(array_, pos_--);
      --tmp;
      return tmp;
    }

    const_iterator& operator++() {
      pos_++;
      return *this;
    }

    const_iterator operator++(int) {
      const_iterator tmp(array_, pos_++);
      ++tmp;
      return tmp;
    }

   private:
    Array<TableName, Key, N>* array_;
    size_t pos_;
    Key key_;
  };

  /**
   * @brief Constant reverse iterator
   *
   */
  class const_reverse_iterator
      : public std::iterator<std::bidirectional_iterator_tag, const Key> {
   public:
    friend bool operator==(const const_reverse_iterator& a,
                           const const_reverse_iterator& b) {
      return a.array_ == b.array_ && a.pos_ = b.pos_;
    }
    friend bool operator!=(const const_reverse_iterator& a,
                           const const_reverse_iterator& b) {
      return a.array_ != b.array_ || a.pos_ != b.pos_;
    }

   public:
    /**
     * @brief Construct a new Const Reverse iterator object
     *
     * @param array Array
     * @param pos position
     */
    const_reverse_iterator(Array<TableName, Key, N>* array, size_t pos)
        : array_(array), pos_(pos) {}

    /**
     * @brief Get the element value
     *
     * @return Key&
     */
    const Key& operator*() {
      key_ = array_->get_const(pos_ - 1);
      return key_;
    }

    /**
     * @brief Get the element value
     *
     * @return Key&
     */
    const Key& operator->() {
      key_ = array_->get_const(pos_ - 1);
      return key_;
    }

    const_reverse_iterator& operator--() {
      pos_++;
      return *this;
    }

    const_reverse_iterator operator--(int) {
      platon_assert(pos_ > 0, "pos can't be negative");
      const_reverse_iterator tmp(array_, pos_++);
      ++tmp;
      return tmp;
    }

    const_reverse_iterator& operator++() {
      pos_--;
      return *this;
    }

    const_reverse_iterator operator++(int) {
      const_reverse_iterator tmp(array_, pos_--);
      --tmp;
      return tmp;
    }

   private:
    Array<TableName, Key, N>* array_;
    size_t pos_;
    Key key_;
  };

  typedef std::reverse_iterator<iterator> reverse_iterator;

 public:
  static_assert(N != 0, "array no support size = 0");
  Array() {}

  Array(const Array<TableName, Key, N>&) = delete;
  Array(const Array<TableName, Key, N>&&) = delete;
  Array<TableName, Key, N>& operator=(const Array<TableName, Key, N>&) = delete;

  ~Array() { Flush(); }

  /**
   * @brief iterator start position
   *
   * @return iterator
   *
   * Example:
   *
   * @code
   * typedef platon::db::Array<"name_test"_n, std::string, 3> ArrayName;
   * ArrayName name;
   * name[0] = "hello";
   * auto iter = name.begin();
   * assert(*iter == "hello");
   * @endcode
   */
  iterator begin() { return iterator(this, 1); }

  /**
   * @brief iterator end position
   *
   * @return iterator
   * Example:
   *
   * @code
   * typedef platon::db::Array<"name_test"_n, std::string, 3> ArrayName;
   * ArrayName name;
   * name[0] = "hello";
   * name[1] = "world";
   * auto iter = name.begin();
   * for (auto iter = name.begin(); iter != name.end(); iter++) {
   * }
   * @endcode
   */
  iterator end() { return iterator(this, N + 1); }

  /**
   * @brief Reverse iterator start position
   *
   * @return reverse_iterator
   */
  reverse_iterator rbegin() { return reverse_iterator(end()); }

  /**
   * @brief Reverse iterator end position
   *
   * @return reverse_iterator
   */
  reverse_iterator rend() { return reverse_iterator(begin()); }

  /**
   * @brief Constant iterator start position
   *
   * @return const_iterator
   */
  const_iterator cbegin() { return const_iterator(this, 1); }

  /**
   * @brief Constant iterator end position
   *
   * @return const_iterator
   */
  const_iterator cend() { return const_iterator(this, N + 1); }

  /**
   * @brief Inverse constant iterator start position
   *
   * @return const_reverse_iterator
   */
  const_reverse_iterator crbegin() { return const_reverse_iterator(this, N); }

  /**
   * @brief Inverse constant iterator end position
   *
   * @return const_reverse_iterator
   */
  const_reverse_iterator crend() { return const_reverse_iterator(this, 0); }

  /**
   * @brief Get the specified position element
   *
   * @param pos Element position
   * @return Key& Element value
   * Example:
   *
   * @code
   * typedef platon::db::Array<"name_test"_n, std::string, 3> ArrayName;
   * ArrayName name;
   * name[0] = "hello";
   * assert(name.at[0] = "hello");
   * @endcode
   */
  Key& at(size_t pos) {
    platon_assert(pos < N, "out of range pos:", pos, "size:", N);
    auto iter = cache_.find(pos);
    if (iter != cache_.end()) {
      return iter->second;
    }
    Key key;
    get_state(EncodeKey(pos), key);
    auto iterc = cache_.emplace(std::make_pair(pos, key));
    platon_assert(iterc.second, "cache emplace failed");
    return iterc.first->second;
  }

  /**
   * @brief Bracket operator
   *
   * @param pos position
   * @return Key& element
   * Example:
   *
   * @code
   * typedef platon::db::Array<"name_test"_n, std::string, 3> ArrayName;
   * ArrayName name;
   * name[0] = "hello";
   * @endcode
   */
  Key& operator[](size_t pos) { return at(pos); }

  /**
   * @brief array size
   *
   * @return size_t
   * Example:
   *
   * @code
   * typedef platon::db::Array<"name_test"_n, std::string, 3> ArrayName;
   * ArrayName name;
   * assert(name.size() == 3);
   * @endcode
   */
  size_t size() { return N; }

  /**
   * @brief Get the Const object. Do not flush to cache
   *
   * @param pos position
   * @return Key Element value
   * Example:
   *
   * @code
   * typedef platon::db::Array<"name_test"_n, std::string, 3> ArrayName;
   * ArrayName name;
   * name[0] = "hello";
   * assert(name.get_const(0) == "hello");
   * @endcode
   */
  Key get_const(size_t pos) {
    auto iter = cache_.find(pos);
    if (iter != cache_.end()) {
      return iter->second;
    }
    Key key;
    get_state(EncodeKey(pos), key);
    return key;
  }

  /**
   * @brief Set the Const object, Do not flush to cache
   *
   * @param pos
   * @param key
   * Example:
   *
   * @code
   * typedef platon::db::Array<"name_test"_n, std::string, 3> ArrayName;
   * ArrayName name;
   * name[0] = "hello";
   * name.set_const(0, "world");
   * assert(name.get_const(0) == "world");
   * @endcode
   */
  void set_const(size_t pos, const Key& key) {
    auto iter = cache_.find(pos);
    if (iter != cache_.end()) {
      cache_[pos] = key;
    }
    set_state(EncodeKey(pos), key);
  }

 private:
  /**
   * @brief Generate the key of the specified index
   *
   * @param index
   * @return std::string
   */
  std::string EncodeKey(size_t index) {
    std::string key;
    key.reserve(name_.length() + 1 + sizeof(index));
    key.append(name_);
    key.append(1, 'A');
    key.append((char*)&index, sizeof(index));
    return key;
  }

 private:
  /**
   * @brief Refresh data to blockchain
   *
   */
  void Flush() {
    for (auto iter : cache_) {
      std::string key = EncodeKey(iter.first);
      set_state(key, iter.second);
    }
  }

 public:
  static const std::string kType;

 private:
  std::map<size_t, Key> cache_;

  const std::string name_ = kType + std::to_string(uint64_t(TableName));
};

template <Name::Raw TableName, typename Key, unsigned N>
const std::string Array<TableName, Key, N>::kType = "__array__";
}  // namespace db
}  // namespace platon
