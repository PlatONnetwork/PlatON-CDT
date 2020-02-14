#include "unit_test.hpp"
#include "platon/RLP.h"
#include "platon/rlp_extend.hpp"
#include "platon/rlp_serialize.hpp"
#include "platon/event.hpp"
#include "platon/print.hpp"
#include <tuple>
#include <array>

std::vector<std::string> g_topic;
std::tuple<std::string, uint32_t> g_args;

#ifdef __cplusplus
extern "C" {
#endif

void platon_event(const uint8_t* topic, size_t topicLen, const uint8_t* args, size_t argsLen){
    if (NULL != topic && 0 != topicLen) {
        byte *topic_ptr = (byte *)topic;
        std::vector<byte> my_topic;
        for(size_t i = 0; i < topicLen; i++) {
            my_topic.push_back(*topic_ptr);
            topic_ptr++;
        }
        RLP rlp(my_topic);
        size_t num = rlp.itemCountStrict();
        platon::println(num);
        switch (num) {
            case 1:
                {
                    std::array<std::string, 1> topic_info;
                    fetch(rlp, topic_info);
                    g_topic.clear();
                    g_topic.push_back(topic_info[0]);
                    break;
                }

            case 2:
                {
                    std::array<std::string, 2> topic_info;
                    fetch(rlp, topic_info);
                    g_topic.clear();
                    g_topic.push_back(topic_info[0]);
                    g_topic.push_back(topic_info[1]);
                    break;
                }

            case 3:
                {
                    std::array<std::string, 3> topic_info;
                    fetch(rlp, topic_info);
                    g_topic.clear();
                    g_topic.push_back(topic_info[0]);
                    g_topic.push_back(topic_info[1]);
                    g_topic.push_back(topic_info[2]);
                    break;
                }

            case 4:
                {
                    std::array<std::string, 4> topic_info;
                    fetch(rlp, topic_info);
                    g_topic.clear();
                    g_topic.push_back(topic_info[0]);
                    g_topic.push_back(topic_info[1]);
                    g_topic.push_back(topic_info[2]);
                    g_topic.push_back(topic_info[3]);
                    break;
                }

            default:
                platon_panic(); 
        } 
    }

    byte *args_ptr = (byte *)args;
    std::vector<byte> my_args;
    for(size_t i = 0; i < argsLen; i++) {
        my_args.push_back(*args_ptr);
        args_ptr++;
    }
    std::tuple<std::string, uint32_t> result;
    fetch(RLP(my_args), result);
    g_args = result;
}

#ifdef __cplusplus
}
#endif

class EventTest {
    PLATON_EVENT(jatel_test, std::string, uint32_t)

    PLATON_EVENT0(jatel_test0, std::string, uint32_t)

    PLATON_EVENT1(jatel_test1, std::string, std::string, uint32_t)

    PLATON_EVENT2(jatel_test2, std::string, std::string, std::string, uint32_t)

    PLATON_EVENT3(jatel_test3, std::string, std::string, std::string, std::string, uint32_t)
public:
    void TestEvent(){PLATON_EMIT_EVENT(jatel_test, "event", 0);}

    void TestEvent0(){PLATON_EMIT_EVENT0(jatel_test0, "event0", 0);}

    void TestEvent1(){PLATON_EMIT_EVENT1(jatel_test1, "index_1", "event1", 1);}

    void TestEvent2(){PLATON_EMIT_EVENT2(jatel_test2, "index_1", "index_2", "event2", 2);}

    void TestEvent3(){PLATON_EMIT_EVENT3(jatel_test3, "index_1", "index_2", "index_3", "event3", 3);}
};

TEST_CASE(event, event) {
    EventTest event_test;
    event_test.TestEvent();
    std::tuple<std::string, uint32_t> event_args(std::make_tuple("event", 0));
    ASSERT_EQ(g_args, event_args);
}

TEST_CASE(event, event0) {
    EventTest event_test;
    event_test.TestEvent0();
    std::vector<std::string> event_topics = {"jatel_test0"};
    ASSERT_EQ(g_topic, event_topics);
    std::tuple<std::string, uint32_t> event_args(std::make_tuple("event0", 0));
    ASSERT_EQ(g_args, event_args);
}


TEST_CASE(event, event1) {
    EventTest event_test;
    event_test.TestEvent1();
    std::vector<std::string> event_topics = {"jatel_test1", "index_1"};
    ASSERT_EQ(g_topic, event_topics);
    std::tuple<std::string, uint32_t> event_args(std::make_tuple("event1", 1));
    ASSERT_EQ(g_args, event_args);
}

TEST_CASE(event, event2) {
    EventTest event_test;
    event_test.TestEvent2();
    std::vector<std::string> event_topics = {"jatel_test2", "index_1", "index_2"};
    ASSERT_EQ(g_topic, event_topics);
    std::tuple<std::string, uint32_t> event_args(std::make_tuple("event2", 2));
    ASSERT_EQ(g_args, event_args);
}

TEST_CASE(event, event3) {
    EventTest event_test;
    event_test.TestEvent3();    
    std::vector<std::string> event_topics = {"jatel_test3", "index_1", "index_2", "index_3"};
    ASSERT_EQ(g_topic, event_topics);
    std::tuple<std::string, uint32_t> event_args(std::make_tuple("event3", 3));
    ASSERT_EQ(g_args, event_args);
}

UNITTEST_MAIN() {
  RUN_TEST(event, event);
  RUN_TEST(event, event0);
  RUN_TEST(event, event1);
  RUN_TEST(event, event2);
  RUN_TEST(event, event3);
}