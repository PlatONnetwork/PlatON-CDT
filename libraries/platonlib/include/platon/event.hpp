#pragma once

#include "RLP.h"
#include "chain.hpp"
#include "common.h"
#include "contract.hpp"
#include "rlp_extend.hpp"
#include "rlp_size.hpp"

#define ARG_COUNT_P1_(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) N

#define ARG_COUNT_P2_ 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0

#define ARG_COUNT_(...) ARG_COUNT_P1_(__VA_ARGS__)

#define ARG_COUNT(...) ARG_COUNT_(_, ##__VA_ARGS__, ARG_COUNT_P2_)

#define ARG_POS_P1_10(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) N
#define ARG_POS_P2_10 2, 3, 4, 5, 6, 7, 8, 9, 10
#define ARG_POS_P1_9(_1, _2, _3, _4, _5, _6, _7, _8, _9, N, ...) N
#define ARG_POS_P2_9 2, 3, 4, 5, 6, 7, 8, 9
#define ARG_POS_P1_8(_1, _2, _3, _4, _5, _6, _7, _8, N, ...) N
#define ARG_POS_P2_8 2, 3, 4, 5, 6, 7, 8
#define ARG_POS_P1_7(_1, _2, _3, _4, _5, _6, _7, N, ...) N
#define ARG_POS_P2_7 2, 3, 4, 5, 6, 7
#define ARG_POS_P1_6(_1, _2, _3, _4, _5, _6, N, ...) N
#define ARG_POS_P2_6 2, 3, 4, 5, 6
#define ARG_POS_P1_5(_1, _2, _3, _4, _5, N, ...) N
#define ARG_POS_P2_5 2, 3, 4, 5
#define ARG_POS_P1_4(_1, _2, _3, _4, N, ...) N
#define ARG_POS_P2_4 2, 3, 4
#define ARG_POS_P1_3(_1, _2, _3, N, ...) N
#define ARG_POS_P2_3 2, 3
#define ARG_POS_P1_2(_1, _2, N, ...) N
#define ARG_POS_P2_2 2
#define ARG_POS_P1_1(...) 1
#define ARG_POS_P2_1 1
#define ARG_POS10_(...) ARG_POS_P1_10(__VA_ARGS__)
#define ARG_POS10(...) ARG_POS10_(_, ##__VA_ARGS__, ARG_POS_P2_10)
#define ARG_POS9_(...) ARG_POS_P1_9(__VA_ARGS__)
#define ARG_POS9(...) ARG_POS9_(_, ##__VA_ARGS__, ARG_POS_P2_9)
#define ARG_POS8_(...) ARG_POS_P1_8(__VA_ARGS__)
#define ARG_POS8(...) ARG_POS8_(_, ##__VA_ARGS__, ARG_POS_P2_8)
#define ARG_POS7_(...) ARG_POS_P1_7(__VA_ARGS__)
#define ARG_POS7(...) ARG_POS7_(_, ##__VA_ARGS__, ARG_POS_P2_7)
#define ARG_POS6_(...) ARG_POS_P1_6(__VA_ARGS__)
#define ARG_POS6(...) ARG_POS6_(_, ##__VA_ARGS__, ARG_POS_P2_6)
#define ARG_POS5_(...) ARG_POS_P1_5(__VA_ARGS__)
#define ARG_POS5(...) ARG_POS5_(_, ##__VA_ARGS__, ARG_POS_P2_5)
#define ARG_POS4_(...) ARG_POS_P1_4(__VA_ARGS__)
#define ARG_POS4(...) ARG_POS4_(_, ##__VA_ARGS__, ARG_POS_P2_4)
#define ARG_POS3_(...) ARG_POS_P1_3(__VA_ARGS__)
#define ARG_POS3(...) ARG_POS3_(_, ##__VA_ARGS__, ARG_POS_P2_3)
#define ARG_POS2_(...) ARG_POS_P1_2(__VA_ARGS__)
#define ARG_POS2(...) ARG_POS2_(_, ##__VA_ARGS__, ARG_POS_P2_2)
#define ARG_POS1_(...) ARG_POS_P1_1(__VA_ARGS__)
#define ARG_POS1(...) ARG_POS1_(_, ##__VA_ARGS__, ARG_POS_P2_1)

#define M_CAT(a, b) M_CAT_(a, b)
#define M_CAT_(a, b) a##b

#define VA_F(...)                     \
  M_CAT(func, ARG_COUNT(__VA_ARGS__)) \
  (1, ARG_COUNT(__VA_ARGS__), __VA_ARGS__)

#define PA_F(...)                       \
  M_CAT(params, ARG_COUNT(__VA_ARGS__)) \
  (1, ARG_COUNT(__VA_ARGS__), __VA_ARGS__)

#define params1(N, total, a) M_CAT(arg, N)
#define params2(N, total, a, ...) \
  M_CAT(arg, N), params1(M_CAT(ARG_POS, total)(__VA_ARGS__), total, __VA_ARGS__)
#define params3(N, total, a, ...) \
  M_CAT(arg, N), params2(M_CAT(ARG_POS, total)(__VA_ARGS__), total, __VA_ARGS__)
#define params4(N, total, a, ...) \
  M_CAT(arg, N), params3(M_CAT(ARG_POS, total)(__VA_ARGS__), total, __VA_ARGS__)
#define params5(N, total, a, ...) \
  M_CAT(arg, N), params4(M_CAT(ARG_POS, total)(__VA_ARGS__), total, __VA_ARGS__)
#define params6(N, total, a, ...) \
  M_CAT(arg, N), params5(M_CAT(ARG_POS, total)(__VA_ARGS__), total, __VA_ARGS__)
#define params7(N, total, a, ...) \
  M_CAT(arg, N), params6(M_CAT(ARG_POS, total)(__VA_ARGS__), total, __VA_ARGS__)
#define params8(N, total, a, ...) \
  M_CAT(arg, N), params7(M_CAT(ARG_POS, total)(__VA_ARGS__), total, __VA_ARGS__)
#define params9(N, total, a, ...) \
  M_CAT(arg, N), params8(M_CAT(ARG_POS, total)(__VA_ARGS__), total, __VA_ARGS__)
#define params10(N, total, a, ...) \
  M_CAT(arg, N), params9(M_CAT(ARG_POS, total)(__VA_ARGS__), total, __VA_ARGS__)

#define func1(N, total, a) a M_CAT(arg, N)
#define func2(N, total, a, ...) \
  a M_CAT(arg, N), func1(M_CAT(ARG_POS, total)(__VA_ARGS__), total, __VA_ARGS__)
#define func3(N, total, a, ...) \
  a M_CAT(arg, N), func2(M_CAT(ARG_POS, total)(__VA_ARGS__), total, __VA_ARGS__)
#define func4(N, total, a, ...) \
  a M_CAT(arg, N), func3(M_CAT(ARG_POS, total)(__VA_ARGS__), total, __VA_ARGS__)
#define func5(N, total, a, ...) \
  a M_CAT(arg, N), func4(M_CAT(ARG_POS, total)(__VA_ARGS__), total, __VA_ARGS__)
#define func6(N, total, a, ...) \
  a M_CAT(arg, N), func5(M_CAT(ARG_POS, total)(__VA_ARGS__), total, __VA_ARGS__)
#define func7(N, total, a, ...) \
  a M_CAT(arg, N), func6(M_CAT(ARG_POS, total)(__VA_ARGS__), total, __VA_ARGS__)
#define func8(N, total, a, ...) \
  a M_CAT(arg, N), func7(M_CAT(ARG_POS, total)(__VA_ARGS__), total, __VA_ARGS__)
#define func9(N, total, a, ...) \
  a M_CAT(arg, N), func8(M_CAT(ARG_POS, total)(__VA_ARGS__), total, __VA_ARGS__)
#define func10(N, total, a, ...) \
  a M_CAT(arg, N), func9(M_CAT(ARG_POS, total)(__VA_ARGS__), total, __VA_ARGS__)

#define PLATON_EVENT0(NAME, ...)                   \
  EVENT void NAME(VA_F(__VA_ARGS__)) {             \
    platon::emit_event0(#NAME, PA_F(__VA_ARGS__)); \
  }

#define PLATON_EMIT_EVENT0(NAME, ...) NAME(__VA_ARGS__)

#define PLATON_EVENT1(NAME, TOPIC_TYPE, ...)                     \
  EVENT1 void NAME(const TOPIC_TYPE &topic, VA_F(__VA_ARGS__)) { \
    platon::emit_event1(#NAME, topic, PA_F(__VA_ARGS__));        \
  }

#define PLATON_EMIT_EVENT1(NAME, TOPIC_DATA, ...) NAME(TOPIC_DATA, __VA_ARGS__)

#define PLATON_EVENT2(NAME, TOPIC_TYPE1, TOPIC_TYPE2, ...)               \
  EVENT2 void NAME(const TOPIC_TYPE1 &topic1, const TOPIC_TYPE2 &topic2, \
                   VA_F(__VA_ARGS__)) {                                  \
    platon::emit_event2(#NAME, topic1, topic2, PA_F(__VA_ARGS__));       \
  }

#define PLATON_EMIT_EVENT2(NAME, TOPIC1_DATA, TOPIC2_DATA, ...) \
  NAME(TOPIC1_DATA, TOPIC2_DATA, __VA_ARGS__)

#define PLATON_EVENT3(NAME, TOPIC_TYPE1, TOPIC_TYPE2, TOPIC_TYPE3, ...)    \
  EVENT3 void NAME(const TOPIC_TYPE1 &topic1, const TOPIC_TYPE2 &topic2,   \
                   const TOPIC_TYPE3 &topic3, VA_F(__VA_ARGS__)) {         \
    platon::emit_event3(#NAME, topic1, topic2, topic3, PA_F(__VA_ARGS__)); \
  }

#define PLATON_EMIT_EVENT3(NAME, TOPIC1_DATA, TOPIC2_DATA, TOPIC3_DATA, ...) \
  NAME(TOPIC1_DATA, TOPIC2_DATA, TOPIC3_DATA, __VA_ARGS__)

namespace platon {

template <typename T, bool>
struct event_type {
  using type = bytes;
  static type event_data_convert(const T &data) {
    RLPStream stream;
    stream.reserve(pack_size(data));
    stream << data;
    const bytesRef result = stream.out();
    std::vector<byte> hash;
    hash.resize(32);
    ::platon_sha3(result.data(), result.size(), hash.data(), hash.size());
    return hash;
  }
};

template <typename T>
struct event_type<T, true> {
  using type = const T &;
  static type event_data_convert(const T &data) { return data; }
};

template <typename T,
          bool is_number = std::numeric_limits<std::decay_t<T>>::is_integer ||
                           std::numeric_limits<std::decay_t<T>>::is_iec559>
typename event_type<T, is_number>::type event_data_convert(const T &data) {
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
inline void event_args(RLPStream &stream, const Args &... args) {
  std::tuple<Args...> tuple_args = std::make_tuple(args...);
  stream.reserve(pack_size(tuple_args));
  stream << tuple_args;
  //  return stream.out();
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
  RLPStream stream;
  event_args(stream, args...);
  bytesRef topic_data = stream.out();
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
  auto event_sign = event_data_convert(name);
  stream.reserve(pack_size(event_sign));
  stream << event_sign;
  const bytesRef topic_data = stream.out();
  RLPStream args_stream;
  event_args(args_stream, args...);
  bytesRef args_data = args_stream.out();
  ::platon_event(topic_data.data(), topic_data.size(), args_data.data(),
                 args_data.size());
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
inline void emit_event1(const std::string &name, const Topic &topic,
                        const Args &... args) {
  RLPStream stream(2);
  auto event_sign = event_data_convert(name);
  auto topic1_data = event_data_convert(topic);
  RLPSize rlps;
  rlps << event_sign << topic1_data;
  stream.reserve(rlps.size());
  stream << event_sign << topic1_data;
  const bytesRef topic_data = stream.out();
  RLPStream args_stream;
  event_args(args_stream, args...);
  bytesRef rlp_data = args_stream.out();
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
inline void emit_event2(const std::string &name, const Topic1 &topic1,
                        const Topic2 &topic2, const Args &... args) {
  RLPStream stream(3);
  auto event_sign = event_data_convert(name);
  auto topic1_data = event_data_convert(topic1);
  auto topic2_data = event_data_convert(topic2);
  RLPSize rlps;
  rlps << event_sign << topic1_data << topic2_data;
  stream.reserve(rlps.size());
  stream << event_sign << topic1_data << topic2_data;
  const bytesRef topic_data = stream.out();
  RLPStream args_stream;
  event_args(args_stream, args...);
  bytesRef rlp_data = args_stream.out();
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
inline void emit_event3(const std::string &name, const Topic1 &topic1,
                        const Topic2 &topic2, const Topic3 &topic3,
                        const Args &... args) {
  RLPStream stream(4);
  auto event_sign = event_data_convert(name);
  auto topic1_data = event_data_convert(topic1);
  auto topic2_data = event_data_convert(topic2);
  auto topic3_data = event_data_convert(topic3);
  RLPSize rlps;
  rlps << event_sign << topic1_data << topic2_data << topic3_data;
  stream.reserve(rlps.size());
  stream << event_sign << topic1_data << topic2_data << topic3_data;
  const bytesRef topic_data = stream.out();
  RLPStream args_stream;
  event_args(args_stream, args...);
  bytesRef rlp_data = args_stream.out();
  ::platon_event(topic_data.data(), topic_data.size(), rlp_data.data(),
                 rlp_data.size());
}
}  // namespace platon
