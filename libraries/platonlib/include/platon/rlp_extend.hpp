#pragma once
#include "boost/fusion/algorithm/iteration/for_each.hpp"
#include<platon/RLP.h>

namespace platon{

template <class _T>
RLPStream& RLPStream::operator << (std::vector<_T> const& _s){
    appendList(_s.size());
    for (auto const& i: _s) {
        *this << i;
    }
    return *this;
}

template <class _T, size_t S> 
RLPStream& RLPStream::operator << (std::array<_T, S> const& _s) { 
    appendList(_s.size()); 
    for (auto const& i: _s) {
        *this << i;
    }
    return *this;
}

template <class _T> 
RLPStream& RLPStream::operator << (std::set<_T> const& _s) { 
    appendList(_s.size()); 
    for (auto const& i: _s) {
        *this << i;
    }
    return *this;
}


template <class _T> 
RLPStream& RLPStream::operator << (std::unordered_set<_T> const& _s) { 
    appendList(_s.size()); 
    for (auto const& i: _s) {
        *this << i;
    }
    return *this; 
}

template <class T, class U> 
RLPStream& RLPStream::operator << (std::pair<T, U> const& _s) { 
    appendList(2); 
    *this << _s.first; 
    *this << _s.second; 
    return *this; 
}

template <class T, class U> 
RLPStream& RLPStream::operator << (std::map<T, U> const& _s) { 
    appendList(_s.size()); 
    for (auto const& i: _s) {
        *this << i;
    }
    return *this; 
}

//get data from the RLP instance
template <class T> 
void fetch(const RLP &rlp, T &value){
    value = T(rlp);
}

template <class T>
void fetch(const RLP &rlp, std::vector<T> &ret)
{
    if (rlp.isList()){
        ret.reserve(rlp.itemCount());
        for (auto const& i: rlp){
            T one;
            fetch(i, one);
            ret.push_back(one);
        }
    }else{
        platonThrow("bad cast");
    }
}


template <class T>
void fetch(const RLP &rlp, std::set<T> &ret)
{
    if (rlp.isList()){
        for (auto const& i: rlp){
            T one;
            fetch(i, one);
            ret.insert(one);
        }
    }else{
        platonThrow("bad cast");
    }
}

template <class T>
void fetch(const RLP &rlp, std::unordered_set<T> &ret)
{
    if (rlp.isList()){
        for (auto const& i: rlp){
            T one;
            fetch(i, one);
            ret.insert(one);
        }
    }else{
        platonThrow("bad cast");
    }
}

template <class T, class U>
void fetch(const RLP &rlp, std::pair<T, U> &ret)
{
    if (rlp.itemCountStrict() != 2){
        platonThrow("bad cast");
    }
    T one;
    fetch(rlp[0], one);
    ret.first = one;
    U two;
    fetch(rlp[1], two);
    ret.second = two;
}

template <class T, size_t N>
void fetch(const RLP &rlp, std::array<T, N> &ret)
{
    if (rlp.itemCountStrict() != N){
        platonThrow("bad cast");
    }
    for (size_t i = 0; i < N; ++i){
        T one;
        fetch(rlp[i], one);
        ret[i] = one;
    }
}

template <class T, class U>
void fetch(const RLP &rlp, std::map<T, U> &ret)
{
    if (rlp.isList()){
        for (auto const& i: rlp){
            std::pair<T, U> one;
            fetch(i, one);
            ret.insert(one);
        }
    }else{
        platonThrow("bad cast");
    }
}

template<typename... Args>
void fetch(const RLP &rlp, std::tuple<Args...>& t ) {
    int vect_index = 0;
    boost::fusion::for_each( t, [&]( auto& i ) {
        fetch(rlp[vect_index], i);
        vect_index++;
    });
}

}