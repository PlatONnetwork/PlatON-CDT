#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <iterator>
#include <platon/print.hpp>
#include <string>

namespace platon {
namespace container {

namespace detail {

template <class InIt, class OutIt>
inline std::pair<InIt, OutIt> copy_n(
    InIt b, typename std::iterator_traits<InIt>::difference_type n, OutIt d) {
  for (; n != 0; --n, ++b, ++d) {
    *d = *b;
  }
  return std::make_pair(b, d);
}

template <class Pod, class T>
inline void pod_fill(Pod* b, Pod* e, T c) {
  assert(b && e && b <= e);
  constexpr auto kUseMemset = sizeof(T) == 1;
  if /* constexpr */ (kUseMemset) {
    memset(b, c, size_t(e - b));
  } else {
    auto const ee = b + ((e - b) & ~7u);
    for (; b != ee; b += 8) {
      b[0] = c;
      b[1] = c;
      b[2] = c;
      b[3] = c;
      b[4] = c;
      b[5] = c;
      b[6] = c;
      b[7] = c;
    }
    // Leftovers
    for (; b != e; ++b) {
      *b = c;
    }
  }
}

/*
 * Lightly structured memcpy, simplifies copying PODs and introduces
 * some asserts. Unfortunately using this function may cause
 * measurable overhead (presumably because it adjusts from a begin/end
 * convention to a pointer/size convention, so it does some extra
 * arithmetic even though the caller might have done the inverse
 * adaptation outside).
 */
template <class Pod>
inline void pod_copy(const Pod* b, const Pod* e, Pod* d) {
  assert(b != nullptr);
  assert(e != nullptr);
  assert(d != nullptr);
  assert(e >= b);
  assert(d >= e || d + (e - b) <= b);
  memcpy(d, b, (e - b) * sizeof(Pod));
}

/*
 * Lightly structured memmove, simplifies copying PODs and introduces
 * some asserts
 */
template <class Pod>
inline void pod_move(const Pod* b, const Pod* e, Pod* d) {
  assert(e >= b);
  memmove(d, b, (e - b) * sizeof(*b));
}

static constexpr size_t kBasicSize = 24;
inline size_t good_malloc_size(size_t min_size) {
  if (min_size <= kBasicSize) {
    return kBasicSize;
  }

  size_t n = min_size % kBasicSize;
  size_t c = min_size / kBasicSize;
  if (n > 0) {
    return (c + 1) * kBasicSize;
  }
  return c * kBasicSize;
}

/**
 * Trivial wrappers around malloc, calloc, realloc that check for allocation
 * failure and throw std::bad_alloc in that case.
 */
inline void* checked_malloc(size_t size) {
  void* p = malloc(size);
  return p;
}

inline void* checked_realloc(void* ptr, size_t size) {
  void* p = realloc(ptr, size);
  return p;
}

/**
 * This function tries to reallocate a buffer of which only the first
 * currentSize bytes are used. The problem with using realloc is that
 * if currentSize is relatively small _and_ if realloc decides it
 * needs to move the memory chunk to a new buffer, then realloc ends
 * up copying data that is not used. It's generally not a win to try
 * to hook in to realloc() behavior to avoid copies - at least in
 * jemalloc, realloc() almost always ends up doing a copy, because
 * there is little fragmentation / slack space to take advantage of.
 */
inline void* smart_realloc(void* p, const size_t current_size,
                           const size_t current_capacity,
                           const size_t new_capacity) {
  assert(p);
  assert(current_size <= current_capacity && current_capacity < new_capacity);

  auto const slack = current_capacity - current_size;
  if (slack * 2 > current_size) {
    // Too much slack, malloc-copy-free cycle:
    auto const result = checked_malloc(new_capacity);
    std::memcpy(result, p, current_size);
    free(p);
    return result;
  }
  // If there's not too much slack, we realloc in hope of coalescing
  return checked_realloc(p, new_capacity);
}

}  // namespace detail

template <class Char>
class string_core {
 public:
  string_core() noexcept { reset(); }

  string_core(const string_core& rhs) {
    assert(&rhs != this);
    switch (rhs.category()) {
      case Category::IsSmall:
        CopySmall(rhs);
        break;
      case Category::IsMedium:
        CopyMedium(rhs);
        break;
      case Category::IsLarge:
        CopyLarge(rhs);
        break;
      default:
        std::abort();
    }
    assert(size() == rhs.size());
    assert(memcmp(data(), rhs.data(), size() * sizeof(Char)) == 0);
  }

  string_core& operator=(const string_core& rhs) = delete;

  string_core(string_core&& goner) noexcept {
    // Take goner's guts
    ml_ = goner.ml_;
    // Clean goner's carcass
    goner.reset();
  }

  string_core(const Char* const data, const size_t size) {
    if (size <= kMaxSmallSize) {
      InitSmall(data, size);
    } else if (size <= kMaxMediumSize) {
      InitMedium(data, size);
    } else {
      InitLarge(data, size);
    }
    assert(this->size() == size);
    assert(size == 0 || memcmp(this->data(), data, size * sizeof(Char)) == 0);
  }

  ~string_core() noexcept {
    if (category() == Category::IsSmall) {
      return;
    }
    DestroyMediumLarge();
  }

  // swap below doesn't test whether &rhs == this (and instead
  // potentially does extra work) on the premise that the rarity of
  // that situation actually makes the check more expensive than is
  // worth.
  void swap(string_core& rhs) {
    auto const t = ml_;
    ml_ = rhs.ml_;
    rhs.ml_ = t;
  }

  // In C++11 data() and c_str() are 100% equivalent.
  const Char* data() const { return c_str(); }

  Char* data() { return c_str(); }

  Char* MutableData() {
    switch (category()) {
      case Category::IsSmall:
        return small_;
      case Category::IsMedium:
        return ml_.data_;
      case Category::IsLarge:
        return MutableDataLarge();
    }
  }

  const Char* c_str() const {
    const Char* ptr = ml_.data_;
    // With this syntax, GCC and Clang generate a CMOV instead of a branch.
    ptr = (category() == Category::IsSmall) ? small_ : ptr;
    return ptr;
  }

  void shrink(const size_t delta) {
    if (category() == Category::IsSmall) {
      ShrinkSmall(delta);
    } else if (category() == Category::IsMedium ||
               RefCounted::Refs(ml_.data_) == 1) {
      ShrinkMedium(delta);
    } else {
      ShrinkLarge(delta);
    }
  }

  void reserve(size_t min_capacity) {
    switch (category()) {
      case Category::IsSmall:
        ReserveSmall(min_capacity);
        break;
      case Category::IsMedium:
        ReserveMedium(min_capacity);
        break;
      case Category::IsLarge:
        ReserveLarge(min_capacity);
        break;
      default:
        std::abort();
    }
    assert(capacity() >= min_capacity);
  }

  Char* ExpandNoinit(const size_t delta, bool exp_growth = false);

  void push_back(Char c) { *ExpandNoinit(1, /* exp_growth = */ true) = c; }

  size_t size() const {
    return (category() == Category::IsSmall) ? small_size() : ml_.size_;
  }

  size_t capacity() const {
    switch (category()) {
      case Category::IsSmall:
        return kMaxSmallSize;
      case Category::IsLarge:
        // For large-sized strings, a multi-referenced chunk has no
        // available capacity. This is because any attempt to append
        // data would trigger a new allocation.
        if (RefCounted::Refs(ml_.data_) > 1) {
          return ml_.size_;
        }
        break;
      case Category::IsMedium:
      default:
        break;
    }
    return ml_.capacity();
  }

  bool IsShared() const {
    return category() == Category::IsLarge && RefCounted::Refs(ml_.data_) > 1;
  }

 private:
  Char* c_str() {
    Char* ptr = ml_.data_;
    // With this syntax, GCC and Clang generate a CMOV instead of a branch.
    ptr = (category() == Category::IsSmall) ? small_ : ptr;
    return ptr;
  }

  void reset() { set_small_size(0); }

  void DestroyMediumLarge() noexcept {
    auto const c = category();
    assert(c != Category::IsSmall);
    if (c == Category::IsMedium) {
      free(ml_.data_);
    } else {
      RefCounted::DecrementRefs(ml_.data_);
    }
  }

  struct RefCounted {
    size_t ref_count_;
    Char data_[1];

    constexpr static size_t GetDataOffset() {
      return offsetof(RefCounted, data_);
    }

    static RefCounted* FromData(Char* p) {
      return static_cast<RefCounted*>(static_cast<void*>(
          static_cast<unsigned char*>(static_cast<void*>(p)) -
          GetDataOffset()));
    }

    static size_t Refs(Char* p) { return FromData(p)->ref_count_; }

    static void IncrementRefs(Char* p) { FromData(p)->ref_count_++; }

    static void DecrementRefs(Char* p) {
      auto const dis = FromData(p);
      size_t oldcnt = dis->ref_count_;
      dis->ref_count_--;
      assert(oldcnt > 0);
      if (oldcnt == 1) {
        free(dis);
      }
    }

    static RefCounted* Create(size_t* size) {
      const size_t alloc_size = detail::good_malloc_size(
          GetDataOffset() + (*size + 1) * sizeof(Char));
      auto result =
          static_cast<RefCounted*>(detail::checked_malloc(alloc_size));
      result->ref_count_ = 1;
      *size = (alloc_size - GetDataOffset()) / sizeof(Char) - 1;
      return result;
    }

    static RefCounted* Create(const Char* data, size_t* size) {
      const size_t effective_size = *size;
      auto result = Create(size);
      if (effective_size > 0) {
        detail::pod_copy(data, data + effective_size, result->data_);
      }
      return result;
    }

    static RefCounted* Reallocate(Char* const data, const size_t current_size,
                                  const size_t current_capacity,
                                  size_t* new_capacity) {
      assert(*new_capacity > 0 && *new_capacity > current_size);
      const size_t alloc_new_capacity = detail::good_malloc_size(
          GetDataOffset() + (*new_capacity + 1) * sizeof(Char));
      auto const dis = FromData(data);
      assert(dis->ref_count_ == 1);
      auto result = static_cast<RefCounted*>(detail::smart_realloc(
          dis, GetDataOffset() + (current_size + 1) * sizeof(Char),
          GetDataOffset() + (current_capacity + 1) * sizeof(Char),
          alloc_new_capacity));
      assert(result->ref_count_ == 1);
      *new_capacity = (alloc_new_capacity - GetDataOffset()) / sizeof(Char) - 1;
      return result;
    }
  };

  typedef uint8_t category_type;

  enum class Category : category_type {
    IsSmall = 0,
    IsMedium = 0x80,
    IsLarge = 0x40,
  };

  Category category() const {
    // works for both big-endian and little-endian
    return static_cast<Category>(bytes_[kLastChar] & kCategoryExtractMask);
  }

  struct MediumLarge {
    Char* data_;
    size_t size_;
    size_t capacity_;

    size_t capacity() const { return capacity_ & kCapacityExtractMask; }

    void set_capacity(size_t cap, Category cat) {
      capacity_ = cap | (static_cast<size_t>(cat) << kCategoryShift);
    }
  };

  union {
    uint8_t bytes_[sizeof(MediumLarge)];  // For accessing the last byte.
    Char small_[sizeof(MediumLarge) / sizeof(Char)];
    MediumLarge ml_;
  };

  constexpr static size_t kLastChar = sizeof(MediumLarge) - 1;
  constexpr static size_t kMaxSmallSize = kLastChar / sizeof(Char);
  constexpr static size_t kMaxMediumSize = 254 / sizeof(Char);
  constexpr static uint8_t kCategoryExtractMask = 0xC0;
  constexpr static size_t kCategoryShift = (sizeof(size_t) - 1) * 8;
  constexpr static size_t kCapacityExtractMask =
      ~(size_t(kCategoryExtractMask) << kCategoryShift);

  static_assert(!(sizeof(MediumLarge) % sizeof(Char)),
                "Corrupt memory layout for string.");

  size_t small_size() const {
    assert(category() == Category::IsSmall);
    auto small = static_cast<size_t>(small_[kMaxSmallSize]);
    assert(static_cast<size_t>(kMaxSmallSize) >= small);
    return static_cast<size_t>(kMaxSmallSize) - small;
  }

  void set_small_size(size_t s) {
    // Warning: this should work with uninitialized strings too,
    // so don't assume anything about the previous value of
    // small_[kMaxSmallSize].
    assert(s <= kMaxSmallSize);
    small_[kMaxSmallSize] = char(kMaxSmallSize - s);
    small_[s] = '\0';
    assert(category() == Category::IsSmall && size() == s);
  }

  void CopySmall(const string_core&);
  void CopyMedium(const string_core&);
  void CopyLarge(const string_core&);

  void InitSmall(const Char* data, size_t size);
  void InitMedium(const Char* data, size_t size);
  void InitLarge(const Char* data, size_t size);

  void ReserveSmall(size_t min_capacity);
  void ReserveMedium(size_t min_capacity);
  void ReserveLarge(size_t min_capacity);

  void ShrinkSmall(size_t delta);
  void ShrinkMedium(size_t delta);
  void ShrinkLarge(size_t delta);

  void unshare(size_t minCapacity = 0);
  Char* MutableDataLarge();
};  // namespace container

template <class Char>
inline void string_core<Char>::CopySmall(const string_core& rhs) {
  static_assert(offsetof(MediumLarge, data_) == 0, "string layout failure");
  static_assert(offsetof(MediumLarge, size_) == sizeof(ml_.data_),
                "string layout failure");
  static_assert(offsetof(MediumLarge, capacity_) == 2 * sizeof(ml_.data_),
                "string layout failure");
  // Just write the whole thing, don't look at details. In
  // particular we need to copy capacity anyway because we want
  // to set the size (don't forget that the last character,
  // which stores a short string's length, is shared with the
  // ml_.capacity field).
  ml_ = rhs.ml_;
  assert(category() == Category::IsSmall && this->size() == rhs.size());
}

template <class Char>
inline void string_core<Char>::CopyMedium(const string_core& rhs) {
  // Medium strings are copied eagerly. Don't forget to allocate
  // one extra Char for the null terminator.
  auto const alloc_size =
      detail::good_malloc_size((1 + rhs.ml_.size_) * sizeof(Char));
  ml_.data_ = static_cast<Char*>(detail::checked_malloc(alloc_size));
  // Also copies terminator.
  detail::pod_copy(rhs.ml_.data_, rhs.ml_.data_ + rhs.ml_.size_ + 1, ml_.data_);
  ml_.size_ = rhs.ml_.size_;
  ml_.set_capacity(alloc_size / sizeof(Char) - 1, Category::IsMedium);
  assert(category() == Category::IsMedium);
}

template <class Char>
inline void string_core<Char>::CopyLarge(const string_core& rhs) {
  // Large strings are just refcounted
  ml_ = rhs.ml_;
  RefCounted::IncrementRefs(ml_.data_);
  assert(category() == Category::IsLarge && size() == rhs.size());
}

// Small strings are bitblitted
template <class Char>
inline void string_core<Char>::InitSmall(const Char* const data,
                                         const size_t size) {
  // Layout is: Char* data_, size_t size_, size_t capacity_
  static_assert(sizeof(*this) == sizeof(Char*) + 2 * sizeof(size_t),
                "string has unexpected size");
  static_assert(sizeof(Char*) == sizeof(size_t),
                "string size assumption violation");
  // sizeof(size_t) must be a power of 2
  static_assert((sizeof(size_t) & (sizeof(size_t) - 1)) == 0,
                "string size assumption violation");

  if (size != 0) {
    detail::pod_copy(data, data + size, small_);
  }

  set_small_size(size);
}

template <class Char>
inline void string_core<Char>::InitMedium(const Char* const data,
                                          const size_t size) {
  // Medium strings are allocated normally. Don't forget to
  // allocate one extra Char for the terminating null.
  auto const alloc_size = detail::good_malloc_size((1 + size) * sizeof(Char));
  ml_.data_ = static_cast<Char*>(detail::checked_malloc(alloc_size));
  if (size > 0) {
    detail::pod_copy(data, data + size, ml_.data_);
  }
  ml_.size_ = size;
  ml_.set_capacity(alloc_size / sizeof(Char) - 1, Category::IsMedium);
  ml_.data_[size] = '\0';
}

template <class Char>
inline void string_core<Char>::InitLarge(const Char* const data,
                                         const size_t size) {
  // Large strings are allocated differently
  size_t effective_capacity = size;
  auto const new_rc = RefCounted::Create(data, &effective_capacity);
  ml_.data_ = new_rc->data_;
  ml_.size_ = size;
  ml_.set_capacity(effective_capacity, Category::IsLarge);
  ml_.data_[size] = '\0';
}

template <class Char>
inline void string_core<Char>::unshare(size_t min_capacity) {
  assert(category() == Category::IsLarge);
  size_t effective_capacity = std::max(min_capacity, ml_.capacity());
  auto const new_rc = RefCounted::Create(&effective_capacity);
  // If this fails, someone placed the wrong capacity in an
  // string.
  assert(effective_capacity >= ml_.capacity());
  // Also copies terminator.
  detail::pod_copy(ml_.data_, ml_.data_ + ml_.size_ + 1, new_rc->data_);
  RefCounted::DecrementRefs(ml_.data_);
  ml_.data_ = new_rc->data_;
  ml_.set_capacity(effective_capacity, Category::IsLarge);
  // size_ remains unchanged.
}

template <class Char>
inline Char* string_core<Char>::MutableDataLarge() {
  assert(category() == Category::IsLarge);
  if (RefCounted::Refs(ml_.data_) > 1) {  // Ensure unique.
    unshare();
  }
  return ml_.data_;
}

template <class Char>
inline void string_core<Char>::ReserveLarge(size_t min_capacity) {
  assert(category() == Category::IsLarge);
  if (RefCounted::Refs(ml_.data_) > 1) {  // Ensure unique
    // We must make it unique regardless; in-place reallocation is
    // useless if the string is shared. In order to not surprise
    // people, reserve the new block at current capacity or
    // more. That way, a string's capacity never shrinks after a
    // call to reserve.
    unshare(min_capacity);
  } else {
    // String is not shared, so let's try to realloc (if needed)
    if (min_capacity > ml_.capacity()) {
      // Asking for more memory
      auto const new_rc = RefCounted::Reallocate(ml_.data_, ml_.size_,
                                                 ml_.capacity(), &min_capacity);
      ml_.data_ = new_rc->data_;
      ml_.set_capacity(min_capacity, Category::IsLarge);
    }
    assert(capacity() >= min_capacity);
  }
}

template <class Char>
inline void string_core<Char>::ReserveMedium(const size_t min_capacity) {
  assert(category() == Category::IsMedium);
  // String is not shared
  if (min_capacity <= ml_.capacity()) {
    return;  // nothing to do, there's enough room
  }
  if (min_capacity <= kMaxMediumSize) {
    // Keep the string at medium size. Don't forget to allocate
    // one extra Char for the terminating null.
    size_t capacity_bytes =
        detail::good_malloc_size((1 + min_capacity) * sizeof(Char));
    // Also copies terminator.
    ml_.data_ = static_cast<Char*>(detail::smart_realloc(
        ml_.data_, (ml_.size_ + 1) * sizeof(Char),
        (ml_.capacity() + 1) * sizeof(Char), capacity_bytes));
    ml_.set_capacity(capacity_bytes / sizeof(Char) - 1, Category::IsMedium);
  } else {
    // Conversion from medium to large string
    string_core nascent;
    // Will recurse to another branch of this function
    nascent.reserve(min_capacity);
    nascent.ml_.size_ = ml_.size_;
    // Also copies terminator.
    detail::pod_copy(ml_.data_, ml_.data_ + ml_.size_ + 1, nascent.ml_.data_);
    nascent.swap(*this);
    assert(capacity() >= min_capacity);
  }
}

template <class Char>
inline void string_core<Char>::ReserveSmall(size_t min_capacity) {
  assert(category() == Category::IsSmall);
  if (min_capacity <= kMaxSmallSize) {
    // small
    // Nothing to do, everything stays put
  } else if (min_capacity <= kMaxMediumSize) {
    // medium
    // Don't forget to allocate one extra Char for the terminating null
    auto const alloc_size_bytes =
        detail::good_malloc_size((1 + min_capacity) * sizeof(Char));
    auto const data =
        static_cast<Char*>(detail::checked_malloc(alloc_size_bytes));
    auto const size = small_size();
    // Also copies terminator.
    detail::pod_copy(small_, small_ + size + 1, data);
    ml_.data_ = data;
    ml_.size_ = size;
    ml_.set_capacity(alloc_size_bytes / sizeof(Char) - 1, Category::IsMedium);
  } else {
    // large
    auto const new_rc = RefCounted::Create(&min_capacity);
    auto const size = small_size();
    // Also copies terminator.
    detail::pod_copy(small_, small_ + size + 1, new_rc->data_);
    ml_.data_ = new_rc->data_;
    ml_.size_ = size;
    ml_.set_capacity(min_capacity, Category::IsLarge);
    assert(capacity() >= min_capacity);
  }
}

template <class Char>
inline Char* string_core<Char>::ExpandNoinit(const size_t delta,
                                             bool exp_growth) {
  // Strategy is simple: make room, then change size
  assert(capacity() >= size());
  size_t sz, new_sz;
  if (category() == Category::IsSmall) {
    sz = small_size();
    new_sz = sz + delta;
    if (new_sz <= kMaxSmallSize) {
      set_small_size(new_sz);
      return small_ + sz;
    }
    ReserveSmall(exp_growth ? std::max(new_sz, 2 * kMaxSmallSize) : new_sz);
  } else {
    sz = ml_.size_;
    new_sz = sz + delta;
    if (new_sz > capacity()) {
      // ensures not shared
      reserve(exp_growth ? std::max(new_sz, 1 + capacity() * 3 / 2) : new_sz);
    }
  }
  assert(capacity() >= new_sz);
  // Category can't be small - we took care of that above
  assert(category() == Category::IsMedium || category() == Category::IsLarge);
  ml_.size_ = new_sz;
  ml_.data_[new_sz] = '\0';
  assert(size() == new_sz);
  return ml_.data_ + sz;
}

template <class Char>
inline void string_core<Char>::ShrinkSmall(const size_t delta) {
  // Check for underflow
  assert(delta <= small_size());
  set_small_size(small_size() - delta);
}

template <class Char>
inline void string_core<Char>::ShrinkMedium(const size_t delta) {
  // Medium strings and unique large strings need no special
  // handling.
  assert(ml_.size_ >= delta);
  ml_.size_ -= delta;
  ml_.data_[ml_.size_] = '\0';
}

template <class Char>
inline void string_core<Char>::ShrinkLarge(const size_t delta) {
  assert(ml_.size_ >= delta);
  // Shared large string, must make unique. This is because of the
  // durn terminator must be written, which may trample the shared
  // data.
  if (delta) {
    string_core(ml_.data_, ml_.size_ - delta).swap(*this);
  }
  // No need to write the terminator.
}

template <class CharT, class Traits = std::char_traits<CharT>>
class basic_string {
 public:
  typedef Traits traits_type;
  typedef CharT value_type;
  typedef value_type& reference;
  typedef const value_type& const_reference;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef value_type* pointer;
  typedef const value_type* const_pointer;

  typedef CharT* iterator;
  typedef const CharT* const_iterator;
  typedef std::reverse_iterator<iterator> reverse_iterator;
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

 private:
  static void Procrustes(size_type& n, size_type nmax) {
    if (n > nmax) {
      n = nmax;
    }
  }

  static size_type TraitsLength(const value_type* s) {
    if (s) {
      return traits_type::length(s);
    }
    return 0;
  }

 public:
  static constexpr size_type npos = size_type(-1);

 public:
  basic_string() noexcept {}

  basic_string(size_type count, value_type ch) {
    auto const data = store_.ExpandNoinit(count);
    detail::pod_fill(data, data + count, ch);
  }

  basic_string(const basic_string& other, size_type pos,
               size_type count = npos) {
    assign(other, pos, count);
  }

  basic_string(const value_type* s, size_type count) : store_(s, count) {}

  basic_string(const value_type* s) : basic_string(s, TraitsLength(s)) {}

  template <class InputIt>
  basic_string(InputIt first, InputIt last) {
    assign(first, last);
  }

  basic_string(const basic_string& rhs) : store_(rhs.store_) {}

  basic_string(basic_string&& rhs) noexcept : store_(std::move(rhs.store_)) {}

  basic_string(std::initializer_list<char> il) { assign(il.begin(), il.end()); }

  ~basic_string() noexcept {}

  basic_string& operator=(const basic_string& str);

  basic_string& operator=(basic_string&& str) noexcept;

  basic_string& operator=(const CharT* s) { return assign(s); }

  basic_string& operator=(CharT ch);

  basic_string& operator=(std::initializer_list<char> il) {
    return assign(il.begin(), il.end());
  }

  basic_string& assign(size_type count, CharT ch);

  basic_string& assign(const basic_string& str) {
    if (&str == *this) {
      return *this;
    }
    return assign(str.data(), str.size());
  }

  basic_string& assign(const basic_string& str, size_type pos,
                       size_type count = npos);

  basic_string& assign(basic_string&& str) noexcept {
    return *this = std::move(str);
  }

  basic_string& assign(const CharT* s, size_type count);

  basic_string& assign(const CharT* s) { return assign(s, TraitsLength(s)); }

  template <class InputIt>
  basic_string& assign(InputIt first, InputIt last) {
    return replace(begin(), end(), first, last);
  }

  basic_string& assign(std::initializer_list<char> ilist) {
    return assign(ilist.begin(), ilist.end());
  }

  // Access
  reference at(size_type pos) { return (*this)[pos]; }
  const_reference at(size_type pos) const { return (*this)[pos]; }

  reference operator[](size_type pos) { return *(begin() + pos); }
  const_reference operator[](size_type pos) const { return *(begin() + pos); }

  value_type& front() { return *begin(); }
  const value_type& front() const { return *begin(); }

  value_type& back() {
    assert(!empty());
    return *(end() - 1);
  }

  const value_type& back() const {
    assert(!empty());
    return *(end() - 1);
  }

  const value_type* data() const noexcept { return c_str(); }

  value_type* data() noexcept { return store_.data(); }

  const value_type* c_str() const { return store_.c_str(); }

  // Iterators
  iterator begin() noexcept { return store_.MutableData(); }
  const_iterator begin() const noexcept { return store_.data(); }
  const_iterator cbegin() const noexcept { return begin(); }

  iterator end() noexcept { return store_.MutableData() + store_.size(); }
  const_iterator end() const noexcept { return store_.data() + store_.size(); }
  const_iterator cend() const noexcept { return end(); }

  reverse_iterator rbegin() noexcept { return reverse_iterator(begin()); }
  const_reverse_iterator rbegin() const noexcept {
    return const_reverse_iterator(begin());
  }
  const_reverse_iterator crbegin() const noexcept { return rbegin(); }

  reverse_iterator rend() noexcept { return reverse_iterator(end()); }
  const_reverse_iterator rend() const noexcept {
    return const_reverse_iterator(end());
  }
  const_reverse_iterator crend() const noexcept { return rend(); }

  // Capacity
  bool empty() const noexcept { return size() == 0; }

  size_type size() const noexcept { return store_.size(); }

  size_type length() const noexcept { return size(); }

  size_type max_size() const noexcept { std::numeric_limits<size_type>::max(); }

  void reserve(size_type new_cap) { store_.reserve(new_cap); }

  size_type capacity() const noexcept { return store_.capacity(); }

  void shrink_to_fit() {
    // Shrink only if slack memory is sufficiently large
    if (capacity() < size() * 3 / 2) {
      return;
    }
    basic_string(cbegin(), cend()).swap(*this);
  }

  // operations
  void clear() noexcept { resize(0); }

  basic_string& insert(size_type index, size_type count, CharT ch) {
    insert(begin() + index, count, ch);
    return *this;
  }

  basic_string& insert(size_type index, const CharT* s) {
    return insert(index, s, TraitsLength(s));
  }

  basic_string& insert(size_type index, const CharT* s, size_type count) {
    insert(begin() + index, s, s + count);
    return *this;
  }

  basic_string& insert(size_type index, const basic_string& str) {
    return insert(index, str.data(), str.size());
  }

  basic_string& insetr(size_type index, const basic_string& str,
                       size_type index_str, size_type count = npos) {
    Procrustes(count, str.length() - index_str);
    insert(begin() + index, str.data() + index_str, count);
  }

  iterator insert(const_iterator pos, CharT ch) {
    const size_type p = pos - cbegin();
    insert(pos, 1, ch);
    return begin() + p;
  }

  iterator insert(const_iterator i, size_type n, value_type ch) {
    assert(i >= cbegin() && i <= cend());
    const size_type pos = i - cbegin();

    auto old_size = size();
    store_.ExpandNoinit(n, true);
    auto b = begin();
    detail::pod_move(b + pos, b + old_size, b + pos + n);
    detail::pod_fill(b + pos, b + pos + n, ch);

    return b + pos;
  }

  template <class InputIt>
  iterator insert(const_iterator pos, InputIt first, InputIt last) {
    return InsertImpl(
        pos, first, last,
        typename std::iterator_traits<InputIt>::iterator_category());
  }

  iterator insert(const_iterator pos, std::initializer_list<char> ilist) {
    return insert(pos, ilist.begin(), ilist.end());
  }

  basic_string& erase(size_type index = 0, size_type n = npos) {
    Procrustes(n, index);
    std::copy(begin() + index + n, end(), begin() + index);
    resize(length() - n);
    return *this;
  }

  iterator erase(const_iterator position) {
    const size_type pos(position - begin());
    erase(pos, 1);
    return begin() + pos;
  }

  iterator erase(const_iterator first, const_iterator last) {
    const size_type pos(first - begin());
    erase(pos, last - first);
    return begin() + pos;
  }

  void push_back(char ch) { store_.push_back(ch); }
  void pop_back() { store_.shrink(1); }

  basic_string& append(size_type count, CharT ch);

  basic_string& append(const basic_string& str);

  basic_string& append(const basic_string& str, size_type pos,
                       size_type count = npos);

  basic_string& append(const CharT* s, size_type count);

  basic_string& append(const CharT* s) { return append(s, TraitsLength(s)); }

  template <class InputIt>
  basic_string& append(InputIt first, InputIt last) {
    return insert(end(), first, last);
  }

  basic_string& append(std::initializer_list<char> ilist) {
    return append(ilist.begin(), ilist.end());
  }

  basic_string& operator+=(const basic_string& str) { return append(str); }

  basic_string& operator+=(CharT ch) {
    push_back(ch);
    return *this;
  }

  basic_string& operator+=(const CharT* s) { return append(s); }

  basic_string& operator+=(std::initializer_list<char> il) {
    append(il);
    return *this;
  }

  int compare(const basic_string& str) const noexcept {
    return compare(0, size(), str);
  }

  int compare(size_type pos1, size_type count1, const basic_string& str) const {
    return compare(pos1, count1, str.data(), str.size());
  }

  int compare(size_type pos1, size_type count1, const basic_string& str,
              size_type pos2, size_type count2) const {
    return compare(pos1, count1, str.data() + pos2,
                   std::min(count2, str.size() - pos2));
  }

  int compare(const CharT* s) const {
    // Could forward to compare(0, size(), s, traitsLength(s))
    // but that does two extra checks
    const size_type n1(size()), n2(TraitsLength(s));
    const int r = traits_type::compare(data(), s, std::min(n1, n2));
    return r != 0 ? r : n1 > n2 ? 1 : n1 < n2 ? -1 : 0;
  }

  int compare(size_type pos1, size_type count1, const CharT* s) const {
    return compare(pos1, count1, s, TraitsLength(s));
  }
  int compare(size_type pos1, size_type count1, const CharT* s,
              size_type count2) const {
    Procrustes(count1, size() - pos1);
    // The line below fixed by Jean-Francois Bastien, 04-23-2007. Thanks!
    const int r =
        traits_type::compare(pos1 + data(), s, std::min(count1, count2));
    return r != 0 ? r : count1 > count2 ? 1 : count1 < count2 ? -1 : 0;
  }

  basic_string& replace(size_type pos, size_type count,
                        const basic_string& str) {
    return replace(pos, count, str.data(), str.length());
  }

  basic_string& replace(const_iterator first, const_iterator last,
                        const basic_string& str) {
    return replace(first, last, str.data(), str.length());
  }

  basic_string& replace(size_type pos, size_type count, basic_string& str,
                        size_type pos2, size_type count2) {
    return replace(pos, count, str.data() + pos2,
                   std::min(count2, str.size() - pos2));
  }

  template <class InputIt>
  basic_string& replace(const_iterator first, const_iterator last,
                        InputIt first2, InputIt last2) {
    using Cat = typename std::iterator_traits<InputIt>::iterator_category;
    return ReplaceImpl(first, last, first2, last2, Cat());
  }

  basic_string& replace(size_type pos, size_type count, const value_type* s,
                        size_type count2) {
    Procrustes(count, length() - pos);
    const iterator b = begin() + pos;
    return replace(b, b + count, s, count2);
  }

  basic_string& replace(const_iterator first, const_iterator last,
                        const value_type* s, size_type count2) {
    assert(first <= last);
    assert(begin() <= first && first <= end());
    assert(begin() <= last && last <= end());
    return replace(first, last, s, s + count2);
  }

  basic_string& replace(size_type pos, size_type count, const value_type* s) {
    return replace(pos, count, s, TraitsLength(s));
  }
  basic_string& replace(const_iterator first, const_iterator last,
                        const value_type* s) {
    return replace(first, last, s, TraitsLength(s));
  }

  basic_string& replace(size_type pos, size_type count, size_type count2,
                        value_type ch) {
    Procrustes(count, size() - pos);
    const iterator b = begin() + pos;
    return replace(b, b + count, count2, ch);
  }

  basic_string& replace(const_iterator first, const_iterator last,
                        size_type count2, value_type ch) {
    size_type count = last - first;
    if (count > count2) {
      std::fill(first, first + count2, ch);
      erase(first + count2, last);
    } else {
      std::fill(first, last, ch);
      insert(last, count2 - count, ch);
    }
    return *this;
  }

  basic_string& replace(const_iterator first, const_iterator last,
                        std::initializer_list<char> il) {
    return replace(first, last, il.begin(), il.end());
  }

  basic_string substr(size_type pos = 0, size_type count = npos) const {
    return basic_string(data() + pos, std::min(count, size() - pos));
  }

  size_type copy(CharT* dest, size_type count, size_type pos = 0) const {
    Procrustes(count, size() - pos);
    if (count != 0) {
      detail::pod_copy(data() + pos, data() + pos + count, dest);
    }
    return count;
  }

  void resize(size_type count, value_type ch = value_type()) {
    auto size = this->size();
    if (count <= size) {
      store_.shrink(size - count);
    } else {
      auto const delta = count - size;
      auto data = store_.ExpandNoinit(delta);
      detail::pod_fill(data, data + delta, ch);
    }
    assert(this->size() == count);
  }

  void swap(basic_string& rhs) noexcept { store_.swap(rhs.store_); }

  size_type find(const basic_string& str, size_type pos = 0) const noexcept {
    return find(str.data(), pos, str.length());
  }
  size_type find(const value_type* s, size_type pos, size_type count) const;

  size_type find(const value_type* s, size_type pos = 0) const {
    return find(s, pos, TraitsLength(s));
  }

  size_type find(CharT ch, size_type pos = 0) const noexcept {
    return find(&ch, pos, 1);
  }

  size_type rfind(const basic_string& str, size_type pos = npos) const
      noexcept {
    return rfind(str.data(), pos, str.length());
  }

  size_type rfind(const value_type* s, size_type pos, size_type count) const;

  size_type rfind(const value_type* s, size_type pos = npos) const {
    return rfind(s, pos, TraitsLength(s));
  }

  size_type rfind(CharT ch, size_type pos = npos) const noexcept {
    return rfind(&ch, pos, 1);
  }

  size_type find_first_of(const basic_string& str, size_type pos = 0) const
      noexcept {
    return find_first_of(str.data(), pos, str.length());
  }
  size_type find_first_of(const value_type* s, size_type pos,
                          size_type count) const;

  size_type find_first_of(const value_type* s, size_type pos = 0) const {
    return find_first_of(s, pos, TraitsLength(s));
  }

  size_type find_first_of(value_type ch, size_type pos = 0) const noexcept {
    return find_first_of(&ch, pos, 1);
  }

  size_type find_first_not_of(const basic_string& str, size_type pos = 0) const
      noexcept {
    return find_first_not_of(str.data(), pos, str.length());
  }

  size_type find_first_not_of(const value_type* s, size_type pos,
                              size_type count) const;

  size_type find_first_not_of(const value_type* s, size_type pos = 0) const {
    return find_first_not_of(s, pos, TraitsLength(s));
  }

  size_type find_first_not_of(value_type ch, size_type pos = 0) const noexcept {
    return find_first_not_of(&ch, pos, 1);
  }

  size_type find_last_of(const basic_string& str, size_type pos = npos) const
      noexcept {
    return find_last_of(str.data(), pos, str.length());
  }

  size_type find_last_of(const value_type* s, size_type pos,
                         size_type count) const;

  size_type find_last_of(const value_type* s, size_type pos = npos) const {
    return find_last_of(s, pos, TraitsLength(s));
  }

  size_type find_last_of(value_type ch, size_type pos = npos) const noexcept {
    return find_last_of(&ch, pos, 1);
  }

  size_type find_last_not_of(const basic_string& str,
                             size_type pos = npos) const noexcept {
    return find_last_not_of(str.data(), pos, str.length());
  }

  size_type find_last_not_of(const value_type* s, size_type pos,
                             size_type count) const;

  size_type find_last_not_of(const value_type* s, size_type pos = npos) const {
    return find_last_not_of(s, pos, TraitsLength(s));
  }

  size_type find_last_not_of(CharT ch, size_type pos = npos) const noexcept {
    return find_last_not_of(ch, pos, 1);
  }

 private:
  template <class FwdIterator>
  iterator InsertImpl(const_iterator it, FwdIterator first, FwdIterator last,
                      std::forward_iterator_tag);

  template <class InputIterator>
  iterator InsertImpl(const_iterator it, InputIterator first,
                      InputIterator last, std::input_iterator_tag);

  template <class FwdIterator>
  bool ReplaceAliased(iterator, iterator, FwdIterator, FwdIterator,
                      std::false_type) {
    return false;
  }

  template <class FwdIterator>
  bool ReplaceAliased(iterator i1, iterator i2, FwdIterator s1, FwdIterator s2,
                      std::true_type);

  template <class FwdIterator>
  void ReplaceImpl(iterator i1, iterator i2, FwdIterator s1, FwdIterator s2,
                   std::forward_iterator_tag);

  template <class InputIterator>
  void ReplaceImpl(iterator i1, iterator i2, InputIterator b, InputIterator e,
                   std::input_iterator_tag);

 private:
  string_core<CharT> store_;
};

using string = basic_string<char>;

template <class CharT, class Traits>
basic_string<CharT, Traits>& basic_string<CharT, Traits>::operator=(
    const basic_string& str) {
  if (&str == this) {
    return *this;
  }

  assign(str.data(), str.length());
}

template <class CharT, class Traits>
basic_string<CharT, Traits>& basic_string<CharT, Traits>::operator=(
    basic_string&& str) noexcept {
  if (&str == this) {
    return *this;
  }
  this->~basic_string();
  new (&store_) string_core<CharT>(std::move(str.store_));
  return *this;
}
template <class CharT, class Traits>
basic_string<CharT, Traits>& basic_string<CharT, Traits>::operator=(
    value_type ch) {
  if (empty()) {
    store_.ExpandNoinit(1);
  } else if (store_.IsShared()) {
    basic_string(1, ch).swap(*this);
    return *this;
  } else {
    store_.shrink(size() - 1);
  }
  front() = ch;
  return *this;
}

template <class CharT, class Traits>
basic_string<CharT, Traits>& basic_string<CharT, Traits>::assign(
    size_type count, CharT ch) {
  return replace(begin(), end(), count, ch);
}

template <class CharT, class Traits>
basic_string<CharT, Traits>& basic_string<CharT, Traits>::assign(
    const basic_string& str, size_type pos, size_type count) {
  const size_type sz = str.size();
  Procrustes(count, sz - pos);
  return assign(str.data() + pos, count);
}

template <class CharT, class Traits>
basic_string<CharT, Traits>& basic_string<CharT, Traits>::assign(
    const value_type* s, size_type count) {
  if (count == 0) {
    resize(0);
  } else if (size() >= count) {
    detail::pod_move(s, s + count, store_.MutableData());
    store_.shrink(size() - count);
    assert(size() == count);
  } else {
    // If n is larger than size(), s cannot alias this string's
    // storage.
    resize(0);
    // Do not use exponential growth here: assign() should be tight,
    // to mirror the behavior of the equivalent constructor.
    detail::pod_copy(s, s + count, store_.ExpandNoinit(count));
  }

  assert(size() == count);
  return *this;
}

template <class CharT, class Traits>
template <class FwdIterator>
typename basic_string<CharT, Traits>::iterator
basic_string<CharT, Traits>::InsertImpl(const_iterator it, FwdIterator first,
                                        FwdIterator last,
                                        std::forward_iterator_tag) {
  assert(i >= cbegin() && i <= cend());
  const size_type pos = it - cbegin();
  auto n = std::distance(first, last);
  assert(n >= 0);

  auto old_size = size();
  store_.ExpandNoinit(n, true);
  auto b = begin();
  detail::pod_move(b + pos, b + old_size, b + pos + n);
  std::copy(first, last, b + pos);

  return b + pos;
}

template <class CharT, class Traits>
template <class InputIterator>
typename basic_string<CharT, Traits>::iterator
basic_string<CharT, Traits>::InsertImpl(const_iterator it, InputIterator first,
                                        InputIterator last,
                                        std::input_iterator_tag) {
  const auto pos = it - cbegin();
  basic_string temp(cbegin(), it);
  for (; first != last; ++first) {
    temp.push_back(*first);
  }
  temp.append(it, cend());
  swap(temp);
  return begin() + pos;
}

template <class CharT, class Traits>
template <class FwdIterator>
bool basic_string<CharT, Traits>::ReplaceAliased(iterator i1, iterator i2,
                                                 FwdIterator s1, FwdIterator s2,
                                                 std::true_type) {
  std::less_equal<const value_type*> le{};
  const bool aliased = le(&*begin(), &*s1) && le(&*s1, &*end());
  if (!aliased) {
    return false;
  }
  // Aliased replace, copy to new string
  basic_string temp;
  temp.reserve(size() - (i2 - i1) + std::distance(s1, s2));
  temp.append(begin(), i1).append(s1, s2).append(i2, end());
  swap(temp);
  return true;
}

template <class CharT, class Traits>
template <class FwdIterator>
void basic_string<CharT, Traits>::ReplaceImpl(iterator i1, iterator i2,
                                              FwdIterator s1, FwdIterator s2,
                                              std::forward_iterator_tag) {
  // Handle aliased replace
  using Sel =
      std::bool_constant<std::is_same<FwdIterator, iterator>::value ||
                         std::is_same<FwdIterator, const_iterator>::value>;
  if (ReplaceAliased(i1, i2, s1, s2, Sel())) {
    return;
  }

  auto const n1 = i2 - i1;
  assert(n1 >= 0);
  auto const n2 = std::distance(s1, s2);
  assert(n2 >= 0);

  if (n1 > n2) {
    // shrinks
    std::copy(s1, s2, i1);
    erase(i1 + n2, i2);
  } else {
    // grows
    s1 = detail::copy_n(s1, n1, i1).first;
    insert(i2, s1, s2);
  }
}

template <class CharT, class Traits>
template <class InputIterator>
void basic_string<CharT, Traits>::ReplaceImpl(iterator i1, iterator i2,
                                              InputIterator b, InputIterator e,
                                              std::input_iterator_tag) {
  basic_string temp(begin(), i1);
  temp.append(b, e).append(i2, end());
  swap(temp);
}

template <class CharT, class Traits>
basic_string<CharT, Traits>& basic_string<CharT, Traits>::append(
    size_type count, value_type ch) {
  auto const data = store_.ExpandNoinit(count, true);
  detail::pod_fill(data, data + count, ch);
  return *this;
}

template <class CharT, class Traits>
basic_string<CharT, Traits>& basic_string<CharT, Traits>::append(
    const basic_string& str) {
  append(str.data(), str.size());
  return *this;
}
template <class CharT, class Traits>
basic_string<CharT, Traits>& basic_string<CharT, Traits>::append(
    const basic_string& str, size_type pos, size_type count) {
  size_type sz = str.size();
  Procrustes(count, sz - pos);
  return append(str.data() + pos, count);
}

template <class CharT, class Traits>
basic_string<CharT, Traits>& basic_string<CharT, Traits>::append(
    const value_type* s, size_type n) {
  if (!n) {
    // Unlikely but must be done
    return *this;
  }
  auto const old_size = size();
  auto const old_data = data();
  auto new_data = store_.ExpandNoinit(n, true);

  std::less_equal<const value_type*> le;
  if (le(old_data, s) && !le(old_data + old_size, s)) {
    assert(le(s + n, old_data + old_size));
    // expandNoinit() could have moved the storage, restore the source.
    s = data() + (s - old_data);
    detail::pod_move(s, s + n, new_data);
  } else {
    detail::pod_copy(s, s + n, new_data);
  }

  assert(size() == old_size + n);
  return *this;
}

template <class CharT, class Traits>
typename basic_string<CharT, Traits>::size_type
basic_string<CharT, Traits>::find(const value_type* needle, size_type pos,
                                  size_type nsize) const {
  auto const size = this->size();
  // nsize + pos can overflow (eg pos == npos), guard against that by checking
  // that nsize + pos does not wrap around.
  if (nsize + pos > size || nsize + pos < pos) {
    return npos;
  }

  if (nsize == 0) {
    return pos;
  }
  // Don't use std::search, use a Boyer-Moore-like trick by comparing
  // the last characters first
  auto const haystack = data();
  auto const nsize_1 = nsize - 1;
  auto const last_needle = needle[nsize_1];

  // Boyer-Moore skip value for the last char in the needle. Zero is
  // not a valid value; skip will be computed the first time it's
  // needed.
  size_type skip = 0;

  const CharT* i = haystack + pos;
  auto iend = haystack + size - nsize_1;

  while (i < iend) {
    // Boyer-Moore: match the last element in the needle
    while (i[nsize_1] != last_needle) {
      if (++i == iend) {
        // not found
        return npos;
      }
    }
    // Here we know that the last char matches
    // Continue in pedestrian mode
    for (size_t j = 0;;) {
      assert(j < nsize);
      if (i[j] != needle[j]) {
        // Not found, we can skip
        // Compute the skip value lazily
        if (skip == 0) {
          skip = 1;
          while (skip <= nsize_1 && needle[nsize_1 - skip] != last_needle) {
            ++skip;
          }
        }
        i += skip;
        break;
      }
      // Check if done searching
      if (++j == nsize) {
        // Yay
        return i - haystack;
      }
    }
  }
  return npos;
}

// operator +
template <class CharT, class Traits>
basic_string<CharT, Traits> operator+(const basic_string<CharT, Traits>& lhs,
                                      const basic_string<CharT, Traits>& rhs) {
  basic_string<CharT, Traits> result;
  result.reserve(lhs.size() + rhs.size());
  result.append(lhs).append(rhs);
  return result;
}

template <class CharT, class Traits>
basic_string<CharT, Traits> operator+(const basic_string<CharT, Traits>& lhs,
                                      const CharT* rhs) {
  basic_string<CharT, Traits> result;
  const auto len = basic_string<CharT, Traits>::traits_type::length(rhs);
  result.reserve(len + lhs.size());
  result.append(lhs).append(rhs, len);
  return result;
}

template <class CharT, class Traits>
basic_string<CharT, Traits> operator+(const basic_string<CharT, Traits>& lhs,
                                      CharT rhs) {
  basic_string<CharT, Traits> result;
  result.reserve(1 + lhs.size());
  result.append(lhs);
  result.push_back(rhs);
  return result;
}

template <class CharT, class Traits>
basic_string<CharT, Traits> operator+(const CharT* lhs,
                                      const basic_string<CharT, Traits>& rhs) {
  basic_string<CharT, Traits> result;
  const auto len = basic_string<CharT, Traits>::traits_type::length(lhs);
  result.reserve(len + rhs.size());
  result.append(lhs, len).append(rhs);
  return result;
}

template <class CharT, class Traits>
basic_string<CharT, Traits> operator+(basic_string<CharT, Traits>&& lhs,
                                      basic_string<CharT, Traits>&& rhs) {
  return std::move(lhs.append(rhs));
}

template <class CharT, class Traits>
basic_string<CharT, Traits> operator+(basic_string<CharT, Traits>&& lhs,
                                      const basic_string<CharT, Traits>& rhs) {
  return std::move(lhs.append(rhs));
}

template <class CharT, class Traits>
basic_string<CharT, Traits> operator+(basic_string<CharT, Traits>&& lhs,
                                      const CharT* rhs) {
  return std::move(lhs += rhs);
}

template <class CharT, class Traits>
basic_string<CharT, Traits> operator+(basic_string<CharT, Traits>&& lhs,
                                      CharT ch) {
  return std::move(lhs += ch);
}

template <class CharT, class Traits>
basic_string<CharT, Traits> operator+(const basic_string<CharT, Traits>& lhs,
                                      basic_string<CharT, Traits>&& rhs) {
  if (rhs.capacity() >= lhs.size() + rhs.size()) {
    return std::move(rhs.insert(0, lhs));
  }
  auto const rhsc = rhs;
  return lhs + rhs;
}

template <class CharT, class Traits>
basic_string<CharT, Traits> operator+(const CharT* lhs,
                                      basic_string<CharT, Traits>&& rhs) {
  auto const len = basic_string<CharT, Traits>::traits_type::length(lhs);
  if (rhs.capacity() >= len + rhs.size()) {
    rhs.insert(rhs.begin(), lhs, len);
    return std::move(rhs);
  }
  basic_string<CharT, Traits> result;
  result.reserve(len + rhs.size());
  result.append(lhs, len).append(rhs);
  return result;
}

template <class CharT>
basic_string<CharT> operator+(CharT lhs, basic_string<CharT>&& rhs) {
  if (rhs.capacity() > rhs.size()) {
    rhs.insert(rhs.begin(), lhs);
    return std::move(rhs);
  }
  auto const& rhsc = rhs;
  return lhs + rhsc;
}

// Compare two basic_string objects
template <class CharT, class Traits>
bool operator==(const basic_string<CharT, Traits>& lhs,
                const basic_string<CharT, Traits>& rhs) {
  return lhs.size() == rhs.size() && lhs.compare(rhs) == 0;
}

template <class CharT, class Traits>
bool operator!=(const basic_string<CharT, Traits>& lhs,
                const basic_string<CharT, Traits>& rhs) {
  return !(lhs == rhs);
}

template <class CharT, class Traits>
bool operator<(const basic_string<CharT, Traits>& lhs,
               const basic_string<CharT, Traits>& rhs) {
  return lhs.compare(rhs) < 0;
}

template <class CharT, class Traits>
bool operator<=(const basic_string<CharT, Traits>& lhs,
                const basic_string<CharT, Traits>& rhs) {
  return !(lhs < rhs);
}

template <class CharT>
bool operator>(const basic_string<CharT>& lhs, const basic_string<CharT>& rhs) {
  return rhs < lhs;
}

template <class CharT>
bool operator>=(const basic_string<CharT>& lhs,
                const basic_string<CharT>& rhs) {
  return !(lhs < rhs);
}

// Compare a basic_string object and null-terminated array T
template <class CharT, class Traits>
bool operator==(const CharT* lhs, const basic_string<CharT, Traits>& rhs) {
  return rhs == lhs;
}

template <class CharT, class Traits>
bool operator==(const basic_string<CharT, Traits>& lhs, const CharT* rhs) {
  return lhs.compare(rhs) == 0;
}

template <class CharT, class Traits>
bool operator!=(const CharT* lhs, const basic_string<CharT, Traits>& rhs) {
  return !(lhs == rhs);
}

template <class CharT, class Traits>
bool operator!=(const basic_string<CharT, Traits>& lhs, const CharT* rhs) {
  return !(lhs == rhs);
}

template <class CharT, class Traits>
bool operator<(const CharT* lhs, const basic_string<CharT, Traits>& rhs) {
  return rhs.compare(lhs) > 0;
}

template <class CharT, class Traits>
bool operator<(const basic_string<CharT, Traits>& lhs, const CharT* rhs) {
  return lhs.compare(rhs) < 0;
}

template <class CharT, class Traits>
bool operator<=(const CharT* lhs, const basic_string<CharT, Traits>& rhs) {
  return !(rhs < lhs);
}

template <class CharT, class Traits>
bool operator<=(const basic_string<CharT, Traits>& lhs, const CharT* rhs) {
  return !(rhs < lhs);
}

template <class CharT, class Traits>
bool operator>(const CharT* lhs, const basic_string<CharT, Traits>& rhs) {
  return rhs < lhs;
}

template <class CharT, class Traits>
bool operator>(const basic_string<CharT, Traits>& lhs, const CharT* rhs) {
  return rhs < lhs;
}

template <class CharT, class Traits>
bool operator>=(const CharT* lhs, const basic_string<CharT, Traits>& rhs) {
  return !(lhs < rhs);
}

template <class CharT, class Traits>
bool operator>=(const basic_string<CharT, Traits>& lhs, const CharT* rhs) {
  return !(lhs < rhs);
}

template <class CharT, class Traits>
void swap(basic_string<CharT, Traits>& lhs,
          basic_string<CharT, Traits>& rhs) noexcept {
  lhs.swap(rhs);
}

template <class CharT, class Traits>
std::basic_ostream<typename basic_string<CharT, Traits>::value_type,
                   typename basic_string<CharT, Traits>::traits_type>&
operator<<(
    std::basic_ostream<typename basic_string<CharT, Traits>::value_type,
                       typename basic_string<CharT, Traits>::traits_type>& os,
    const basic_string<CharT, Traits>& str) {
  typedef std::basic_ostream<typename basic_string<CharT, Traits>::value_type,
                             typename basic_string<CharT, Traits>::traits_type>
      _ostream_type;
  typename _ostream_type::sentry _s(os);
  if (_s) {
    typedef std::ostreambuf_iterator<
        typename basic_string<CharT, Traits>::value_type,
        typename basic_string<CharT, Traits>::traits_type>
        _Ip;
    size_t __len = str.size();
    bool __left =
        (os.flags() & _ostream_type::adjustfield) == _ostream_type::left;
    if (__pad_and_output(_Ip(os), str.data(),
                         __left ? str.data() + __len : str.data(),
                         str.data() + __len, os, os.fill())
            .failed()) {
      os.setstate(_ostream_type::badbit | _ostream_type::failbit);
    }
  }
  return os;
}

template <class CharT, class Traits>
std::basic_istream<typename basic_string<CharT, Traits>::value_type,
                   typename basic_string<CharT, Traits>::traits_type>&
operator>>(
    std::basic_istream<typename basic_string<CharT, Traits>::value_type,
                       typename basic_string<CharT, Traits>::traits_type>& is,
    const basic_string<CharT, Traits>& str) {
  typedef std::basic_istream<typename basic_string<CharT, Traits>::value_type,
                             typename basic_string<CharT, Traits>::traits_type>
      _istream_type;
  typename _istream_type::sentry sentry(is);
  size_t extracted = 0;
  typename _istream_type::iostate err = _istream_type::goodbit;
  if (sentry) {
    auto n = is.width();
    if (n <= 0) {
      n = str.max_size();
    }
    str.erase();
    for (auto got = is.rdbuf()->sgetc(); extracted != size_t(n); ++extracted) {
      if (got == Traits::eof()) {
        err |= _istream_type::eofbit;
        is.width(0);
        break;
      }
      if (isspace(got)) {
        break;
      }
      str.push_back(got);
      got = is.rdbuf()->snextc();
    }
  }
  if (!extracted) {
    err |= _istream_type::failbit;
  }
  if (err) {
    is.setstate(err);
  }
  return is;
}

string to_string(int value);
string to_string(long value);
string to_string(long long value);
string to_string(unsigned value);
string to_string(unsigned long value);
string to_string(unsigned long long value);
string to_string(float value);
string to_string(double value);
string to_string(long double value);
}  // namespace container
}  // namespace platon
