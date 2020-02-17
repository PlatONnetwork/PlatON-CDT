#pragma once

#include "RLP.h"
#include "chain.hpp"
#include "common.h"
#include "contract.hpp"
#include "rlp_extend.hpp"

#define ARG_COUNT_P1_(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) N

#define ARG_COUNT_P2_ 9, 8, 7, 6, 5, 4, 3, 2, 1, 0

#define ARG_COUNT_(...) ARG_COUNT_P1_(__VA_ARGS__)

#define ARG_COUNT(...) ARG_COUNT_(_, ##__VA_ARGS__, ARG_COUNT_P2_)

#define M_CAT(a, b) M_CAT_(a, b)
#define M_CAT_(a, b) a##b

#define VA_F(...) M_CAT(func, ARG_COUNT(__VA_ARGS__))(__VA_ARGS__)

#define PA_F(...) M_CAT(params, ARG_COUNT(__VA_ARGS__))(__VA_ARGS__)

#define params1(a) arg1
#define params2(a, ...) arg2, params1(__VA_ARGS__)
#define params3(a, ...) arg3, params2(__VA_ARGS__)
#define params4(a, ...) arg4, params3(__VA_ARGS__)
#define params5(a, ...) arg5, params4(__VA_ARGS__)
#define params6(a, ...) arg6, params5(__VA_ARGS__)
#define params7(a, ...) arg7, params6(__VA_ARGS__)
#define params8(a, ...) arg8, params7(__VA_ARGS__)
#define params9(a, ...) arg9, params8(__VA_ARGS__)
#define params10(a, ...) arg10, params9(__VA_ARGS__)

#define func1(a) a arg1
#define func2(a, ...) a arg2, func1(__VA_ARGS__)
#define func3(a, ...) a arg3, func2(__VA_ARGS__)
#define func4(a, ...) a arg4, func3(__VA_ARGS__)
#define func5(a, ...) a arg5, func4(__VA_ARGS__)
#define func6(a, ...) a arg6, func5(__VA_ARGS__)
#define func7(a, ...) a arg7, func6(__VA_ARGS__)
#define func8(a, ...) a arg8, func7(__VA_ARGS__)
#define func9(a, ...) a arg9, func8(__VA_ARGS__)
#define func10(a, ...) a arg10, func9(__VA_ARGS__)

#define PLATON_EVENT0(NAME, ...)                               \
  EVENT void NAME(VA_F(__VA_ARGS__)) { \
    platon::emit_event0(#NAME, PA_F(__VA_ARGS__));              \
  }

#define PLATON_EMIT_EVENT0(NAME, ...) NAME(__VA_ARGS__)

#define PLATON_EVENT1(NAME, TOPIC_TYPE, ...)                       \
  EVENT1 void NAME(const TOPIC_TYPE &topic, \
                                          VA_F(__VA_ARGS__)) {     \
    platon::emit_event1(#NAME, topic, PA_F(__VA_ARGS__));                 \
  }

#define PLATON_EMIT_EVENT1(NAME, TOPIC_DATA, ...) \
  NAME(TOPIC_DATA, __VA_ARGS__)

#define PLATON_EVENT2(NAME, TOPIC_TYPE1, TOPIC_TYPE2, ...)           \
  EVENT2 void NAME(const TOPIC_TYPE1 &topic1, \
                                          const TOPIC_TYPE2 &topic2, \
                                          VA_F(__VA_ARGS__)) {       \
    platon::emit_event2(#NAME, topic1, topic2, PA_F(__VA_ARGS__));          \
  }

#define PLATON_EMIT_EVENT2(NAME, TOPIC1_DATA, TOPIC2_DATA, ...) \
  NAME(TOPIC1_DATA, TOPIC2_DATA, __VA_ARGS__)

#define PLATON_EVENT3(NAME, TOPIC_TYPE1, TOPIC_TYPE2, TOPIC_TYPE3, ...) \
  EVENT3 void NAME(                              \
      const TOPIC_TYPE1 &topic1, const TOPIC_TYPE2 &topic2,             \
      const TOPIC_TYPE3 &topic3, VA_F(__VA_ARGS__)) {                   \
    platon::emit_event3(#NAME, topic1, topic2, topic3, PA_F(__VA_ARGS__));     \
  }

#define PLATON_EMIT_EVENT3(NAME, TOPIC1_DATA, TOPIC2_DATA, TOPIC3_DATA, ...) \
  NAME(TOPIC1_DATA, TOPIC2_DATA, TOPIC3_DATA, __VA_ARGS__)


namespace platon {

template <typename T, bool>
struct event_type {
  using type = bytes;
  static type event_data_convert(const T &data){
      RLPStream stream;
      stream << data;
      std::vector<byte> result = stream.out();
      std::vector<byte> hash;
      hash.resize(32);
      ::platon_sha3(result.data(), result.size(), hash.data(), hash.size());
      return hash;
  }
};

template <typename T>
struct event_type<T, true> {
  using type = T;
  static type event_data_convert(const T &data){
    return data;
}
};

template <typename T,
          bool is_number  = std::numeric_limits<std::decay_t<T>>::is_integer ||
          std::numeric_limits<std::decay_t<T>>::is_iec559>
typename event_type<T, is_number>::type event_data_convert(const T& data){
  return event_type<T, is_number>::event_data_convert(data);
}

/**
 * @brief Gets rlp-encoded data for any number and type parameters
 *
 * @param args any number and type parameters
 *             
 * @return An array of bytes after RLP encodes data
 */
template <typename... Args>
inline bytes event_args(const Args &... args) {
  std::tuple<Args...> tuple_args = std::make_tuple(args...);
  RLPStream stream;
  stream << tuple_args;
  return stream.out();
} 

/**
 * @brief Send events that are unindexed and anonymous
 *
 * @param args Any number of event parameters of any type
 *             
 * @return void
 */
template <typename... Args>
inline void emit_event(const Args &... args) {
  bytes topic_data = event_args(args...);
  ::platon_event(NULL, 0, topic_data.data(), topic_data.size());
}

/**
 * @brief Send event items that are not indexed
 *
 * @param name The name of the event
 * @param args Any number of event parameters of any type
 *          
 * @return void
 */
template <typename... Args>
inline void emit_event0(const std::string &name, const Args &... args) {
  RLPStream stream(1);
  stream << event_data_convert(name);
  bytes topic_data = stream.out();
  bytes args_data = event_args(args...);
  ::platon_event(topic_data.data(), topic_data.size(), args_data.data(), args_data.size());
}

/**
 * @brief Sends an event that has only one index entry
 *
 * @param name The name of the event
 * @param topic Event index value
 * @param args Any number of event parameters of any type
 *             
 * @return void
 */
template <class Topic, typename... Args>
inline void emit_event1(const std::string &name, const Topic &topic, const Args &... args) {
  RLPStream stream(2);
  stream << event_data_convert(name) << event_data_convert(topic);
  bytes topic_data = stream.out();
  bytes rlp_data = event_args(args...);
  ::platon_event(topic_data.data(), topic_data.size(), rlp_data.data(),
                 rlp_data.size());
}

/**
 * @brief Sends an event with two index values
 *
 * @param name The name of the event
 * @param topic1 The first index value of the event
 * @param topic2 The second index value of the event
 * @param args Any number of event parameters of any type
 *             
 * @return void
 */
template <class Topic1, class Topic2, typename... Args>
inline void emit_event2(const std::string &name, const Topic1 &topic1, const Topic2 &topic2,
                        const Args &... args) {
  RLPStream stream(3);
  stream << event_data_convert(name) << event_data_convert(topic1) << event_data_convert(topic2);
  bytes topic_data = stream.out();
  bytes rlp_data = event_args(args...);
  ::platon_event(topic_data.data(), topic_data.size(), rlp_data.data(),
                 rlp_data.size());
}

/**
 * @brief Sends an event with three event index values
 *
 * @param name The name of the event
 * @param topic1 The first index value of the event
 * @param topic2 The second index value of the event
 * @param topic3 The third index value of the event
 * @param args Any number of event parameters of any type
 *             
 * @return void
 */
template <class Topic1, class Topic2, class Topic3, typename... Args>
inline void emit_event3(const std::string &name, const Topic1 &topic1, const Topic2 &topic2,
                        const Topic3 &topic3, const Args &... args) {
  RLPStream stream(4);
  stream << event_data_convert(name) << event_data_convert(topic1) << 
    event_data_convert(topic2) << event_data_convert(topic3);
  bytes topic_data = stream.out();
  bytes rlp_data = event_args(args...);
  ::platon_event(topic_data.data(), topic_data.size(), rlp_data.data(),
                 rlp_data.size());
}
}  // namespace platon
