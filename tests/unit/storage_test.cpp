#include "platon/contract.hpp"
#include "platon/dispatcher.hpp"
#include "platon/rlp_serialize.hpp"
#include "platon/storagetype.hpp"
#include "unit_test.hpp"


using namespace platon;
std::map<std::vector<byte>, std::vector<byte>> result;

std::vector<byte> get_vector(const uint8_t *address, size_t len) {
  byte *ptr = (byte *)address;
  std::vector<byte> vect_result;
  for (size_t i = 0; i < len; i++) {
    vect_result.push_back(*ptr);
    ptr++;
  }
  return vect_result;
}

#ifdef __cplusplus
extern "C" {
#endif

void platon_set_state(const uint8_t *key, size_t klen, const uint8_t *value,
                      size_t vlen) {
  std::vector<byte> vect_key, vect_value;
  vect_key = get_vector(key, klen);
  vect_value = get_vector(value, vlen);
  result[vect_key] = vect_value;
}

size_t platon_get_state_length(const uint8_t *key, size_t klen) {
  std::vector<byte> vect_key;
  vect_key = get_vector(key, klen);
  return result[vect_key].size();
}

int32_t platon_get_state(const uint8_t *key, size_t klen, uint8_t *value,
                         size_t vlen) {
  std::vector<byte> vect_key, vect_value;
  vect_key = get_vector(key, klen);
  vect_value = result[vect_key];
  for (size_t i = 0; i < vlen && i < vect_value.size(); i++) {
    *(value + i) = vect_value[i];
  }
  return vlen;
}

#ifdef __cplusplus
}
#endif

class TestContract {
public:
  TestContract() {}

  void AddMessage(const std::string &one_message) {
    info.self().push_back(one_message);
  }

  std::vector<std::string> GetMessage() { return info.self(); }

private:
  StorageType<"info"_n, std::vector<std::string>> info;
};

TEST_CASE(storage, add) {

  {
    TestContract test1;
    test1.AddMessage("test1");
  }

  {
    TestContract test1;
    auto result_message = test1.GetMessage();
    std::vector<std::string> right_message = {"test1"};
    ASSERT_EQ(result_message, right_message);
  }

  {
    TestContract test2;
    test2.AddMessage("test2");
  }

  {
    TestContract test2;
    auto result_message = test2.GetMessage();
    std::vector<std::string> right_message = {"test1", "test2"};
    ASSERT_EQ(result_message, right_message);
  }

  {
    TestContract test3;
    test3.AddMessage("test3");
  }

  {
    TestContract test3;
    auto result_message = test3.GetMessage();
    std::vector<std::string> right_message = {"test1", "test2", "test3"};
    ASSERT_EQ(result_message, right_message);
  }
}

UNITTEST_MAIN() { RUN_TEST(storage, add); }