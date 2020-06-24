#undef NDEBUG
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

struct OtherType {
  std::string name;
  std::string description;
  PLATON_SERIALIZE(OtherType, (name)(description))
};

TEST_CASE(convert, bytes){
  bytes corret_result = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};

  // std::array<int8_t, N>
  std::array<int8_t, 32> array_int = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
  auto array_int_result = event_data_convert(array_int);
  ASSERT_EQ(corret_result, array_int_result);
  std::array<int8_t, 37> array_int_append = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x12, 0x13, 0x14};
  array_int_result = event_data_convert(array_int_append);
  ASSERT_EQ(corret_result, array_int_result);

  
  // std::array<uint8_t, N>
  std::array<uint8_t, 32> array_uint = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
  auto array_uint_result = event_data_convert(array_uint);
  ASSERT_EQ(corret_result, array_uint_result);
  std::array<uint8_t, 37> array_uint_append = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x12, 0x13, 0x14};
  array_uint_result = event_data_convert(array_uint_append);
  ASSERT_EQ(corret_result, array_uint_result);

  // std::vector<int8_t>
  std::vector<int8_t> vector_int = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
  auto vector_int_result = event_data_convert(vector_int);
  ASSERT_EQ(corret_result, vector_int_result);
  vector_int = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x12, 0x13, 0x14};
  vector_int_result = event_data_convert(vector_int);
  ASSERT_EQ(corret_result, vector_int_result);

  
  // std::vector<uint8_t>
  std::vector<uint8_t> vector_uint = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
  auto vector_uint_result = event_data_convert(vector_uint);
  ASSERT_EQ(corret_result, vector_uint_result);
  vector_uint = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x12, 0x13, 0x14};
  vector_uint_result = event_data_convert(vector_uint);
  ASSERT_EQ(corret_result, vector_uint_result);

  // std::list<int8_t>
  std::list<int8_t> list_int = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
  auto list_int_result = event_data_convert(list_int);
  ASSERT_EQ(corret_result, list_int_result);
  list_int = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x12, 0x13, 0x14};
  list_int_result = event_data_convert(list_int);
  ASSERT_EQ(corret_result, list_int_result);

  
  // std::list<uint8_t>
  std::list<uint8_t> list_uint = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
  auto list_uint_result = event_data_convert(list_uint);
  ASSERT_EQ(corret_result, list_uint_result);
  list_uint = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x12, 0x13, 0x14};
  list_uint_result = event_data_convert(list_uint);
  ASSERT_EQ(corret_result, list_uint_result);

  // FixedHash<N>
  FixedHash<32> fixed_uint = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
  auto fixed_uint_result = event_data_convert(fixed_uint);
  ASSERT_EQ(corret_result, fixed_uint_result);
  FixedHash<37> fixed_uint_append = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x12, 0x13, 0x14};
  fixed_uint_result = event_data_convert(fixed_uint_append);
  ASSERT_EQ(corret_result, fixed_uint_result);

  // std::string
  std::string str = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
  auto str_result = event_data_convert(str);
  ASSERT_EQ(corret_result, str_result);
  str = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x12, 0x13, 0x14};
  str_result = event_data_convert(str);
  ASSERT_EQ(corret_result, str_result);

  // other type
  OtherType other_type = {.name = "jatel", .description = "simple"};
  auto other_type_result = event_data_convert(other_type);
  RLPStream other_type_stream;
  other_type_stream << other_type;
  bytesRef other_type_result_ref = other_type_stream.out();
  bytes other_type_result_bytes(other_type_result_ref.data(), other_type_result_ref.data()+other_type_result_ref.size());
  ASSERT_EQ(other_type_result, other_type_result_bytes);
  other_type = {.name = "jateljateljateljateljateljateljateljatel", .description = "simplesimplesimplesimplesimplesimplesimplesimple"};
  other_type_result = event_data_convert(other_type);
  other_type_stream.clear();
  other_type_stream << other_type;
  other_type_result_ref = other_type_stream.out();
  bytes other_type_result_bytes_temp = bytes(other_type_result_ref.data(), other_type_result_ref.data()+other_type_result_ref.size());
  other_type_result_bytes.resize(32);
  platon_sha3(other_type_result_bytes_temp.data(), other_type_result_bytes_temp.size(), other_type_result_bytes.data(), other_type_result_bytes.size());
  ASSERT_EQ(other_type_result, other_type_result_bytes);
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
  RUN_TEST(convert, bytes);
}