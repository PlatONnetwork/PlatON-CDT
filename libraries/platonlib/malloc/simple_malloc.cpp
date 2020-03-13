
#include "platon/panic.hpp"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

extern unsigned char __heap_base;

#ifdef __cplusplus
}
#endif

namespace platon {
struct dsmalloc {
  inline char *align(char *ptr, uint8_t align_amt) {
    return (char *)((((size_t)ptr) + align_amt - 1) & ~(align_amt - 1));
  }

  inline size_t align(size_t ptr, uint8_t align_amt) {
    return (ptr + align_amt - 1) & ~(align_amt - 1);
  }

  static constexpr uint32_t wasm_page_size = 64 * 1024;

  dsmalloc() {
    volatile uint32_t heap_base = uint32_t(&__heap_base);
    heap = align((char *)heap_base, 8);
    last_ptr = heap;

    next_page = __builtin_wasm_memory_size(0);
  }

  char *operator()(size_t sz, uint8_t align_amt = 8) {
    if (sz == 0)
      return 0;

    size_t length_size = sizeof(size_t);
    size_t align_size = align(length_size, align_amt);
    size_t real_size = sz + align_size;

    char *ret = last_ptr;
    last_ptr = align(last_ptr + real_size, align_amt);
    size_t pages_to_alloc = real_size >> 16;
    next_page += pages_to_alloc;
    if ((next_page << 16) <= (size_t)last_ptr) {
      next_page++;
      pages_to_alloc++;
    }

    // allocate memeory
    if(pages_to_alloc > 0){
      size_t alloc_result = __builtin_wasm_memory_grow(0, pages_to_alloc);
      if (0 == alloc_result) {
        internal::platon_throw("failed to allocate pages");
      }
    }

    // set size
    memcpy(ret, &sz, length_size);

    return ret + align_size;
  }

  char *heap;
  char *last_ptr;
  size_t offset;
  size_t next_page;
};
dsmalloc _dsmalloc;
} // namespace platon

extern "C" {

void *malloc(size_t size) {
  void *ret = platon::_dsmalloc(size);
  return ret;
}

void *memset(void *, int, size_t);
void *calloc(size_t count, size_t size) {
  if (void *ptr = platon::_dsmalloc(count * size)) {
    memset(ptr, 0, count * size);
    return ptr;
  }
  return nullptr;
}

void *realloc(void *ptr, size_t size) {
  if (size == 0 || nullptr == ptr)
    return 0;

  char *old_alloc = static_cast<char *>(ptr);
  char *new_alloc = nullptr;
  size_t length_size = sizeof(size_t);
  size_t align_size = 8;
  size_t copy_size = 0;

  memcpy(&copy_size, old_alloc - align_size, length_size);
  if (size <= copy_size) {
    new_alloc = old_alloc;
  } else {
    new_alloc = platon::_dsmalloc(size);
    memcpy(new_alloc, ptr, copy_size);
  }

  return new_alloc;
}

void free(void *ptr) {}
}
