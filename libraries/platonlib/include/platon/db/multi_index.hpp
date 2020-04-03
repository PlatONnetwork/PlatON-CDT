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

template <typename T>
struct IndexKey {
  uint64_t table_name;
  uint64_t index_name;
  T value;
  PLATON_SERIALIZE(IndexKey<T>, (table_name)(index_name)(value))
};

template <typename T>
struct NormalIndexKey {
  uint64_t table_name;
  uint64_t index_name;
  T value;

  // section number
  uint64_t serial;

  PLATON_SERIALIZE(NormalIndexKey, (table_name)(index_name)(value)(serial))
};

struct NormalIndexValue {
  constexpr static size_t MAXSIZE = 30;

  uint64_t previous;
  // sequence data
  std::vector<uint64_t> vect_seq;
  uint64_t next;

  PLATON_SERIALIZE(NormalIndexValue, (previous)(vect_seq)(next));
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
  auto operator()(const ChainedPtr &x) const -> std::enable_if_t<
      !std::is_convertible<const ChainedPtr &, const Class &>::value, Type> {
    return operator()(*x);
  }

  Type operator()(const Class &x) const { return (x.*PtrToMemberFunction)(); }

  Type operator()(const std::reference_wrapper<const Class> &x) const {
    return operator()(x.get());
  }

  Type operator()(const std::reference_wrapper<Class> &x) const {
    return operator()(x.get());
  }
};

// sequence id and T data
struct MultiDBKey {
  uint64_t table_name;
  uint64_t seq;
  PLATON_SERIALIZE(MultiDBKey, (table_name)(seq))
};

template <typename T>
void set_state_db(uint64_t table_name, uint64_t seq, const T &value) {
  MultiDBKey key = {.table_name = table_name, .seq = seq};
  set_state(key, value);
}

template <typename T>
void get_state_db(uint64_t table_name, uint64_t seq, T &value) {
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

// unique index and squence id
template <typename T>
void set_index_db(uint64_t table_name, uint64_t index_name, uint64_t seq,
                  const T &value) {
  // todo find value form Extractor, create index sequence
  IndexKey<T> key = {
      .table_name = table_name, .index_name = index_name, .value = value};

  set_state(key, seq);
}

template <typename T>
bool has_index_db(uint64_t table_name, uint64_t index_name, const T &value) {
  IndexKey<T> key = {
      .table_name = table_name, .index_name = index_name, .value = value};
  return has_state(key);
}

template <typename T, typename R>
R get_index_db(uint64_t table_name, uint64_t index_name, const T &value) {
  R result;
  IndexKey<T> key = {
      .table_name = table_name, .index_name = index_name, .value = value};
  get_state(key, result);
  return result;
}

template <typename T>
void delete_index_db(uint64_t table_name, uint64_t index_name, const T &value) {
  IndexKey<T> key = {
      .table_name = table_name, .index_name = index_name, .value = value};
  del_state(key);
}

template <typename T>
bool check_unique(uint64_t table_name, uint64_t index_name, const T &value) {
  IndexKey<T> key = {
      .table_name = table_name, .index_name = index_name, .value = value};
  return has_state(key);
}

// normal index key and previous sequence, next sequence
constexpr uint64_t HEADSERIAL = 0;

template <typename T>
bool has_normal_index_db(uint64_t table_name, uint64_t index_name,
                         const T &value) {
  NormalIndexKey<T> key = {.table_name = table_name,
                           .index_name = index_name,
                           .value = value,
                           .serial = HEADSERIAL};
  NormalIndexValue result;
  size_t len = get_state(key, result);
  if (0 == len) return false;
  return true;
}

template <typename T>
NormalIndexValue get_normal_index_one_db(uint64_t table_name,
                                         uint64_t index_name, const T &value,
                                         uint64_t serial) {
  NormalIndexKey<T> key = {.table_name = table_name,
                           .index_name = index_name,
                           .value = value,
                           .serial = serial};
  NormalIndexValue result;
  get_state(key, result);
  return result;
}

template <typename T>
void set_normal_index_one_db(uint64_t table_name, uint64_t index_name,
                             const T &value, uint64_t serial,
                             const NormalIndexValue &index_value) {
  NormalIndexKey<T> key = {.table_name = table_name,
                           .index_name = index_name,
                           .value = value,
                           .serial = serial};
  set_state(key, index_value);
}

template <typename T>
void delete_normal_index_one_db(uint64_t table_name, uint64_t index_name,
                                const T &value, uint64_t serial) {
  NormalIndexKey<T> key = {.table_name = table_name,
                           .index_name = index_name,
                           .value = value,
                           .serial = serial};
  del_state(key);
}

template <typename T>
void append_normal_index_one_db(uint64_t table_name, uint64_t index_name,
                                uint64_t seq, const T &value) {
  NormalIndexKey<T> key = {.table_name = table_name,
                           .index_name = index_name,
                           .value = value,
                           .serial = HEADSERIAL};
  NormalIndexValue head;
  size_t len = get_state(key, head);

  // the first data
  if (0 == len) {
    head.vect_seq = std::vector<uint64_t>{seq};
    head.previous = HEADSERIAL;
    head.next = HEADSERIAL;
    set_normal_index_one_db(table_name, index_name, value, HEADSERIAL, head);
    return;
  }

  // the first one
  if (HEADSERIAL == head.previous && HEADSERIAL == head.next) {
    if (head.vect_seq.size() < NormalIndexValue::MAXSIZE) {
      head.vect_seq.push_back(seq);
      set_normal_index_one_db(table_name, index_name, value, HEADSERIAL, head);
    } else {
      NormalIndexValue new_value = {
          .previous = HEADSERIAL, .vect_seq = {seq}, .next = HEADSERIAL};
      uint64_t new_serial = HEADSERIAL + 1;
      set_normal_index_one_db(table_name, index_name, value, new_serial,
                              new_value);
      head.previous = new_serial;
      head.next = new_serial;
      set_normal_index_one_db(table_name, index_name, value, HEADSERIAL, head);
    }
    return;
  }

  // more than one
  if (HEADSERIAL != head.previous) {
    uint64_t last_serial = head.previous;
    NormalIndexValue old_last =
        get_normal_index_one_db(table_name, index_name, value, last_serial);
    if (old_last.vect_seq.size() < NormalIndexValue::MAXSIZE) {
      old_last.vect_seq.push_back(seq);
      set_normal_index_one_db(table_name, index_name, value, last_serial,
                              old_last);
    } else {
      NormalIndexValue new_value = {
          .previous = last_serial, .vect_seq = {seq}, .next = HEADSERIAL};
      uint64_t new_serial = last_serial + 1;
      set_normal_index_one_db(table_name, index_name, value, new_serial,
                              new_value);
      old_last.next = new_serial;
      set_normal_index_one_db(table_name, index_name, value, last_serial,
                              old_last);
      head.previous = new_serial;
      set_normal_index_one_db(table_name, index_name, value, HEADSERIAL, head);
    }
  }
}

template <typename T>
void delete_normal_index_db(uint64_t table_name, uint64_t index_name,
                            uint64_t seq, const T &value) {
  NormalIndexKey<T> key = {.table_name = table_name,
                           .index_name = index_name,
                           .value = value,
                           .serial = HEADSERIAL};
  NormalIndexValue head;
  size_t len = get_state(key, head);
  if (0 == len) return;

  // only one
  if (HEADSERIAL == head.previous && HEADSERIAL == head.next) {
    auto iter =
        std::lower_bound(head.vect_seq.begin(), head.vect_seq.end(), seq);
    if (head.vect_seq.end() == iter) return;
    head.vect_seq.erase(iter);
    if (0 == head.vect_seq.size()) {
      delete_normal_index_one_db(table_name, index_name, value, HEADSERIAL);
    } else {
      set_normal_index_one_db(table_name, index_name, value, HEADSERIAL, head);
    }
    return;
  }

  // more than one
  auto next_valid = [&](uint64_t serial, bool &bfind) {
    NormalIndexValue one_value;
    while (true) {
      NormalIndexKey<T> key = {.table_name = table_name,
                               .index_name = index_name,
                               .value = value,
                               .serial = serial};
      size_t len = get_state(key, one_value);
      if (0 != len) {
        bfind = true;
        break;
      }
      ++serial;
      if (serial > head.previous) {
        bfind = false;
        break;
      }
    }
    return std::make_pair(serial, one_value);
  };

  auto previous_valid = [&](uint64_t serial, bool &bfind) {
    NormalIndexValue one_value;
    while (true) {
      NormalIndexKey<T> key = {.table_name = table_name,
                               .index_name = index_name,
                               .value = value,
                               .serial = serial};
      size_t len = get_state(key, one_value);
      if (0 != len) {
        bfind = true;
        break;
      }
      if (HEADSERIAL == serial) {
        bfind = false;
        break;
      }
      --serial;
    }
    return std::make_pair(serial, one_value);
  };

  uint64_t begin_serial = HEADSERIAL;
  uint64_t end_serial = head.previous;
  bool find_valid = false;

  while (begin_serial <= end_serial) {
    uint64_t mid_serial = (begin_serial + end_serial) / 2;

    auto next_valid_pair = next_valid(mid_serial, find_valid);
    if (!find_valid) return;

    if (next_valid_pair.second.vect_seq.back() < seq) {
      begin_serial = next_valid(next_valid_pair.first + 1, find_valid).first;
      if (!find_valid) return;
    } else if (next_valid_pair.second.vect_seq.front() > seq) {
      if (mid_serial == next_valid_pair.first) mid_serial -= 1;
      end_serial = previous_valid(mid_serial, find_valid).first;
      if (!find_valid) return;
    } else {
      uint64_t real_serial = next_valid_pair.first;
      NormalIndexValue real_value = next_valid_pair.second;
      auto iter = std::lower_bound(real_value.vect_seq.begin(),
                                   real_value.vect_seq.end(), seq);
      if (real_value.vect_seq.end() == iter) return;
      real_value.vect_seq.erase(iter);
      if (0 == real_value.vect_seq.size()) {
        uint64_t previous = real_value.previous;
        uint64_t next = real_value.next;
        delete_normal_index_one_db(table_name, index_name, value, real_serial);

        // head
        if (HEADSERIAL == real_serial) {
          // only one
          if (HEADSERIAL == previous && HEADSERIAL == next) return;

          // only two
          if (previous == next) {
            NormalIndexValue next_value =
                get_normal_index_one_db(table_name, index_name, value, next);
            delete_normal_index_one_db(table_name, index_name, value, next);
            set_normal_index_one_db(table_name, index_name, value, HEADSERIAL,
                                    next_value);
            return;
          }

          // more than two
          NormalIndexValue next_value =
              get_normal_index_one_db(table_name, index_name, value, next);
          next_value.previous = previous;
          delete_normal_index_one_db(table_name, index_name, value, next);
          set_normal_index_one_db(table_name, index_name, value, HEADSERIAL,
                                  next_value);
          uint64_t new_next = next_value.next;
          NormalIndexValue new_next_value =
              get_normal_index_one_db(table_name, index_name, value, new_next);
          new_next_value.previous = HEADSERIAL;
          set_normal_index_one_db(table_name, index_name, value, new_next,
                                  new_next_value);
          return;
        }

        // only two
        if (HEADSERIAL == previous && HEADSERIAL == next) {
          head.previous = HEADSERIAL;
          head.next = HEADSERIAL;
          set_normal_index_one_db(table_name, index_name, value, HEADSERIAL,
                                  head);
          return;
        }

        // more than two
        NormalIndexValue previous_value =
            get_normal_index_one_db(table_name, index_name, value, previous);
        previous_value.next = next;
        set_normal_index_one_db(table_name, index_name, value, previous,
                                previous_value);
        NormalIndexValue next_value =
            get_normal_index_one_db(table_name, index_name, value, next);
        next_value.previous = previous;
        set_normal_index_one_db(table_name, index_name, value, next,
                                next_value);
      } else {
        set_normal_index_one_db(table_name, index_name, value, real_serial,
                                real_value);
      }
      break;
    }
  }
}

template <typename T>
size_t get_normal_index_count_db(uint64_t table_name, uint64_t index_name,
                                 const T &value) {
  NormalIndexValue head =
      get_normal_index_one_db(table_name, index_name, value, HEADSERIAL);
  size_t count = head.vect_seq.size();
  while (HEADSERIAL != head.next) {
    head = get_normal_index_one_db(table_name, index_name, value, head.next);
    count += head.vect_seq.size();
  }

  return count;
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
  static_assert(sizeof...(Indices) <= 16,
                "multi_index only supports a maximum of 16 secondary indices");

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

    static constexpr bool unique() {
      return std::is_same<IndexTypeName, IndexType::UniqueIndex>::value;
    }

    static constexpr uint64_t index_name() { return kIndexName; }
    static constexpr uint64_t table_name() { return kTableName; }
    static constexpr uint64_t index_number() { return kIndexNumber; }

    static auto extract_secondary_key(const T &obj) {
      return SecondaryExtractorType()(obj);
    }

    // iterator
    class const_iterator
        : public std::iterator<std::bidirectional_iterator_tag, const T> {
     public:
      friend bool operator==(const const_iterator &a, const const_iterator &b) {
        if (a.multiIndex_ != b.multiIndex_) return false;
        if (a.key_ != b.key_) return false;
        if (a.serial_ != b.serial_) return false;
        if (a.num_ != b.num_) return false;
        return true;
      }

      friend bool operator!=(const const_iterator &a, const const_iterator &b) {
        return !(a == b);
      }

      const T &operator*() const {
        NormalIndexValue index_value =
            get_normal_index_one_db(table_name(), index_name(), key_, serial_);
        uint64_t seq = index_value.vect_seq[num_];
        return static_cast<T &>(*multiIndex_->get_item_ptr(seq));
      }

      uint64_t get_seq() {
        NormalIndexValue index_value =
            get_normal_index_one_db(table_name(), index_name(), key_, serial_);
        return index_value.vect_seq[num_];
      }

      const T *operator->() const { return &const_cast<T &>(**this); }

      const_iterator &operator++() {
        NormalIndexValue index_value =
            get_normal_index_one_db(table_name(), index_name(), key_, serial_);
        if (num_ == index_value.vect_seq.size() - 1) {
          serial_ = index_value.next;
          if (HEADSERIAL == serial_) {
            num_ = NormalIndexValue::MAXSIZE;
          } else {
            num_ = 0;
          }
        } else {
          ++num_;
        }
        return *this;
      }

      const_iterator operator++(int) {
        auto result = *this;
        ++(*this);
        return result;
      }

      const_iterator &operator--() {
        NormalIndexValue index_value =
            get_normal_index_one_db(table_name(), index_name(), key_, serial_);
        if (num_ == 0) {
          if (HEADSERIAL == serial_) {
            num_ = NormalIndexValue::MAXSIZE;
          } else {
            serial_ = index_value.previous;
            index_value = get_normal_index_one_db(table_name(), index_name(),
                                                  key_, serial_);
            num_ = index_value.vect_seq.size() - 1;
          }
        } else {
          --num_;
        }
        return *this;
      }

      const_iterator operator--(int) {
        auto result = *this;
        --(*this);
        return result;
      }

     private:
      const_iterator(MultiIndex *mi, const SecondaryKeyType &key,
                     uint64_t serial, size_t num)
          : multiIndex_(mi), key_(key), serial_(serial), num_(num) {}

      MultiIndex *multiIndex_;
      SecondaryKeyType key_;
      uint64_t serial_;
      size_t num_;

      friend class MultiIndex;
      friend class Index;
    };

    /**
      * @brief Iterator start position
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
        auto index = member_table.get_index<"index2"_n>();
        for (auto it = index.cbegin(uint8_t(10)); it != index.cend(uint8_t(10));
       ++it) {}
      * @endcode
      */
    const_iterator cbegin(const SecondaryKeyType &value) {
      NormalIndexKey<SecondaryKeyType> key = {.table_name = table_name(),
                                              .index_name = index_name(),
                                              .value = value,
                                              .serial = HEADSERIAL};
      NormalIndexValue result;
      size_t len = get_state(key, result);
      if (0 == len) return cend(value);
      return const_iterator(multidx_, value, HEADSERIAL, 0);
    }

    /**
      * @brief Iterator end position
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
          IndexedBy<"index"_n, IndexMemberFun<Member, std::string,
        &Member::Name, IndexType::UniqueIndex>>, IndexedBy<"index2"_n,
        IndexMemberFun<Member, uint8_t, &Member::Age, IndexType::NormalIndex>>>
            member_table;
            auto index = member_table.get_index<"index2"_n>();
            for (auto it = index.cbegin(uint8_t(10)); it !=
        index.cend(uint8_t(10));
            ++it) {}
      * @endcode
      */
    const_iterator cend(const SecondaryKeyType &key) {
      return const_iterator(multidx_, key, HEADSERIAL,
                            NormalIndexValue::MAXSIZE);
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

      auto index = member_table.get_index<"index2"_n>();
      iter = index.cbegin(uint8_t(10));
      index.modify(iter, [&](auto &m) { m.sex = 15; });
     * @endcode
     */
    template <typename Lambda>
    void modify(const_iterator position, Lambda &&constructor) {
      auto item = std::make_shared<Item>(multidx_, [&](auto &i) {
        T &obj = static_cast<T &>(i);
        obj = *position;
        constructor(obj);
        i.$seq = position.get_seq();
      });

      T &new_obj = static_cast<T &>(*item);
      const T &old_obj = static_cast<const T &>(*position);
      bool enable = true;

      // update index key is illegal operation
      hana::any_of(multidx_->indices_, [&](auto &idx) {
        typedef typename decltype(+hana::at_c<0>(idx))::type IndexType;
        // check index key
        if (IndexType::extract_secondary_key(old_obj) !=
            IndexType::extract_secondary_key(new_obj)) {
          enable = false;
          return true;
        }
        return false;
      });

      if (enable) {
        // update
        set_state_db(static_cast<uint64_t>(TableName), position.get_seq(),
                     new_obj);
        multidx_->seq2item_[item->$seq] = item;
      }
    }

    /**
     * @brief erase data based on iterator.
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

      auto index = member_table.get_index<"index2"_n>();
      auto iter = index.cbegin(uint8_t(10));
      index.erase(iter);
     * @endcode
     */
    void erase(const_iterator position) {
      uint64_t seq = position.get_seq();
      hana::for_each(multidx_->indices_, [&](auto &idx) {
        typedef typename decltype(+hana::at_c<0>(idx))::type IndexType;
        if (IndexType::unique()) {
          delete_index_db<typename IndexType::SecondaryKeyType>(
              IndexType::table_name(), IndexType::index_name(),
              IndexType::extract_secondary_key(*position));
        } else {
          delete_normal_index_db<typename IndexType::SecondaryKeyType>(
              IndexType::table_name(), IndexType::index_name(), seq,
              IndexType::extract_secondary_key(*position));
        }
      });

      // delete key
      delete_state_db(static_cast<uint64_t>(TableName), seq);

      // delete
      multidx_->seq2item_.erase(seq);
    }

   private:
    friend class MultiIndex;
    Index(MultiIndex *midx) : multidx_(midx) {}
    MultiIndex *multidx_;
  };

  /**
    * @brief Gets the index object of a non-unique index
    *
    *
    * @return index object
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
      auto index = member_table.get_index<"index2"_n>();
    * @endcode
    */
  template <Name::Raw IndexName>
  auto get_index() {
    static_assert(
        !check_index_unique<IndexName>(),
        "name provided cannot be the unique index within multi_index");
    auto res = hana::find_if(indices_, [&](auto &&idx) {
      typedef typename decltype(+hana::at_c<0>(idx))::type IndexType;
      return std::integral_constant<
          bool, IndexType::index_name() == static_cast<uint64_t>(IndexName)>();
    });
    static_assert(res != hana::nothing,
                  "name provided is not the name of any secondary index within "
                  "multi_index");
    return typename decltype(+hana::at_c<0>(res.value()))::type(this);
  }

 private:
  // item
  struct Item : public T {
    template <typename Constructor>
    Item(const MultiIndex *idx, Constructor &&c) : $idx(idx) {
      c(*this);
    }

    const MultiIndex *$idx;
    uint64_t $seq;
  };

  std::map<uint64_t, std::shared_ptr<Item>> seq2item_;

  std::shared_ptr<Item> get_item_ptr(uint64_t seq) {
    std::shared_ptr<Item> result;
    if (seq2item_.find(seq) != seq2item_.end()) {
      result = seq2item_[seq];
    } else {
      auto item = std::make_shared<Item>(this, [&](auto &i) {
        T &obj = static_cast<T &>(i);
        get_state_db(static_cast<uint64_t>(TableName), seq, obj);
        i.$seq = seq;
      });
      seq2item_[seq] = item;
      result = item;
    }
    return result;
  }

  template <uint64_t I>
  struct IntC {
    enum e { value = I };
    operator uint64_t() const { return I; }
  };

 public:
  class const_iterator
      : public std::iterator<std::bidirectional_iterator_tag, const T> {
   public:
    friend bool operator==(const const_iterator &a, const const_iterator &b) {
      if (a.multiIndex_ != b.multiIndex_) {
        return false;
      }
      return a.item_->$seq == b.item_->$seq;
    }

    friend bool operator!=(const const_iterator &a, const const_iterator &b) {
      return !(a == b);
    }

    const T &operator*() const {
      T &obj = static_cast<T &>(*item_);
      auto &seq2item = multiIndex_->seq2item_;
      if (seq2item.find(item_->$seq) == seq2item.end()) {
        get_state_db(static_cast<uint64_t>(TableName), item_->$seq, obj);
        seq2item[item_->$seq] = item_;
      }
      return obj;
    }

    const T *operator->() const { return &const_cast<T &>(**this); }

    const_iterator &operator++() {
      uint64_t end_seq = multiIndex_->seq_.get();
      if (item_->$seq == end_seq) {
        return *this;
      }

      auto &seq2item = multiIndex_->seq2item_;
      uint64_t seq = item_->$seq + 1;
      if (seq2item.find(seq) == seq2item.end()) {
        for (; seq < end_seq; ++seq) {
          if (has_state_db(static_cast<uint64_t>(TableName), seq)) break;
        }

        // get item
        if (seq2item.find(seq) != seq2item.end()) {
          item_ = seq2item[seq];
        } else {
          item_ = std::make_shared<Item>(multiIndex_,
                                         [&](auto &i) { i.$seq = seq; });
        }
      } else {
        item_ = seq2item[seq];
      }
      return *this;
    }

    const_iterator operator++(int) {
      auto result = *this;
      ++(*this);
      return result;
    }

    const_iterator &operator--() {
      uint64_t end_seq = multiIndex_->seq_.get();
      uint64_t seq = 0 == item_->$seq ? end_seq : item_->$seq - 1;
      auto &seq2item = multiIndex_->seq2item_;
      if (seq2item.find(seq) == seq2item.end()) {
        constexpr uint64_t begin_seq = 0;
        for (; seq >= begin_seq; --seq) {
          if (has_state_db(static_cast<uint64_t>(TableName), seq)) break;
          if (seq == begin_seq) {
            seq = end_seq;
            break;
          }
        }

        // get item
        if (seq2item.find(seq) != seq2item_.end()) {
          item_ = seq2item[seq];
        } else {
          item_ = std::make_shared<Item>(multiIndex_,
                                         [&](auto &i) { i.$seq = seq; });
        }
      } else {
        item_ = seq2item[seq];
      }
      return *this;
    }

    const_iterator operator--(int) {
      auto result = *this;
      --(*this);
      return result;
    }

   private:
    const_iterator(MultiIndex *mi, std::shared_ptr<Item> i = nullptr)
        : multiIndex_(mi), item_(i) {}

    void reset(std::shared_ptr<Item> i) { item_ = i; }

    MultiIndex *multiIndex_;
    std::shared_ptr<Item> item_;
    friend class MultiIndex;
  };  /// class MultiIndex::const_iterator

  /**
   * @brief Iterator start position
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
   for (auto it = member_table.cbegin(); it != it_end; ++it){}
   * @endcode
   */
  const_iterator cbegin() {
    uint64_t begin_seq = 0;
    uint64_t end_seq = seq_.get();
    for (; begin_seq < end_seq; ++begin_seq) {
      if (has_state_db(static_cast<uint64_t>(TableName), begin_seq)) break;
    }

    const_iterator result(this, nullptr);
    if (seq2item_.find(begin_seq) != seq2item_.end()) {
      result.reset(seq2item_[begin_seq]);
    } else {
      auto item =
          std::make_shared<Item>(this, [&](auto &i) { i.$seq = begin_seq; });
      result.reset(item);
    }
    return result;
  }

  /**
   * @brief Iterator end position
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
    for (auto it = member_table.cbegin(); it != it_end; ++it) {
    }
   * @endcode
   */
  const_iterator cend() {
    auto item =
        std::make_shared<Item>(this, [&](auto &i) { i.$seq = seq_.get(); });
    return const_iterator(this, item);
  }

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
  std::pair<const_iterator, bool> emplace(Lambda &&constructor) {
    // create new item
    auto item = std::make_shared<Item>(this, [&](auto &i) {
      T &obj = static_cast<T &>(i);
      constructor(obj);
      i.$seq = seq_.get();
      seq_.self() += 1;
    });

    // check unique index conflict
    const T &obj = static_cast<T &>(*item);
    if (has_unique_index()) {
      bool is_conflict = hana::any_of(indices_, [&](auto &idx) {
        typedef typename decltype(+hana::at_c<0>(idx))::type IndexType;
        if (IndexType::unique()) {
          if (check_unique<typename IndexType::SecondaryKeyType>(
                  IndexType::table_name(), IndexType::index_name(),
                  IndexType::extract_secondary_key(obj)))
            return true;
        }
        return false;
      });
      if (is_conflict) {
        seq_.self() -= 1;
        return std::make_pair(cend(), false);
      }
    }

    // set index into statedb
    hana::for_each(indices_, [&](auto &idx) {
      typedef typename decltype(+hana::at_c<0>(idx))::type IndexType;
      // todo set index into statedb
      if (IndexType::unique()) {
        set_index_db<typename IndexType::SecondaryKeyType>(
            IndexType::table_name(), IndexType::index_name(), item->$seq,
            IndexType::extract_secondary_key(obj));
      } else {
        // todo append new
        append_normal_index_one_db<typename IndexType::SecondaryKeyType>(
            IndexType::table_name(), IndexType::index_name(), item->$seq,
            IndexType::extract_secondary_key(obj));
      }
    });

    set_state_db(static_cast<uint64_t>(TableName), item->$seq, obj);

    // add
    seq2item_[item->$seq] = item;

    return std::make_pair(const_iterator(this, item), true);
  }

  /**
   * @brief Find the data, Only a unique index is available.
   *
   * @param key key of index
   * @return the first iterator. cend() if not found.
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

    auto vect_iter = member_table.find<"index2"_n>(uint8_t(10));
   * @endcode
   */
  template <Name::Raw IndexName, typename KEY>
  const_iterator find(const KEY &key) {
    static_assert(check_index_unique<IndexName>(),
                  "name provided is not the unique index within multi_index");
    const_iterator result = cend();
    hana::any_of(indices_, [&](auto &idx) {
      uint64_t index_name = static_cast<uint64_t>(IndexName);
      typedef typename decltype(+hana::at_c<0>(idx))::type IndexType;
      if (IndexType::index_name() == index_name) {
        if (has_index_db(static_cast<uint64_t>(TableName),
                         static_cast<uint64_t>(IndexName), key)) {
          uint64_t seq = get_index_db<KEY, uint64_t>(
              static_cast<uint64_t>(TableName),
              static_cast<uint64_t>(IndexName), key);
          auto item = get_item_ptr(seq);
          result.reset(item);
        }
        return true;
      }
      return false;
    });
    return result;
  }

  /**
    * @brief
    *
    *
    * @return Gets the number of data corresponding to the index value
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
      auto count = member_table.count<"index2"_n>(uint8_t(10));
    * @endcode
    */
  template <Name::Raw IndexName, typename KEY>
  size_t count(const KEY &key) {
    size_t result = 0;
    hana::any_of(indices_, [&](auto &idx) {
      uint64_t index_name = static_cast<uint64_t>(IndexName);
      typedef typename decltype(+hana::at_c<0>(idx))::type IndexType;
      if (IndexType::index_name() == index_name) {
        if (IndexType::unique()) {
          if (has_index_db(static_cast<uint64_t>(TableName),
                           static_cast<uint64_t>(IndexName), key))
            result = 1;
        } else {
          if (has_normal_index_db(static_cast<uint64_t>(TableName),
                                  static_cast<uint64_t>(IndexName), key))
            result = get_normal_index_count_db(static_cast<uint64_t>(TableName),
                                               static_cast<uint64_t>(IndexName),
                                               key);
        }
        return true;
      }
      return false;
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
  void modify(const_iterator position, Lambda &&constructor) {
    // reduce query statedb operation, don't chekc exists position in statedb
    // so user need make sure position exists
    // create new item
    auto item = std::make_shared<Item>(this, [&](auto &i) {
      T &obj = static_cast<T &>(i);
      obj = *position;
      constructor(obj);
      i.$seq = position.item_->$seq;
    });

    T &new_obj = static_cast<T &>(*item);
    const T &old_obj = static_cast<const T &>(*position);
    bool enable = true;

    // update index key is illegal operation
    hana::any_of(indices_, [&](auto &idx) {
      typedef typename decltype(+hana::at_c<0>(idx))::type IndexType;
      // check index key
      if (IndexType::extract_secondary_key(old_obj) !=
          IndexType::extract_secondary_key(new_obj)) {
        enable = false;
        return true;
      }
      return false;
    });

    if (enable) {
      // update
      set_state_db(static_cast<uint64_t>(TableName), position.item_->$seq,
                   new_obj);
      seq2item_[item->$seq] = item;
    }
  }

  /**
   * @brief erase data based on iterator.
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

    auto vect_iter = member_table.find<"index2"_n>(uint8_t(10));
    member_table.erase(vect_iter[0]);
   * @endcode
   */
  void erase(const_iterator position) {
    hana::for_each(indices_, [&](auto &idx) {
      typedef typename decltype(+hana::at_c<0>(idx))::type IndexType;
      if (IndexType::unique()) {
        delete_index_db<typename IndexType::SecondaryKeyType>(
            IndexType::table_name(), IndexType::index_name(),
            IndexType::extract_secondary_key(*position));
      } else {
        delete_normal_index_db<typename IndexType::SecondaryKeyType>(
            IndexType::table_name(), IndexType::index_name(),
            position.item_->$seq, IndexType::extract_secondary_key(*position));
      }
    });

    // delete key
    delete_state_db(static_cast<uint64_t>(TableName), position.item_->$seq);

    // delete
    seq2item_.erase(position.item_->$seq);
  }

  static constexpr auto transform_indices() {
    typedef decltype(hana::zip_shortest(
        hana::make_tuple(IntC<0>(), IntC<1>(), IntC<2>(), IntC<3>(), IntC<4>(),
                         IntC<5>(), IntC<6>(), IntC<7>(), IntC<8>(), IntC<9>(),
                         IntC<10>(), IntC<11>(), IntC<12>(), IntC<13>(),
                         IntC<14>(), IntC<15>()),
        hana::tuple<Indices...>())) IndicesInputType;

    return hana::transform(IndicesInputType(), [&](auto &&idx) {
      typedef typename std::decay<decltype(hana::at_c<0>(idx))>::type NumType;
      typedef typename std::decay<decltype(hana::at_c<1>(idx))>::type IdxType;
      return hana::make_tuple(
          hana::type_c<
              Index<Name::Raw(static_cast<uint64_t>(IdxType::IndexName)),
                    typename IdxType::SecondaryExtractorType, NumType::e::value,
                    typename IdxType::SecondaryExtractorType::IndexType>>);
    });
  }

  typedef decltype(MultiIndex::transform_indices()) IndicesType;

  IndicesType indices_;

  static constexpr bool has_unique_index() {
    return hana::any_of(IndicesType(), [&](auto &idx) {
      typedef typename decltype(+hana::at_c<0>(idx))::type IndexType;
      return IndexType::unique();
    });
  }

  template <Name::Raw IndexName>
  static constexpr bool check_index_unique() {
    return hana::any_of(IndicesType(), [&](auto &idx) {
      typedef typename decltype(+hana::at_c<0>(idx))::type IndexType;
      return IndexType::index_name() == static_cast<uint64_t>(IndexName) &&
             IndexType::unique();
    });
  }

  template <Name::Raw IndexName>
  static constexpr bool check_index_exist() {
    return hana::any_of(IndicesType(), [&](auto &idx) {
      typedef typename decltype(+hana::at_c<0>(idx))::type IndexType;
      return IndexType::index_name() == static_cast<uint64_t>(IndexName);
    });
  }

  Uint64<TableName> seq_;
};
}  // namespace db
}  // namespace platon
