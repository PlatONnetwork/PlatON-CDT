//
// Created by yangzhou on 2/18/20.
//

#ifndef __VECTOR_H_
#define __VECTOR_H_
#include <stdbool.h>
#include <stddef.h>
#include <algorithm>
#include <iterator>
namespace platon {
namespace container {

/***** DEFINITIONS *****/

#define VECTOR_MINIMUM_CAPACITY 2
#define VECTOR_GROWTH_FACTOR 2
#define VECTOR_SHRINK_THRESHOLD (1 / 4)

#define VECTOR_ERROR -1
#define VECTOR_SUCCESS 0

#define VECTOR_UNINITIALIZED NULL
#define VECTOR_INITIALIZER \
  { 0, 0, 0, VECTOR_UNINITIALIZED }

/***** STRUCTURES *****/

typedef struct Vector {
  size_t size;
  size_t capacity;
  size_t element_size;

  void* data;
} Vector;

typedef struct Iterator {
  void* pointer;
  size_t element_size;
} Iterator;

/***** METHODS *****/

/* Constructor */
int vector_setup(Vector* vector, size_t capacity, size_t element_size);

/* Copy Constructor */
int vector_copy(Vector* destination, Vector* source);

/* Copy Assignment */
int vector_copy_assign(Vector* destination, Vector* source);

/* Move Constructor */
int vector_move(Vector* destination, Vector* source);

/* Move Assignment */
int vector_move_assign(Vector* destination, Vector* source);

int vector_swap(Vector* destination, Vector* source);

/* Destructor */
int vector_destroy(Vector* vector);

/* Insertion */
int vector_push_back(Vector* vector, void* element);
int vector_push_front(Vector* vector, void* element);
int vector_insert(Vector* vector, size_t index, void* element);
int vector_assign(Vector* vector, size_t index, void* element);

/* Deletion */
int vector_pop_back(Vector* vector);
int vector_pop_front(Vector* vector);
int vector_erase(Vector* vector, size_t index);
int vector_clear(Vector* vector);

/* Lookup */
void* vector_get(Vector* vector, size_t index);
const void* vector_const_get(const Vector* vector, size_t index);
void* vector_front(Vector* vector);
void* vector_back(Vector* vector);
#define VECTOR_GET_AS(type, vector_pointer, index) \
  *((type*)vector_get((vector_pointer), (index)))

/* Information */
bool vector_is_initialized(const Vector* vector);
size_t vector_byte_size(const Vector* vector);
size_t vector_free_space(const Vector* vector);
bool vector_is_empty(const Vector* vector);

/* Memory management */
int vector_resize(Vector* vector, size_t new_size);
int vector_reserve(Vector* vector, size_t minimum_capacity);
int vector_shrink_to_fit(Vector* vector);

void* vector_offset(Vector* vector, size_t index);

/* Iterators */
Iterator vector_begin(Vector* vector);
Iterator vector_end(Vector* vector);
Iterator vector_iterator(Vector* vector, size_t index);

void* iterator_get(Iterator* iterator);
#define ITERATOR_GET_AS(type, iterator) *((type*)iterator_get((iterator)))

int iterator_erase(Vector* vector, Iterator* iterator);

void iterator_increment(Iterator* iterator);
void iterator_decrement(Iterator* iterator);

void* iterator_next(Iterator* iterator);
void* iterator_previous(Iterator* iterator);

bool iterator_equals(Iterator* first, Iterator* second);
bool iterator_is_before(Iterator* first, Iterator* second);
bool iterator_is_after(Iterator* first, Iterator* second);

size_t iterator_index(Vector* vector, Iterator* iterator);

#define VECTOR_FOR_EACH(vector_pointer, iterator_name)           \
  for (Iterator(iterator_name) = vector_begin((vector_pointer)), \
      end = vector_end((vector_pointer));                        \
       !iterator_equals(&(iterator_name), &end);                 \
       iterator_increment(&(iterator_name)))

/***** PRIVATE *****/

#define MAX(a, b) ((a) > (b) ? (a) : (b))

bool _vector_should_grow(Vector* vector);
bool _vector_should_shrink(Vector* vector);

size_t _vector_free_bytes(const Vector* vector);
void* _vector_offset(Vector* vector, size_t index);
const void* _vector_const_offset(const Vector* vector, size_t index);

void _vector_assign(Vector* vector, size_t index, void* element);

int _vector_move_right(Vector* vector, size_t index);
void _vector_move_left(Vector* vector, size_t index);

int _vector_adjust_capacity(Vector* vector);
int _vector_reallocate(Vector* vector, size_t new_capacity);

void _vector_swap(size_t* first, size_t* second);
template <typename Iter>
class iter_wrap {
 public:
  typedef std::remove_cv_t<Iter> iterator_type;

  typedef typename std::iterator_traits<iterator_type>::iterator_category
      iterator_category;
  typedef typename std::iterator_traits<iterator_type>::value_type value_type;
  typedef typename std::iterator_traits<iterator_type>::difference_type
      difference_type;
  typedef typename std::iterator_traits<iterator_type>::pointer pointer;
  typedef typename std::iterator_traits<iterator_type>::reference reference;

 public:
  template <typename Iter2>
  iter_wrap(const iter_wrap<Iter2>& v) : iter_(v.iter_) {}

  reference operator*() const { return *iter_; }
  pointer operator->() const { return iter_; }

  iter_wrap& operator++() {
    ++iter_;
    return *this;
  }
  iter_wrap operator++(int) {
    iter_wrap retval = *this;
    ++(*this);
    return retval;
  }

  iter_wrap& operator--() {
    --iter_;
    return *this;
  }
  iter_wrap operator--(int) {
    iter_wrap retval = *this;
    --(*this);
    return retval;
  }

  iter_wrap operator+(difference_type n) const {
    iter_wrap retval = *this;
    retval += n;
    return retval;
  }
  iter_wrap& operator+=(difference_type n) {
    iter_ += n;
    return *this;
  }
  iter_wrap operator-(difference_type n) const {
    iter_wrap retval = *this;
    retval -= n;
    return retval;
  }
  iter_wrap& operator-=(difference_type n) {
    iter_ -= n;
    return *this;
  }

  //  bool operator==(iter_wrap other) const { return iter_ == other.iter_; }
  //  bool operator!=(iter_wrap other) const { return !(*this == other); }

  template <typename R1, typename R2>
  friend bool operator==(const iter_wrap<R1>& x, const iter_wrap<R2>& y);
  template <typename R1, typename R2>
  friend bool operator!=(const iter_wrap<R1>& x, const iter_wrap<R2>& y);
  template <typename R1, typename R2>
  friend bool operator<(const iter_wrap<R1>& x, const iter_wrap<R2>& y);
  template <typename R1, typename R2>
  friend bool operator<=(const iter_wrap<R1>& x, const iter_wrap<R2>& y);
  template <typename R1, typename R2>
  friend bool operator>(const iter_wrap<R1>& x, const iter_wrap<R2>& y);
  template <typename R1, typename R2>
  friend bool operator>=(const iter_wrap<R1>& x, const iter_wrap<R2>& y);
  template <typename R1, typename R2>
  friend auto operator+(const iter_wrap<R1>& x, const iter_wrap<R2>& y);
  template <typename R1, typename R2>
  friend auto operator-(const iter_wrap<R1>& x, const iter_wrap<R2>& y);

 public:
  explicit iter_wrap(Iter iter) : iter_(iter) {}

 public:
  iterator_type iter_;
};
template <typename R1, typename R2>
bool operator==(const iter_wrap<R1>& x, const iter_wrap<R2>& y) {
  return x.iter_ == y.iter_;
}
template <typename R1, typename R2>
bool operator!=(const iter_wrap<R1>& x, const iter_wrap<R2>& y) {
  return !(x == y);
}
template <typename R1, typename R2>
bool operator<(const iter_wrap<R1>& x, const iter_wrap<R2>& y) {
  return x.iter_ < y.iter_;
}
template <typename R1, typename R2>
bool operator<=(const iter_wrap<R1>& x, const iter_wrap<R2>& y) {
  return x.iter_ <= y.iter_;
}
template <typename R1, typename R2>
bool operator>(const iter_wrap<R1>& x, const iter_wrap<R2>& y) {
  return x.iter_ > y.iter_;
}
template <typename R1, typename R2>
bool operator>=(const iter_wrap<R1>& x, const iter_wrap<R2>& y) {
  return x.iter_ >= y.iter_;
}
template <typename R1, typename R2>
auto operator+(const iter_wrap<R1>& x, const iter_wrap<R2>& y) {
  return x.iter_ + y.iter_;
}
template <typename R1, typename R2>
auto operator-(const iter_wrap<R1>& x, const iter_wrap<R2>& y) {
  return x.iter_ - y.iter_;
}

template <typename T>
class vector {
 public:
  typedef T value_type;
  typedef value_type& reference;
  typedef value_type* pointer;
  typedef const pointer const_pointer;
  typedef const value_type& const_reference;
  typedef size_t size_type;
  typedef iter_wrap<pointer> iterator;
  typedef iter_wrap<const_pointer> const_iterator;
  typedef std::reverse_iterator<iterator> reverse_iterator;
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

  explicit vector() { vector_setup(&vector_, 0, sizeof(T)); }
  explicit vector(size_type count, const_reference value) {
    vector_setup(&vector_, count, sizeof(T));
    vector_resize(&vector_, count);
    for (size_type i = 0; i < count; i++) {
      value_type* obj = new (vector_get(&vector_, i)) T;
      *obj = value;
    }
  }
  explicit vector(size_type count) {
    vector_setup(&vector_, count, sizeof(T));
    vector_resize(&vector_, count);
    for (size_type i = 0; i < count; ++i) {
      reference obj = at(i);
      obj = value_type();
    }
  }
  template <typename InputIt>
  vector(InputIt first,
         typename std::enable_if_t<
             std::is_constructible_v<
                 value_type, typename std::iterator_traits<InputIt>::reference>,
             InputIt>
             last) {
    size_type size = last - first;
    vector_setup(&vector_, size, sizeof(T));
    vector_resize(&vector_, size);
    for (size_type i = 0; first != last; ++first, ++i) {
      reference obj = at(i);
      obj = *first;
    }
  }
  vector(const vector& other) {
    vector_setup(&vector_, other.size(), sizeof(T));
    vector_resize(&vector_, other.size());
    for (size_type i = 0; i < other.size(); ++i) {
      reference obj = at(i);
      obj = other[i];
    }
  }
  vector(vector&& other) {
    vector_move(&vector_, &other.vector_);
    ;
  }
  vector(std::initializer_list<T> init) {
    vector_setup(&vector_, init.size(), sizeof(T));
    for (auto first = init.begin(); first != init.end(); ++first) {
      emplace_back(*first);
    }
  }
  ~vector() { vector_destroy(&vector_); }
  vector& operator=(const vector& other) {
    free_all_objects();
    vector_resize(&vector_, other.size());
    for (size_type i = 0; i < other.size(); ++i) {
      reference obj = at(i);
      obj = other[i];
    }
    return *this;
  }
  vector& operator=(vector&& other) {
    free_all_objects();
    vector_move(&vector_, &other.vector_);
    return *this;
  }
  vector& operator=(std::initializer_list<T> ilist) {
    free_all_objects();
    vector_resize(&vector_, ilist.size());
    size_type i = 0;
    for (auto first = ilist.begin(); first != ilist.end(); ++first, ++i) {
      reference obj = at(i);
      obj = *first;
    }
    return *this;
  }

  template <typename InputIt>
  void assign(
      InputIt first,
      typename std::enable_if_t<
          std::is_constructible_v<
              value_type, typename std::iterator_traits<InputIt>::reference>,
          InputIt>
          last) {
    free_all_objects();
    size_type size = last - first;
    vector_resize(&vector_, size);
    for (size_type i = 0; first != last; ++first, ++i) {
      reference obj = at(i);
      obj = *first;
    }
  }
  void assign(size_type count, const value_type& value) {
    free_all_objects();
    vector_resize(&vector_, count);
    for (size_type i = 0; i < count; ++i) {
      reference obj = at(i);
      obj = value;
    }
  }
  void assign(std::initializer_list<value_type> ilist) {
    free_all_objects();
    vector_resize(&vector_, ilist.size());
    size_type i = 0;
    for (auto first = ilist.begin(); first != ilist.end(); ++first, ++i) {
      reference obj = at(i);
      obj = *first;
    }
  }

  reference at(size_type pos) {
    return *reinterpret_cast<pointer>(vector_get(&vector_, pos));
  }
  const_reference at(size_type pos) const {
    return *reinterpret_cast<const_pointer>(
        vector_get(const_cast<Vector*>(&vector_), pos));
  }
  reference operator[](size_type pos) { return at(pos); }
  const_reference operator[](size_type pos) const { return at(pos); }

  reference front() {
    return *reinterpret_cast<pointer>(vector_get(&vector_, 0));
  }
  const_reference front() const {
    *reinterpret_cast<const_pointer>(vector_get(&vector_, 0));
  }

  reference back() { return *reinterpret_cast<pointer>(vector_back(&vector_)); }
  const_reference back() const {
    return *reinterpret_cast<const_pointer>(vector_back(&vector_));
  }

  pointer data() { return reinterpret_cast<pointer>(vector_front(&vector_)); }
  const_pointer data() const {
    return reinterpret_cast<pointer>(
        vector_front(const_cast<Vector*>(&vector_)));
  }

  iterator begin() const {
    return iterator(reinterpret_cast<pointer>(
        vector_offset(const_cast<Vector*>(&vector_), 0)));
  }
  const_iterator cbegin() const {
    return const_iterator(reinterpret_cast<const_pointer>(
        vector_offset(const_cast<Vector*>(&vector_), 0)));
  }
  iterator end() const {
    return iterator(reinterpret_cast<pointer>(
        vector_offset(const_cast<Vector*>(&vector_), vector_.size)));
  }
  const_iterator cend() const {
    return const_iterator(reinterpret_cast<const_pointer>(
        vector_offset(const_cast<Vector*>(&vector_), vector_.size)));
  }
  reverse_iterator rbegin() const { return std::make_reverse_iterator(end()); }
  const_reverse_iterator crbegin() const {
    return std::make_reverse_iterator(cend());
  }
  reverse_iterator rend() const { return std::make_reverse_iterator(begin()); }
  const_reverse_iterator crend() const {
    return std::make_reverse_iterator(cbegin());
  }

  size_type size() const { return vector_.size; }
  size_type max_size() const {
    return (uint64_t)1 << (sizeof(size_type) * 8 - 1);
  }
  void reserve(size_type new_cap) { vector_reserve(&vector_, new_cap); }
  bool empty() const { return vector_is_empty(&vector_); }
  size_type capacity() const { return vector_.capacity; }
  void shrink_to_fit() { vector_shrink_to_fit(&vector_); }
  void clear() {
    free_all_objects();
    vector_clear(&vector_);
  }

  iterator insert(const_iterator pos, const value_type& value) {
    value_type obj = value;
    auto start = pos - cbegin();
    vector_insert(&vector_, start, reinterpret_cast<void*>(&obj));
    return iterator(reinterpret_cast<pointer>(
        vector_get(const_cast<Vector*>(&vector_), start)));
  }
  iterator insert(const_iterator pos, value_type&& value) {
    value_type lvalue = std::move(value);
    auto start = pos - cbegin();

    vector_insert(&vector_, start, reinterpret_cast<void*>(&lvalue));
    return iterator(reinterpret_cast<pointer>(
        vector_get(const_cast<Vector*>(&vector_), start)));
  }
  iterator insert(const_iterator pos, size_type count,
                  const value_type& value) {
    value_type obj = value;
    auto start = pos - cbegin();
    for (size_type i = 0; i < count; ++i) {
      vector_insert(&vector_, start + i, reinterpret_cast<void*>(&obj));
    }
    return iterator(reinterpret_cast<pointer>(
        vector_get(const_cast<Vector*>(&vector_), start)));
  }
  template <class InputIt>
  iterator insert(
      const_iterator pos, InputIt first,
      typename std::enable_if_t<
          std::is_constructible_v<
              value_type, typename std::iterator_traits<InputIt>::reference>,
          InputIt>
          last) {
    auto start = pos - cbegin();
    for (size_type i = 0; first < last; ++first, ++i) {
      value_type obj = *first;
      vector_insert(&vector_, start + i, &obj);
    }
    return iterator(reinterpret_cast<pointer>(
        vector_get(const_cast<Vector*>(&vector_), start)));
  }
  iterator insert(const_iterator pos, std::initializer_list<T> ilist) {
    auto start = pos - cbegin();
    auto first = ilist.begin();
    auto last = ilist.end();
    insert(pos, first, last);
    return iterator(reinterpret_cast<pointer>(
        vector_get(const_cast<Vector*>(&vector_), start)));
  }

  template <class... Args>
  iterator emplace(const_iterator pos, Args&&... args) {
    T obj(std::forward<Args>(args)...);
    auto start = pos - cbegin();
    vector_insert(&vector_, start, reinterpret_cast<void*>(&obj));
    return iterator(reinterpret_cast<pointer>(
        vector_get(const_cast<Vector*>(&vector_), start)));
  }

  // iterator erase(iterator pos) {}
  iterator erase(const_iterator pos) {
    auto index = pos - cbegin();
    free_object(index);
    vector_erase(&vector_, index);
    return index >= vector_.size
               ? end()
               : iterator(reinterpret_cast<pointer>(
                     vector_get(const_cast<Vector*>(&vector_), index)));
  }
  iterator erase(const_iterator first, const_iterator last) {
    auto pos = first - cbegin();
    for (; first < last; ++first) {
      free_object(pos);
      vector_erase(&vector_, pos);
    }
    return pos >= vector_.size ? end()
                               : iterator(reinterpret_cast<pointer>(vector_get(
                                     const_cast<Vector*>(&vector_), pos)));
  }
  //  iterator erase(const_iterator first, const_iterator last) {}
  void push_back(const value_type& value) {
    vector_push_back(&vector_,
                     const_cast<void*>(reinterpret_cast<const void*>(&value)));
  }
  void push_back(value_type&& value) {
    value_type obj = std::move(value);
    vector_push_back(&vector_, reinterpret_cast<void*>(&obj));
  }

  template <class... Args>
  reference emplace_back(Args&&... args) {
    T obj(std::forward<Args>(args)...);
    vector_push_back(&vector_, reinterpret_cast<void*>(&obj));
    return *reinterpret_cast<pointer>(vector_back(&vector_));
  }

  void pop_back() {
    if (!empty()) {
      pointer obj = reinterpret_cast<pointer>(vector_back(&vector_));
      delete obj;
      vector_pop_back(&vector_);
    }
  }

  void resize(size_type count) {
    free_range_of_objects(count, vector_.size);
    size_type old_size = vector_.size;
    vector_resize(&vector_, count);
    for (size_type i = old_size; i < count; ++i) {
      new (vector_get(&vector_, i)) T;
    }
  }
  void resize(size_type count, const value_type& value) {
    free_range_of_objects(count, vector_.size);
    size_type old_size = vector_.size;
    vector_resize(&vector_, count);
    for (size_type i = old_size; i < count; ++i) {
      value_type* obj = reinterpret_cast<pointer>(vector_get(&vector_, i));
      *obj = value;
    }
  }

 private:
  void free_all_objects() {
    for (size_type i = 0; i < vector_.size; ++i) {
      free_object(i);
    }
  }
  void free_range_of_objects(size_type first, size_type last) {
    for (; first < last; ++first) {
      free_object(first);
    }
  }
  void free_object(size_type pos) {
    value_type* obj = reinterpret_cast<pointer>(vector_get(&vector_, pos));
    delete obj;
  }

 private:
  Vector vector_;
};

template <class T>
bool operator==(const vector<T>& lhs, const vector<T>& rhs) {
  return lhs.size() == rhs.size() &&
         std::equal(lhs.begin(), lhs.end(), rhs.begin());
}
template <class T>
bool operator!=(const vector<T>& lhs, const vector<T>& rhs) {
  return !(lhs == rhs);
}

template <class T>
bool operator<(const vector<T>& lhs, const vector<T>& rhs) {
  return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(),
                                      rhs.end());
}
template <class T>
bool operator<=(const vector<T>& lhs, const vector<T>& rhs) {
  return !(rhs > lhs);
}
template <class T>
bool operator>(const vector<T>& lhs, const vector<T>& rhs) {
  return rhs < lhs;
}
template <class T>
bool operator>=(const vector<T>& lhs, const vector<T>& rhs) {
  return !(lhs < rhs);
}

}  // namespace container
}  // namespace platon

#endif