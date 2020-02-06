#pragma once

#include "platon/rlp_serialize.hpp"
#include "platon/storage.hpp"
namespace platon {
namespace db {
/**
 * @brief Implement map operations, Map templates
 *
 * @tparam *Name The name of the Map, the name of the Map should be unique
 * within each contract.
 * @tparam Key key type
 * @tparam Value value type
 * @tparam MapType::Traverse The default is Traverse, when Traverse needs extra
 * data structure to operate, set to NoTraverse when no traversal operation is
 * needed.
 */
template <const char *Name, typename Key, typename Value>
class Map {
 public:
  /**
   * @brief KeyWrapper
   *
   */
  class KeyWrapper {
   public:
    /**
     * @brief Construct a new Key Wrapper object
     *
     * @param name first name
     * @param key key
     */
    KeyWrapper(const std::string &name, const Key &key)
        : name_(name), key_(key) {}
    /**
     * @brief Construct a new platon serialize object
     *
     * @param name_
     */
    PLATON_SERIALIZE(KeyWrapper, (name_)(key_))
   private:
    const std::string &name_;
    const Key &key_;
  };

 public:
  Map() {}
  Map(const Map<Name, Key, Value> &) = delete;
  Map(const Map<Name, Key, Value> &&) = delete;
  Map<Name, Key, Value> &operator=(const Map<Name, Key, Value> &) = delete;
  /**
   * @brief Destroy the Map object Refresh data to the blockchain
   *
   */
  ~Map() { flush(); }

  /**
   * @brief Insert a new key-value pair, Update to cache
   *
   * @param k Key
   * @param v Value
   * @return true Inserted successfully
   * @return false Insert failed
   */
  bool insert(const Key &k, const Value &v) {
    init();
    map_[k] = v;
    modify_.insert(k);
    return true;
  }

  /**
   * @brief Insert a new key-value pair that will not be updated to the cache.
   * Suitable for large number of inserts, no updates after insertion
   *
   * @param k Key
   * @param v Value
   * @return true Inserted successfully
   * @return false Insert failed
   */
  bool insert_const(const Key &k, const Value &v) {
    init();
    if (map_.find(k) != map_.end()) {
      map_[k] = v;
    }

    platon::set_state(KeyWrapper(keyPrefix_, k), v);
    return true;
  }

  /**
   * @brief Get the Const object, will not join the cache
   *
   * @param k Key
   * @return Value
   */
  Value get_const(const Key &k) {
    init();
    auto iter = map_.find(k);
    if (iter != map_.end()) {
      return iter->second;
    }

    Value v;
    platon::get_state(KeyWrapper(keyPrefix_, k), v);
    return v;
  }

  /**
   * @brief Get value, will be added to the cache
   *
   * @param k Key
   * @return Value&
   */
  Value &at(const Key &k) {
    init();
    auto iter = map_.find(k);
    if (iter != map_.end()) {
      return iter->second;
    }

    Value v;
    platon::get_state(KeyWrapper(keyPrefix_, k), v);
    map_[k] = v;
    modify_.insert(k);
    return map_[k];
  }

  /**
   * @brief Delete key-value pairs
   *
   * @param k Key
   */
  void erase(const Key &k) {
    init();
    auto iter = map_.find(k);
    if (iter != map_.end()) {
      map_.erase(iter);
    }
    modify_.insert(k);
  }

  /**
   * @brief Bracket operator
   *
   * @param k Key
   * @return Value& Get Value
   */
  Value &operator[](const Key &k) {
    init();
    return at(k);
  }

  /**
   * @brief Checks if there is an element with key equivalent to key in the
   * container.
   *
   * @param k Key
   * @return true if there is such an element, otherwise false.
   */
  bool contains(const Key &key) {
    init();
    auto iter = map_.find(key);
    if (iter != map_.end()) {
      return true;
    } else if (modify_.find(key) != modify_.end()) {
      return false;
    }
    return platon::has_state(KeyWrapper(keyPrefix_, key));
  }

  //    /**
  //     * @brief Get the length of the map, only allowed when the MapType is
  //     Traverse
  //     *
  //     * @return size_t length
  //     */
  //    size_t size() {
  //      init();
  //      return keySet_.size();
  //    }
  /**
   * @brief Refresh the modified data in memory to the blockchain
   *
   */
  void flush() {
    std::for_each(modify_.begin(), modify_.end(), [this](const Key &k) {
      auto iter = map_.find(k);
      if (iter != map_.end()) {
        platon::set_state(KeyWrapper(keyPrefix_, k), iter->second);
      } else {
        platon::del_state(KeyWrapper(keyPrefix_, k));
      }
    });
  }

 public:
  static const std::string kType;

 private:
  /**
   * @brief Initialize, get data from the blockchain
   *
   */
  void init() {
    //      platon::get_state(sizePrefix, size_);
  }

  std::map<Key, Value> map_;
  std::set<Key> modify_;
  const std::string keyPrefix_ = kType + Name;
  //    const std::string sizePrefix = kType + string("s_") + Name;
  //    size_t size_ = 0;

  bool init_ = false;
};

template <const char *Name, typename Key, typename Value>
const std::string Map<Name, Key, Value>::kType = "__map__";
}  // namespace db
}  // namespace platon
