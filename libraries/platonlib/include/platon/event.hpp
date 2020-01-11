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


#define PLATON_EVENT1(NAME, TOPIC_TYPE, ...) \
    EVENT1 void M_CAT(platon_event1_, NAME)(const TOPIC_TYPE &topic, VA_F(__VA_ARGS__)) { \
        platon::emit_event1(topic, PA_F(__VA_ARGS__)); \
    }

#define PLATON_EMIT_EVENT1(NAME, TOPIC_DATA, ...) \
    M_CAT(platon_event1_, NAME)(TOPIC_DATA, __VA_ARGS__)


#define PLATON_EVENT2(NAME, TOPIC_TYPE1, TOPIC_TYPE2, ...) \
    EVENT2 void M_CAT(platon_event2_, NAME)(const TOPIC_TYPE1 &topic1, const TOPIC_TYPE2 &topic2, VA_F(__VA_ARGS__)) { \
        platon::emit_event2(topic1, topic2, PA_F(__VA_ARGS__)); \
    }

#define PLATON_EMIT_EVENT2(NAME, TOPIC1_DATA, TOPIC2_DATA, ...) \
    M_CAT(platon_event2_, NAME)(TOPIC1_DATA, TOPIC2_DATA, __VA_ARGS__)

#define PLATON_EVENT3(NAME, TOPIC_TYPE1, TOPIC_TYPE2, TOPIC_TYPE3, ...) \
    EVENT3 void M_CAT(platon_event3_, NAME)(const TOPIC_TYPE1 &topic1, const TOPIC_TYPE2 &topic2, const TOPIC_TYPE3 &topic3, VA_F(__VA_ARGS__)) { \
        platon::emit_event3(topic1, topic2, topic3, PA_F(__VA_ARGS__)); \
    }

#define PLATON_EMIT_EVENT3(NAME, TOPIC1_DATA, TOPIC2_DATA, TOPIC3_DATA, ...) \
    M_CAT(platon_event3_, NAME)(TOPIC1_DATA, TOPIC2_DATA, TOPIC3_DATA, __VA_ARGS__)

#define PLATON_EVENT4(NAME, TOPIC_TYPE1, TOPIC_TYPE2, TOPIC_TYPE3, TOPIC_TYPE4, ...) \
    EVENT4 void M_CAT(platon_event4_, NAME)(const TOPIC_TYPE1 &topic1, const TOPIC_TYPE2 &topic2, const TOPIC_TYPE3 &topic3, const TOPIC_TYPE4 &topic4, VA_F(__VA_ARGS__)) { \
        platon::emit_event4(topic1, topic2, topic3, topic4, PA_F(__VA_ARGS__)); \
    }

#define PLATON_EMIT_EVENT4(NAME, TOPIC1_DATA, TOPIC2_DATA, TOPIC3_DATA, TOPIC4_DATA, ...) \
    M_CAT(platon_event4_, NAME)(TOPIC1_DATA, TOPIC2_DATA, TOPIC3_DATA, TOPIC4_DATA, __VA_ARGS__)

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

    template<typename One_topic>
    inline bytes one_topic_data(const One_topic &one_topic){
        RLPStream stream;
        stream << one_topic;
        return stream.out();    
    }

    template<typename... Args>
    inline void emit_event(const Args &... args) {
        bytes rlpData = event_agrs(args...);
        ::platon_event(rlpData.data(), rlpData.size());
    }

    template<class Topic, typename... Args>
    inline void emit_event1(const Topic &topic, const Args &... args) {
        bytes rlp_data = event_agrs(args...);
        bytes topic_data = one_topic_data(topic);
        ::platon_event1(topic_data.data(), topic_data.size(), rlp_data.data(), rlp_data.size());
    }

    template<class Topic1, class Topic2, typename... Args>
    inline void emit_event2(const Topic1 &topic1, const Topic2 &topic2, const Args &... args) {
        bytes rlp_data = event_agrs(args...);
        bytes topic1_data = one_topic_data(topic1);
        bytes topic2_data = one_topic_data(topic2);
        ::platon_event2(topic1_data.data(), topic1_data.size(), topic2_data.data(), 
        topic2_data.size(), rlp_data.data(), rlp_data.size());
    }

    template<class Topic1, class Topic2, class Topic3, typename... Args>
    inline void emit_event3(const Topic1 &topic1, const Topic2 &topic2, const Topic3 &topic3, const Args &... args) {
        bytes rlp_data = event_agrs(args...);
        bytes topic1_data = one_topic_data(topic1);
        bytes topic2_data = one_topic_data(topic2);
        bytes topic3_data = one_topic_data(topic3);
        ::platon_event3(topic1_data.data(), topic1_data.size(), topic2_data.data(), topic2_data.size(),
        topic3_data.data(), topic3_data.size(), rlp_data.data(), rlp_data.size());
    }

    template<class Topic1, class Topic2, class Topic3, class Topic4, typename... Args>
    inline void emit_event4(const Topic1 &topic1, const Topic2 &topic2, const Topic3 &topic3, const Topic4 &topic4, const Args &... args) {
        bytes rlp_data = event_agrs(args...);
        bytes topic1_data = one_topic_data(topic1);
        bytes topic2_data = one_topic_data(topic2);
        bytes topic3_data = one_topic_data(topic3);
        bytes topic4_data = one_topic_data(topic4);
        ::platon_event4(topic1_data.data(), topic1_data.size(), topic2_data.data(), topic2_data.size(), 
        topic3_data.data(), topic3_data.size(), topic4_data.data(), topic4_data.size(), rlp_data.data(), rlp_data.size());
    }
}
