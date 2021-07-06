//
// Created by yangzhou on 3/2/20.
//
#include <string>
#include "common.h"
#include "print.hpp"
#include "vector_ref.h"
#pragma once

namespace platon {
class BytesBuffer {
 public:
  explicit BytesBuffer(size_t capacity = 0)
      : buffer_(nullptr), size_(0), capacity_(0) {
    reserve(capacity);
  }
  BytesBuffer(BytesBuffer &&other) noexcept { move(std::move(other)); }

  ~BytesBuffer() { destory(); }

  BytesBuffer(const BytesBuffer &) = delete;
  BytesBuffer &operator=(const BytesBuffer &) = delete;

  BytesBuffer &operator=(BytesBuffer &&other) noexcept {
    move(std::move(other));
    return *this;
  }

  void push_back(uint8_t b) {
    expand(1);
    buffer_[size_] = b;
    ++size_;
  }

  void relocate(size_t pos) {
    if (pos <= capacity_) {
      size_ = pos;
    }
  }
  void clear() { size_ = 0; }
  void resize(size_t num) {
    reserve(num);
    size_ = num;
  }
  uint8_t &back() { return *(buffer_ + (size_ - 1)); }

  size_t size() const { return size_; }
  size_t capacity() const { return capacity_; }
  uint8_t *data() { return buffer_; }
  const uint8_t *data() const { return buffer_; }
  uint8_t &operator[](size_t pos) { return *(buffer_ + pos); }
  const uint8_t &operator[](size_t pos) const { return *(buffer_ + pos); }
  void append(const uint8_t *begin, const uint8_t *end) {
    size_t size = end - begin;
    expand(size);
    ::memcpy(buffer_ + size_, begin, size);
    size_ += size;
  }

  vector_ref<uint8_t> out() const {
    return vector_ref<uint8_t>(buffer_, size_);
  }
  void reserve(size_t size) {
    if (capacity_ < size) {
      uint8_t *tmp = (uint8_t *)malloc(size);
      memcpy(tmp, buffer_, size_);
      destory();
      buffer_ = tmp;
      capacity_ = size;
    }
  }

 private:
  void expand(size_t num) {
    if (size_ + num > capacity_) {
      reserve(size_ + num);
    }
  }
  void move(BytesBuffer &&other) {
    destory();
    buffer_ = other.buffer_;
    size_ = other.size_;
    capacity_ = other.capacity_;
    other.buffer_ = nullptr;
    other.size_ = 0;
    other.capacity_ = 0;
  }
  void destory() {
    if (buffer_ != nullptr) {
      ::free(buffer_);
    }
  }

 private:
  uint8_t *buffer_;
  size_t size_;
  size_t capacity_;
};  // namespace platon
}  // namespace platon