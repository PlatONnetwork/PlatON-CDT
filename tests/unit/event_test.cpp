#include "platon/event.hpp"
#include <array>
#include <tuple>
#include "platon/RLP.h"
#include "platon/print.hpp"
#include "platon/rlp_extend.hpp"
#include "platon/rlp_serialize.hpp"
#include "unit_test.hpp"

using namespace platon;

std::vector<bytes> g_topic;
std::tuple<std::string, int> g_args;

#ifdef __cplusplus
extern "C" {
#endif

void platon_sha3(const uint8_t *src, size_t srcLen, uint8_t *dest,
                 size_t destLen) {
  for (int i = 0; i < destLen; i++) {
    if (i < srcLen) {
      dest[i] = src[i];
    } else {
      dest[i] = 0;
    }
  }
}

void platon_event(const uint8_t *topic, size_t topicLen, const uint8_t *args,
                  size_t argsLen) {
  if (NULL != topic && 0 != topicLen) {
    byte *topic_ptr = (byte *)topic;
    std::vector<byte> my_topic;
    for (size_t i = 0; i < topicLen; i++) {
      my_topic.push_back(*topic_ptr);
      topic_ptr++;
    }
    RLP rlp(my_topic);
    size_t num = rlp.itemCountStrict();
    platon::println(num);
    switch (num) {
      case 1: {
        std::array<bytes, 1> topic_info;
        fetch(rlp, topic_info);
        g_topic.clear();
        g_topic.push_back(topic_info[0]);
        break;
      }

      case 2: {
        std::array<bytes, 2> topic_info;
        fetch(rlp, topic_info);
        g_topic.clear();
        g_topic.push_back(topic_info[0]);
        g_topic.push_back(topic_info[1]);
        break;
      }

      case 3: {
        std::array<bytes, 3> topic_info;
        fetch(rlp, topic_info);
        g_topic.clear();
        g_topic.push_back(topic_info[0]);
        g_topic.push_back(topic_info[1]);
        g_topic.push_back(topic_info[2]);
        break;
      }

      case 4: {
        std::array<bytes, 4> topic_info;
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
  for (size_t i = 0; i < argsLen; i++) {
    my_args.push_back(*args_ptr);
    args_ptr++;
  }
  std::tuple<std::string, int> result;
  fetch(RLP(my_args), result);
  g_args = result;
}

#ifdef __cplusplus
}
#endif

class EventTest {
  PLATON_EVENT0(jatel_test0, std::string, int)

  PLATON_EVENT1(jatel_test1, std::string, std::string, int)

  PLATON_EVENT2(jatel_test2, std::string, std::string, std::string, int)

  PLATON_EVENT3(jatel_test3, std::string, std::string, std::string, std::string,
                int)
 public:
  void TestEvent0() { PLATON_EMIT_EVENT0(jatel_test0, "event0", 0); }

  void TestEvent1() { PLATON_EMIT_EVENT1(jatel_test1, "index_1", "event1", 1); }

  void TestEvent2() {
    PLATON_EMIT_EVENT2(jatel_test2, "index_1", "index_2", "event2", 2);
  }

  void TestEvent3() {
    PLATON_EMIT_EVENT3(jatel_test3, "index_1", "index_2", "index_3", "event3",
                       3);
  }
};

TEST_CASE(event, event0) {
  EventTest event_test;
  event_test.TestEvent0();
  std::vector<bytes> event_topics = {event_data_convert("jatel_test0")};
  ASSERT_EQ(g_topic, event_topics);
  std::tuple<std::string, int> event_args(std::make_tuple("event0", 0));
  ASSERT_EQ(g_args, event_args);
}

TEST_CASE(event, event1) {
  EventTest event_test;
  event_test.TestEvent1();
  std::vector<bytes> event_topics = {event_data_convert("jatel_test1"),
                                     event_data_convert("index_1")};
  ASSERT_EQ(g_topic, event_topics);
  std::tuple<std::string, int> event_args(std::make_tuple("event1", 1));
  ASSERT_EQ(g_args, event_args);
}

TEST_CASE(event, event2) {
  EventTest event_test;
  event_test.TestEvent2();
  std::vector<bytes> event_topics = {event_data_convert("jatel_test2"),
                                     event_data_convert("index_1"),
                                     event_data_convert("index_2")};
  ASSERT_EQ(g_topic, event_topics);
  std::tuple<std::string, int> event_args(std::make_tuple("event2", 2));
  ASSERT_EQ(g_args, event_args);
}

TEST_CASE(event, event3) {
  EventTest event_test;
  event_test.TestEvent3();
  std::vector<bytes> event_topics = {
      event_data_convert("jatel_test3"), event_data_convert("index_1"),
      event_data_convert("index_2"), event_data_convert("index_3")};
  ASSERT_EQ(g_topic, event_topics);
  std::tuple<std::string, int> event_args(std::make_tuple("event3", 3));
  ASSERT_EQ(g_args, event_args);
}

TEST_CASE(convert, bool) {
  bool b_data = true;
  RLPStream stream;
  auto info = event_data_convert(b_data);
  stream << info;
  bytesRef result = stream.out();
  b_data = false;
  fetch(RLP(result), b_data);
  ASSERT_EQ(b_data, true);
}

TEST_CASE(convert, int) {
  int int_data = -2;
  RLPStream stream;
  auto info = event_data_convert(int_data);
  stream << info;
  bytesRef result = stream.out();
  int_data = 0;
  fetch(RLP(result), int_data);
  ASSERT_EQ(int_data, -2);
}

TEST_CASE(convert, u128) {
  u128 u128_data = 256;
  RLPStream stream;
  stream << event_data_convert(u128_data);
  bytesRef result = stream.out();
  u128_data = 0;
  fetch(RLP(result), u128_data);
  ASSERT_EQ(u128_data, 256);
}

TEST_CASE(convert, string) {
  std::string str_data = "test";
  RLPStream stream;
  auto info = event_data_convert(str_data);
  stream << info;
  bytesRef result = stream.out();
  bytes fetch_data;
  fetch(RLP(result), fetch_data);
  ASSERT_EQ(info, fetch_data);
}

TEST_CASE(convert, vector) {
  std::vector<std::string> vect_data = {"abc", "def"};
  RLPStream stream;
  auto info = event_data_convert(vect_data);
  stream << info;
  bytesRef result = stream.out();
  bytes fetch_data;
  fetch(RLP(result), fetch_data);
  ASSERT_EQ(info, fetch_data);
}

UNITTEST_MAIN() {
  RUN_TEST(event, event0);
  RUN_TEST(event, event1);
  RUN_TEST(event, event2);
  RUN_TEST(event, event3);
  RUN_TEST(convert, bool);
  RUN_TEST(convert, int);
  RUN_TEST(convert, u128);
  RUN_TEST(convert, string);
  RUN_TEST(convert, vector);
}