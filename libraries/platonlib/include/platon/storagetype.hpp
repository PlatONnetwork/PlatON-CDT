//
// Created by zhou.yang on 2018/11/16.
//

#pragma once

#include <string>

#include <vector>
#include <array>
#include <set>
#include <map>
#include <tuple>
#include <deque>
#include "platon/storage.hpp"

namespace platon {

    /**
     * @brief Basic type package
     * 
     * @tparam *Name Element value name, in the same contract, the name needs to be unique
     * @tparam T Element type
     */
    template <uint32_t index, typename T>
    class StorageType {
    public:
        /**
         * @brief Construct a new Storage Type object
         * 
         */
        StorageType() {
            init();
        }

        /**
         * @brief Construct a new Storage Type object
         * 
         * @param d Element
         */
        StorageType(const T& d):default_(d) {
            init();
        }

        StorageType(const StorageType<index, T>  &) = delete;
        StorageType(const StorageType<index, T> &&) = delete;
        /**
         * @brief Destroy the Storage Type object. Refresh to blockchain
         * 
         */
        ~StorageType() {
            flush();
        }



        T& operator=(const T& t) { t_ = t; }

        template<typename P>
        bool operator==(const P &t) const { return t_ == t; }
        template<typename P>
        bool operator!=(const P &t) const { return t_ == t; }
        template<typename P>
        bool operator<(const P &t) const { return t_ < t; }
        template<typename P>
        bool operator>=(const P &t) const { return t_ >= t; }
        template<typename P>
        bool operator<=(const P &t) const { return t_ <= t; }
        template<typename P>
        bool operator>(const P &t) const { return t_ > t; }

        template<typename P>
        T& operator^=(const P &t) const { t_ ^= t; return t_; }
        template<typename P>
        T operator^(const P &t) const { return t_ ^ t; }
        template<typename P>
        T& operator|=(const P &t) const { t_ |= t; return t_; }
        template<typename P>
        T operator|(const P &t) const { return t_ | t; }
        template<typename P>
        T& operator&=(const P &t) const { t_ &= t; return  t_; }
        template<typename P>
        T operator&(const P &t) const { return t_ & t; }

        T operator~() const { return ~t_; }

        T& operator<<(int offset) { t_ << offset; return t_; }
        T& operator>>(int offset) { t_ >> offset; return t_; }

        T& operator++() { return ++t_; }
        T operator++(int) { return ++t_;  }

        T& operator[](int i) { return t_[i]; }
        template<typename P>
        T& operator+=(const P &p) { t_ += p; return t_; }
        template<typename P>
        T& operator-=(const P &p) { t_ -= p; return t_; }
        T& operator*() { return t_; }
        T* operator->() { return &t_; }

        operator bool() const { return t_ ? true : false; }

        T get() const { return t_; }
        T& self() { return t_; }
    private:
        /**
         * @brief Load from blockchain
         * 
         */
        void init() {
            if (getState(index_, t_) == 0) {
                t_ = default_;
            }
        }
        /**
         * @brief Refresh to blockchain
         * 
         */
        void flush() {
            setState(index_, t_);
        }
        T default_;
        const uint32_t index_ = index;
        T t_;
    };

    template <uint32_t index>
    using Uint8 = class StorageType<index, uint8_t>;

    template <uint32_t index>
    using Int8 = class StorageType<index, int8_t>;

    template <uint32_t index>
    using Uint16 = class StorageType<index, uint16_t>;

    template <uint32_t index>
    using Int16 = class StorageType<index, int16_t>;

    template <uint32_t index>
    using Uint = class StorageType<index, uint32_t>;

    template <uint32_t index>
    using Int = class StorageType<index, int32_t>;

    template <uint32_t index>
    using Uint64 = class StorageType<index, uint64_t>;

    template <uint32_t index>
    using Int64 = class StorageType<index, int64_t>;

    template <uint32_t index>
    using Float = class StorageType<index, float>;

    template <uint32_t index>
    using Double = class StorageType<index, double>;

    template <uint32_t index>
    using String = class StorageType<index, std::string>;

    template <uint32_t index, typename T>
    using Vector = class StorageType<index, std::vector<T>>;

    template <uint32_t index, typename T>
    using Set = class StorageType<index, std::set<T>>;

    template <uint32_t index, typename K, typename V>
    using Map = class StorageType<index, std::map<K,V>>;

    template <uint32_t index,  typename T, size_t N>
    using Array = class StorageType<index, std::array<T,N>>;

    template <uint32_t index, typename... Types>
    using Tuple = class StorageType<index, std::tuple<Types...>>;

    template <uint32_t index,  typename T>
    using Deque = class StorageType<index, std::deque<T>>;

//    template <const char *name,  typename T>
//    using Queue = class StorageType<name, std::queue<T>>;
//
//
//    template <const char *name,  typename T>
//    using Stack = class StorageType<name, std::stack<T>>;

//    template <const char *name,  typename T>
//    using List = class StorageType<name, std::list<T>>;
//
//    template <const char *name,  typename K, typename V>
//    using UnorderedMap = class StorageType<name, std::unordered_map<K, V>>;
//
//    template <const char *name, typename T>
//    using UnorderedSet = class StorageType<name, std::unordered_set<T>>;

}
//    example
//    Double<name> dou;
//    dou = 2.4;
//    dou += 1.2;

