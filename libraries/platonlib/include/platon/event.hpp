#pragma once

#include "common.h"
#include "RLP.h"
#include "rlp_extend.hpp"
#include "contract.hpp"

#define ARG_COUNT_P1_(\
  _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) \
  N

#define ARG_COUNT_P2_ \
  9, 8, 7, 6, 5, 4, 3, 2, 1, 0

#define ARG_COUNT_(...) \
  ARG_COUNT_P1_(__VA_ARGS__)

#define ARG_COUNT(...) \
  ARG_COUNT_(_, ##__VA_ARGS__, ARG_COUNT_P2_)

#define M_CAT(a, b) M_CAT_(a, b)
#define M_CAT_(a, b) a##b

#define VA_F(...) \
  M_CAT(func, ARG_COUNT(__VA_ARGS__)) (__VA_ARGS__)

#define PA_F(...) \
  M_CAT(params, ARG_COUNT(__VA_ARGS__)) (__VA_ARGS__)

#define params1(a)   arg1
#define params2(a, ...)  arg2, params1(__VA_ARGS__)
#define params3(a, ...)  arg3, params2(__VA_ARGS__)
#define params4(a, ...)  arg4, params3(__VA_ARGS__)
#define params5(a, ...)  arg5, params4(__VA_ARGS__)
#define params6(a, ...)  arg6, params5(__VA_ARGS__)
#define params7(a, ...)  arg7, params6(__VA_ARGS__)
#define params8(a, ...)  arg8, params7(__VA_ARGS__)
#define params9(a, ...)  arg9, params8(__VA_ARGS__)
#define params10(a, ...) arg10, params9(__VA_ARGS__)

#define func1(a)  a arg1
#define func2(a, ...) a arg2, func1(__VA_ARGS__)
#define func3(a, ...) a arg3, func2(__VA_ARGS__)
#define func4(a, ...) a arg4, func3(__VA_ARGS__)
#define func5(a, ...) a arg5, func4(__VA_ARGS__)
#define func6(a, ...) a arg6, func5(__VA_ARGS__)
#define func7(a, ...) a arg7, func6(__VA_ARGS__)
#define func8(a, ...) a arg8, func7(__VA_ARGS__)
#define func9(a, ...) a arg9, func8(__VA_ARGS__)
#define func10(a, ...) a arg10, func9(__VA_ARGS__)


#define PLATON_EVENT(NAME, ...) \
    EVENT void M_CAT(platon_event_, NAME)(VA_F(__VA_ARGS__)) { \
        platon::emit_event(PA_F(__VA_ARGS__)); \
    }

#define PLATON_EMIT_EVENT(NAME, ...) \
    M_CAT(platon_event_, NAME)(__VA_ARGS__)


#define PLATON_EVENT1(NAME, TOPIC, ...) \
    EVENT void M_CAT(platon_event_, NAME)(const std::string &topic, VA_F(__VA_ARGS__)) { \
        platon::emit_event1(topic, PA_F(__VA_ARGS__)); \
    }

#define PLATON_EMIT_EVENT1(NAME, TOPIC, ...) \
    M_CAT(platon_event_, NAME)(#TOPIC, __VA_ARGS__)


#define PLATON_EVENT2(NAME, TOPIC1, TOPIC2, ...) \
    EVENT void M_CAT(platon_event_, NAME)(const std::string &topic1, const std::string &topic2, VA_F(__VA_ARGS__)) { \
        platon::emit_event2(topic1, topic2, PA_F(__VA_ARGS__)); \
    }

#define PLATON_EMIT_EVENT2(NAME, TOPIC1, TOPIC2, ...) \
    M_CAT(platon_event_, NAME)(#TOPIC1, #TOPIC2, __VA_ARGS__)

#define PLATON_EVENT3(NAME, TOPIC1, TOPIC2, TOPIC3, ...) \
    EVENT void M_CAT(platon_event_, NAME)(const std::string &topic1, const std::string &topic2, const std::string &topic3, VA_F(__VA_ARGS__)) { \
        platon::emit_event3(topic1, topic2, topic3, PA_F(__VA_ARGS__)); \
    }

#define PLATON_EMIT_EVENT3(NAME, TOPIC1, TOPIC2, TOPIC3, ...) \
    M_CAT(platon_event_, NAME)(#TOPIC1, #TOPIC2, #TOPIC3, __VA_ARGS__)

#define PLATON_EVENT4(NAME, TOPIC1, TOPIC2, TOPIC3, TOPIC4, ...) \
    EVENT void M_CAT(platon_event_, NAME)(const std::string &topic1, const std::string &topic2, const std::string &topic3, const std::string &topic4, VA_F(__VA_ARGS__)) { \
        platon::emit_event4(topic1, topic2, topic3, topic4, PA_F(__VA_ARGS__)); \
    }

#define PLATON_EMIT_EVENT4(NAME, TOPIC1, TOPIC2, TOPIC3, TOPIC4, ...) \
    M_CAT(platon_event_, NAME)(#TOPIC1, #TOPIC2, #TOPIC3, #TOPIC4, __VA_ARGS__)

#ifdef __cplusplus
extern "C" {
#endif
    void platon_event(const uint8_t* args, size_t argsLen);
    void platon_event1(const uint8_t* topic, size_t topicLen, const uint8_t* args, size_t argsLen);
    void platon_event2(const uint8_t* topic1, size_t topic1Len, const uint8_t* topic2, size_t topic2Len, const uint8_t* args, size_t argsLen);
    void platon_event3(const uint8_t* topic1, size_t topic1Len, const uint8_t* topic2, size_t topic2Len, const uint8_t* topic3, size_t topic3Len, uint8_t* args, size_t argsLen);
    void platon_event4(const uint8_t* topic1, size_t topic1Len, const uint8_t* topic2, size_t topic2Len, 
    const uint8_t* topic3, size_t topic3Len, const uint8_t* topic4, size_t topic4Len, uint8_t* args, size_t argsLen);
#ifdef __cplusplus
}
#endif

namespace platon {

    template<typename... Args>
    inline bytes event_agrs(const Args &... args) {
        std::tuple<Args...> tuple_args = std::make_tuple(args...);
        RLPStream stream;
        stream << tuple_args;
        return stream.out();
    }

    template<typename... Args>
    inline void emit_event(const Args &... args) {
        bytes rlpData = event_agrs(args...);
        ::platon_event(rlpData.data(), rlpData.size());
    }

    template<typename... Args>
    inline void emit_event1(const std::string &topic, const Args &... args) {
        bytes rlpData = event_agrs(args...);
        ::platon_event1((const uint8_t*)topic.data(), topic.length(), rlpData.data(), rlpData.size());
    }

    template<typename... Args>
    inline void emit_event2(const std::string &topic1, const std::string &topic2, const Args &... args) {
        bytes rlpData = event_agrs(args...);
        ::platon_event2((const uint8_t*)topic1.data(), topic1.length(), (const uint8_t*)topic2.data(), 
        topic2.length(), rlpData.data(), rlpData.size());
    }

    template<typename... Args>
    inline void emit_event3(const std::string &topic1, const std::string &topic2, const std::string &topic3, const Args &... args) {
        bytes rlpData = event_agrs(args...);
        ::platon_event3((const uint8_t*)topic1.data(), topic1.length(), (const uint8_t*)topic2.data(), topic2.length(),
        (const uint8_t*)topic3.data(), topic3.length(), rlpData.data(), rlpData.size());
    }

    template<typename... Args>
    inline void emit_event4(const std::string &topic1, const std::string &topic2, const std::string &topic3, const std::string &topic4, const Args &... args) {
        bytes rlpData = event_agrs(args...);
        ::platon_event4((const uint8_t*)topic1.data(), topic1.length(), (const uint8_t*)topic2.data(), topic2.length(), 
        (const uint8_t*)topic3.data(), topic3.length(), (const uint8_t*)topic4.data(), topic4.length(), rlpData.data(), rlpData.size());
    }
}
