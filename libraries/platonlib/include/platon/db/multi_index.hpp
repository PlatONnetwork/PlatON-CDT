#pragma once
#include <boost/hana.hpp>
#include <set>
#include <type_traits>
#include "platon/name.hpp"
#include "platon/print.hpp"
#include "platon/storagetype.hpp"

namespace platon {
namespace db {

namespace hana = boost::hana;
const uint32_t kMaxSecondaryIndexKeys = 100;
template <typename T>
struct IndexKey {
  uint64_t table_name;
  uint64_t index_name;
  T value;
  PLATON_SERIALIZE(IndexKey<T>, (table_name)(index_name)(value))
};
template <typename T>
struct SecondaryIndexKey {
  uint64_t table_name;
  uint64_t index_name;
  T value;
  uint32_t position;
  PLATON_SERIALIZE(SecondaryIndexKey<T>,
                   (table_name)(index_name)(value)(position))
};
struct NormalIndexValue {
  // set of sequence
  std::set<uint64_t> keys;

  uint32_t pre_seq = 0;
  uint32_t next_seq = 0;
  std::set<uint64_t>::iterator iter;
  PLATON_SERIALIZE(NormalIndexValue, (keys)(pre_seq)(next_seq));
};

struct IndexType {
  struct UniqueIndex {};
  struct NormalIndex {};
};

template <Name::Raw IndexSeq, typename Extractor>
struct IndexedBy {
  enum Constants { IndexName = static_cast<uint64_t>(IndexSeq) };
  //  static constexpr const char* IndexName() { return "Name"; }
  typedef Extractor SecondaryExtractorType;
};

template <class Class, typename Type,
          Type (Class::*PtrToMemberFunction)() const, typename Index>
struct IndexMemberFun {
  typedef typename std::remove_reference<Type>::type ResultType;
  typedef Index IndexType;
  template <typename ChainedPtr>
  auto operator()(const ChainedPtr& x) const -> std::enable_if_t<
      !std::is_convertible<const ChainedPtr&, const Class&>::value, Type> {
    return operator()(*x);
  }

  Type operator()(const Class& x) const { return (x.*PtrToMemberFunction)(); }

  Type operator()(const std::reference_wrapper<const Class>& x) const {
    return operator()(x.get());
  }

  Type operator()(const std::reference_wrapper<Class>& x) const {
    return operator()(x.get());
  }
};

struct MultiDBKey {
  uint64_t table_name;
  uint64_t seq;
  PLATON_SERIALIZE(MultiDBKey, (table_name)(seq))
};
template <typename T>
void set_state_db(uint64_t table_name, uint64_t seq, const T& value) {
  MultiDBKey key = {.table_name = table_name, .seq = seq};
  set_state(key, value);
}

template <typename T>
void get_state_db(uint64_t table_name, uint64_t seq, T& value) {
  MultiDBKey key = {.table_name = table_name, .seq = seq};
  get_state(key, value);
}
bool has_state_db(uint64_t table_name, uint64_t seq) {
  MultiDBKey key = {.table_name = table_name, .seq = seq};
  return has_state(key);
}
void delete_state_db(uint64_t table_name, uint64_t seq) {
  MultiDBKey key = {.table_name = table_name, .seq = seq};
  del_state(key);
}

template <typename T>
void set_index_db(uint64_t table_name, uint64_t index_name, uint64_t seq,
                  const T& value, bool unique) {
  // todo find value form Extractor, create index sequence
  IndexKey<T> key = {
      .table_name = table_name, .index_name = index_name, .value = value};

  set_state(key, seq);
}

template <typename T, typename R>
R get_index_db(uint64_t table_name, uint64_t index_name, const T& value) {
  R result;
  IndexKey<T> key = {
      .table_name = table_name, .index_name = index_name, .value = value};
  get_state(key, result);
  return result;
}
template <typename T>
void delete_index_db(uint64_t table_name, uint64_t index_name, const T& value) {
  IndexKey<T> key = {
      .table_name = table_name, .index_name = index_name, .value = value};
  del_state(key);
}

template <typename T>
void append_secondary_index_db(uint64_t table_name, uint64_t index_name,
                               uint64_t seq, const T& value) {
  // todo find value form Extractor, create index sequence
  IndexKey<T> key = {
      .table_name = table_name, .index_name = index_name, .value = value};
  if (!has_state(key)) {
    NormalIndexValue index_value;
    index_value.keys.emplace(seq);
    set_state(key, index_value);

  } else {
    NormalIndexValue index_value;
    get_state(key, index_value);
    index_value.keys.emplace(seq);
    set_state(key, index_value);
  }
}

template <typename T>
void delete_secondary_index_db(uint64_t table_name, uint64_t index_name,
                               uint64_t seq, const T& value) {
  IndexKey<T> key = {
      .table_name = table_name, .index_name = index_name, .value = value};

  if (has_state(key)) {
    NormalIndexValue pre_value;
    get_state(key, pre_value);
    if (pre_value.keys.find(seq) != pre_value.keys.end()) {
      pre_value.keys.erase(seq);

      if (pre_value.keys.empty()) {
        del_state(key);
      } else {
        set_state(key, pre_value);
      }
    }
  }
}
template <typename T>
bool check_unique(uint64_t table_name, uint64_t index_name, const T& value) {
  IndexKey<T> key = {
      .table_name = table_name, .index_name = index_name, .value = value};
  return has_state(key);
}

/**
 * @brief MultiIndex Multi Index Table
 * @details MultiIndex supports unique indexes and ordinary indexes. The unique
 * index should be placed first in the parameter. The structure needs to provide
 * the get function corresponding to the index field.
 *
 * Example:
 *
 * @code
  struct Member {
   std::string name;
   uint8_t age;
   uint8_t sex;
   uint64_t $seq_;
   std::string Name() const { return name; }
   uint8_t Age() const { return age; }
   PLATON_SERIALIZE(Member, (name)(age)(sex))
  };
  MultiIndex<
   "table"_n, Member,
    IndexedBy<"index"_n, IndexMemberFun<Member, std::string, &Member::Name,
                                       IndexType::UniqueIndex>>,
   IndexedBy<"index2"_n, IndexMemberFun<Member, uint8_t, &Member::Age,
                                        IndexType::NormalIndex>>>
   member_table;

 * @endcode
 */

template <Name::Raw TableName, typename T, typename... Indices>
class MultiIndex {
 public:
  template <Name::Raw IndexName, typename Extractor, uint64_t Number,
            typename IndexTypeName>
  struct Index {
   public:
    typedef Extractor SecondaryExtractorType;
    typedef typename std::decay<decltype(Extractor()(nullptr))>::type
        SecondaryKeyType;

    enum Constants {
      kTableName = static_cast<uint64_t>(TableName),
      kIndexName = static_cast<uint64_t>(IndexName),
      kIndexNumber = Number,
    };

    static bool unique() {
      return std::is_same<IndexTypeName, IndexType::UniqueIndex>::value;
    }
    static uint64_t index_name() { return kIndexName; }
    static uint64_t table_name() { return kTableName; }
    static uint64_t index_number() { return kIndexNumber; }
    static auto extract_secondary_key(const T& obj) {
      return SecondaryExtractorType()(obj);
    }
  };
  struct Item : public T {
    template <typename Constructor>
    Item(const MultiIndex* idx, Constructor&& c) : $idx(idx) {
      c(*this);
    }

    const MultiIndex* $idx;
    uint64_t $seq;
    std::shared_ptr<NormalIndexValue> normal_index_value;
  };

  class const_iterator
      : public std::iterator<std::bidirectional_iterator_tag, const T> {
   public:
    friend bool operator==(const const_iterator& a, const const_iterator& b) {
      if (a.multi_index_ != b.multi_index_) {
        return false;
      }
      // secondary index
      if (a.item_->normal_index_value != nullptr ||
          b.item_->normal_index_value != nullptr) {
        if (a.begin_ || b.begin_) {
          bool equal_a = a.item_->normal_index_value != nullptr
                             ? a.item_->normal_index_value->iter ==
                                   a.item_->normal_index_value->keys.begin()
                             : a.begin_;
          bool equal_b = b.item_->normal_index_value != nullptr
                             ? b.item_->normal_index_value->iter ==
                                   b.item_->normal_index_value->keys.begin()
                             : b.begin_;

          return equal_a && equal_b;
        }
        if (a.end_ || b.end_) {
          bool equal_a = a.item_->normal_index_value != nullptr
                             ? a.item_->normal_index_value->iter ==
                                   a.item_->normal_index_value->keys.end()
                             : a.end_;
          bool equal_b = b.item_->normal_index_value != nullptr
                             ? b.item_->normal_index_value->iter ==
                                   b.item_->normal_index_value->keys.end()
                             : b.end_;

          return equal_a && equal_b;
        }
      }

      return a.item_ == b.item_;
    }
    friend bool operator!=(const const_iterator& a, const const_iterator& b) {
      if (a.multi_index_ != b.multi_index_) {
        return true;
      }
      // secondary index
      if (a.item_->normal_index_value != nullptr ||
          b.item_->normal_index_value != nullptr) {
        if (a.begin_ || b.begin_) {
          bool equal_a = a.item_->normal_index_value != nullptr
                             ? a.item_->normal_index_value->iter ==
                                   a.item_->normal_index_value->keys.begin()
                             : a.begin_;
          bool equal_b = b.item_->normal_index_value != nullptr
                             ? b.item_->normal_index_value->iter ==
                                   b.item_->normal_index_value->keys.begin()
                             : b.begin_;

          return !(equal_a && equal_b);
        }
        if (a.end_ || b.end_) {
          bool equal_a = a.item_->normal_index_value != nullptr
                             ? a.item_->normal_index_value->iter ==
                                   a.item_->normal_index_value->keys.end()
                             : a.end_;
          bool equal_b = b.item_->normal_index_value != nullptr
                             ? b.item_->normal_index_value->iter ==
                                   b.item_->normal_index_value->keys.end()
                             : b.end_;

          return !(equal_a && equal_b);
        }
      }
      return a.item_ != b.item_;
    }

    const T& operator*() const { return *static_cast<const T*>(item_.get()); }
    const T* operator->() const { return static_cast<const T*>(item_.get()); }

    const_iterator operator++(int) {
      if (item_->normal_index_value != nullptr) {
        const_iterator result(*this);
        ++(*this);
        return result;
      }
      item_ = nullptr;
      return *this;
    }

    const_iterator operator--(int) {
      if (item_->normal_index_value != nullptr) {
        const_iterator result(*this);
        --(*this);
        return result;
      }
      const_iterator end(*this);
      end.end_ = true;
      return end;
    }

    const_iterator& operator++() {
      if (item_->normal_index_value != nullptr) {
        item_->normal_index_value->iter++;
      } else {
        item_ = nullptr;
      }
      return *this;
    }
    const_iterator& operator--() {
      if (item_->normal_index_value != nullptr) {
        if (item_->normal_index_value->iter !=
            item_->normal_index_value->keys.begin()) {
          item_->normal_index_value->iter--;
        }
      } else {
        item_ = nullptr;
      }

      return *this;
    }

   private:
    const_iterator(const MultiIndex* mi, std::shared_ptr<Item> i = nullptr)
        : multi_index_(mi), item_(i) {}
    const_iterator(const MultiIndex* mi, bool begin, bool end)
        : multi_index_(mi), item_(nullptr), begin_(begin), end_(end) {}
    void reset(std::shared_ptr<Item> i) { item_ = i; }

    const MultiIndex* multi_index_;
    std::shared_ptr<Item> item_;
    bool begin_ = false;
    bool end_ = false;
    friend class MultiIndex;
  };  /// class MultiIndex::const_iterator

  template <uint64_t I>
  struct IntC {
    enum e { value = I };
    operator uint64_t() const { return I; }
  };

 public:
  /**
   * @brief Iterator start position, but its return value cannot be incremented
   * or decremented because MultiIndex does not support full-order traversal
   * container.
   *
   *
   * @return const_iterator
   *
   * Example:
   *
   * @code
    struct Member {
     std::string name;
     uint8_t age;
     uint8_t sex;
     uint64_t $seq_;
     std::string Name() const { return name; }
     uint8_t Age() const { return age; }
     PLATON_SERIALIZE(Member, (name)(age)(sex))
    };
    MultiIndex<
     "table"_n, Member,
      IndexedBy<"index"_n, IndexMemberFun<Member, std::string, &Member::Name,
                                         IndexType::UniqueIndex>>,
     IndexedBy<"index2"_n, IndexMemberFun<Member, uint8_t, &Member::Age,
                                          IndexType::NormalIndex>>>
     member_table;
   auto iter = member_table.find<"index2"_n>(uint8_t(10));
   assert(iter == member_table.cbegin())
   * @endcode
   */
  const_iterator cbegin() const { return const_iterator(this, true, false); }

  /**
   * @brief Insert new value
   *
   *
   * @return const_iterator
   *
   * Example:
   *
   * @code
    struct Member {
     std::string name;
     uint8_t age;
     uint8_t sex;
     uint64_t $seq_;
     std::string Name() const { return name; }
     uint8_t Age() const { return age; }
     PLATON_SERIALIZE(Member, (name)(age)(sex))
    };
    MultiIndex<
     "table"_n, Member,
      IndexedBy<"index"_n, IndexMemberFun<Member, std::string, &Member::Name,
                                         IndexType::UniqueIndex>>,
     IndexedBy<"index2"_n, IndexMemberFun<Member, uint8_t, &Member::Age,
                                          IndexType::NormalIndex>>>
     member_table;
    auto iter = member_table.find<"index2"_n>(uint8_t(10));
    for (; iter != member_table.cend(); iter++) {
    }
   * @endcode
   */
  const_iterator cend() const { return const_iterator(this, false, true); }

  /**
   * @brief Iterator start position, but its return value cannot be incremented
   * or decremented because MultiIndex does not support full-order traversal
   * container.
   *
   * @param constructor of value
   *
   * @return Returns an iterator that indicates whether the insertion was
   * successful with the bool type. If unique index conflicts, the insertion
   * fails
   *
   * Example:
   *
   * @code
    struct Member {
     std::string name;
     uint8_t age;
     uint8_t sex;
     uint64_t $seq_;
     std::string Name() const { return name; }
     uint8_t Age() const { return age; }
     PLATON_SERIALIZE(Member, (name)(age)(sex))
    };
    MultiIndex<
     "table"_n, Member,
      IndexedBy<"index"_n, IndexMemberFun<Member, std::string, &Member::Name,
                                         IndexType::UniqueIndex>>,
     IndexedBy<"index2"_n, IndexMemberFun<Member, uint8_t, &Member::Age,
                                          IndexType::NormalIndex>>>
     member_table;

   member_table.emplace([&](auto &m) {
     m.age = 10;
     m.name = "hello";
     m.sex = 1;
    });
   * @endcode
   */
  template <typename Lambda>
  std::pair<const_iterator, bool> emplace(Lambda&& constructor) {
    // create new item
    auto item = std::make_shared<Item>(this, [&](auto& i) {
      T& obj = static_cast<T&>(i);
      constructor(obj);
      seq_++;
      i.$seq = seq_.get();
    });

    // check unique index conflict
    const T& obj = static_cast<T&>(*item);
    if (has_unique_index()) {
      typedef typename decltype(
          +hana::at_c<0>(hana::at_c<0>(indices_)))::type IndexType;
      if (IndexType::unique()) {
        if (check_unique<typename IndexType::SecondaryKeyType>(
                IndexType::table_name(), IndexType::index_name(),
                IndexType::extract_secondary_key(obj))) {
          return std::make_pair(const_iterator(this, nullptr), false);
        }
      }
    }

    hana::for_each(indices_, [&](auto& idx) {
      typedef typename decltype(+hana::at_c<0>(idx))::type IndexType;
      // todo set index into statedb
      if (IndexType::unique()) {
        set_index_db<typename IndexType::SecondaryKeyType>(
            IndexType::table_name(), IndexType::index_name(), seq_.get(),
            IndexType::extract_secondary_key(obj), IndexType::unique());
      } else {
        // todo append new
        append_secondary_index_db<typename IndexType::SecondaryKeyType>(
            IndexType::table_name(), IndexType::index_name(), seq_.get(),
            IndexType::extract_secondary_key(obj));
      }
    });

    set_state_db(static_cast<uint64_t>(TableName), seq_.get(), obj);
    return std::make_pair(const_iterator(this, item), true);
  }

  /**
   * @brief Find the data, the unique index will only return one piece of data,
   * the secondary index will return the data set, iterable through the
   * iterator.
   *
   * @param key key of index
   * @return iterator of data set
   *
   * Example:
   *
   * @code
    struct Member {
     std::string name;
     uint8_t age;
     uint8_t sex;
     uint64_t $seq_;
     std::string Name() const { return name; }
     uint8_t Age() const { return age; }
     PLATON_SERIALIZE(Member, (name)(age)(sex))
    };
    MultiIndex<
     "table"_n, Member,
      IndexedBy<"index"_n, IndexMemberFun<Member, std::string, &Member::Name,
                                         IndexType::UniqueIndex>>,
     IndexedBy<"index2"_n, IndexMemberFun<Member, uint8_t, &Member::Age,
                                          IndexType::NormalIndex>>>
     member_table;

    auto iter = member_table.find<"index2"_n>(uint8_t(10));
   * @endcode
   */
  template <Name::Raw IndexName, typename KEY>
  const_iterator find(const KEY& key) {
    const_iterator result(this, nullptr);
    hana::for_each(indices_, [&](auto& idx) {
      uint64_t index_name = static_cast<uint64_t>(IndexName);
      typedef typename decltype(+hana::at_c<0>(idx))::type IndexType;
      if (IndexType::index_name() == index_name) {
        if (IndexType::unique()) {
          auto seq = get_index_db<KEY, uint64_t>(
              static_cast<uint64_t>(TableName),
              static_cast<uint64_t>(IndexName), key);
          if (!has_state_db(IndexType::table_name(), seq)) {
            return;
          }
          auto item = std::make_shared<Item>(this, [&](auto& i) {
            T& obj = reinterpret_cast<T&>(i);
            get_state_db(IndexType::table_name(), seq, obj);
          });
          //          result = const_iterator(this, item);
          result.reset(item);
        } else {
          NormalIndexValue value = get_index_db<KEY, NormalIndexValue>(
              static_cast<uint64_t>(TableName),
              static_cast<uint64_t>(IndexName), key);
          if (value.keys.empty()) {
            result.reset(nullptr);
          } else {
            auto item = std::make_shared<Item>(this, [&](auto& i) {
              value.iter = value.keys.begin();
              T& obj = reinterpret_cast<T&>(i);
              get_state_db(IndexType::table_name(), *value.iter, obj);
              i.$seq = *value.iter;
              i.normal_index_value = std::make_shared<NormalIndexValue>(value);
              i.normal_index_value->iter = i.normal_index_value->keys.begin();
            });
            result.reset(item);
          }
        }
      }
    });

    return result;
  }

  /**
   * @brief Modify data based on iterator, but cannot modify all index-related
   * fields .
   *
   * @param position position of iterator
   * @param constructor lambda function that updates the target object
   *
   * Example:
   *
   * @code
    struct Member {
     std::string name;
     uint8_t age;
     uint8_t sex;
     uint64_t $seq_;
     std::string Name() const { return name; }
     uint8_t Age() const { return age; }
     PLATON_SERIALIZE(Member, (name)(age)(sex))
    };
    MultiIndex<
     "table"_n, Member,
      IndexedBy<"index"_n, IndexMemberFun<Member, std::string, &Member::Name,
                                         IndexType::UniqueIndex>>,
     IndexedBy<"index2"_n, IndexMemberFun<Member, uint8_t, &Member::Age,
                                          IndexType::NormalIndex>>>
     member_table;

     member_table.modify(r.first, [&](auto &m) { m.sex = 15; });
   * @endcode
   */
  template <typename Lambda>
  void modify(const_iterator position, Lambda&& constructor) {
    // reduce query statedb operation, don't chekc exists position in statedb
    // so user need make sure position exists
    // create new item
    auto item = std::make_shared<Item>(this, [&](auto& i) {
      T& obj = static_cast<T&>(i);
      obj = *position;
      constructor(obj);
    });

    T& new_obj = static_cast<T&>(*item);
    const T& old_obj = static_cast<const T&>(*position);
    bool enable = true;

    // update index key is illegal operation
    hana::for_each(indices_, [&](auto& idx) {
      typedef typename decltype(+hana::at_c<0>(idx))::type IndexType;
      // check index key
      if (IndexType::extract_secondary_key(old_obj) !=
          IndexType::extract_secondary_key(new_obj)) {
        enable = false;
      }
    });

    if (enable) {
      // update
      set_state_db(static_cast<uint64_t>(TableName), position.item_->$seq,
                   new_obj);
    }
  }

  /**
   * @brief Modify data based on iterator, but cannot modify all index-related
   * fields .
   *
   * @param position position of iterator
   *
   * Example:
   *
   * @code
    struct Member {
     std::string name;
     uint8_t age;
     uint8_t sex;
     uint64_t $seq_;
     std::string Name() const { return name; }
     uint8_t Age() const { return age; }
     PLATON_SERIALIZE(Member, (name)(age)(sex))
    };
    MultiIndex<
    "table"_n, Member,
    IndexedBy<"index"_n, IndexMemberFun<Member, std::string, &Member::Name,
                                       IndexType::UniqueIndex>>,
    IndexedBy<"index2"_n, IndexMemberFun<Member, uint8_t, &Member::Age,
                                        IndexType::NormalIndex>>>
    member_table;

    auto iter = member_table.find<"index2"_n>(uint8_t(10));
    member_table.erase(iter);
   * @endcode
   */
  void erase(const_iterator position) {
    hana::for_each(indices_, [&](auto& idx) {
      typedef typename decltype(+hana::at_c<0>(idx))::type IndexType;
      if (IndexType::unique()) {
        delete_index_db<typename IndexType::SecondaryKeyType>(
            IndexType::table_name(), IndexType::index_name(),
            IndexType::extract_secondary_key(*position));
      } else {
        delete_secondary_index_db<typename IndexType::SecondaryKeyType>(
            IndexType::table_name(), IndexType::index_name(),
            position.item_->$seq, IndexType::extract_secondary_key(*position));
      }
    });
    // delete key
    delete_state_db(static_cast<uint64_t>(TableName), position.item_->$seq);
  }
  int unique_index_number() {
    if (unique_number_ != -1) {
      return unique_number_;
    }
    hana::for_each(indices_, [&](auto& idx) {
      typedef typename decltype(+hana::at_c<0>(idx))::type IndexType;
      if (IndexType::unique()) {
        unique_number_ = IndexType::index_number();
      }
    });
    return unique_number_;
  }
  bool has_unique_index() { return unique_index_number() != -1; }
  static constexpr auto transform_indices() {
    typedef decltype(hana::zip_shortest(
        hana::make_tuple(IntC<0>(), IntC<1>(), IntC<2>(), IntC<3>(), IntC<4>()),
        hana::tuple<Indices...>())) IndicesInputType;

    return hana::transform(IndicesInputType(), [&](auto&& idx) {
      typedef typename std::decay<decltype(hana::at_c<0>(idx))>::type NumType;
      typedef typename std::decay<decltype(hana::at_c<1>(idx))>::type IdxType;
      return hana::make_tuple(
          hana::type_c<
              Index<Name::Raw(static_cast<uint64_t>(IdxType::IndexName)),
                    typename IdxType::SecondaryExtractorType, NumType::e::value,
                    typename IdxType::SecondaryExtractorType::IndexType>>,
          hana::type_c<
              Index<Name::Raw(static_cast<uint64_t>(IdxType::IndexName)),
                    typename IdxType::SecondaryExtractorType, NumType::e::value,
                    typename IdxType::SecondaryExtractorType::IndexType>>);
    });
  }

  typedef decltype(MultiIndex::transform_indices()) IndicesType;

  IndicesType indices_;
  Uint64<TableName> seq_;
  int unique_number_ = -1;
};  // namespace db
}  // namespace db
}  // namespace platon
