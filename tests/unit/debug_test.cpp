#undef NDEBUG
#define TESTNET
#include "platon/platon.hpp"
#include "unit_test.hpp"

template <typename T>
void print_rlp_code(const std::string& prefix, const T& data,
                    const bytesRef& result) {
  std::string info;
  char temp_info[100] = {};
  for (auto i : result) {
    sprintf(temp_info, "%d, ", int(i));
    info += temp_info;
  }
  platon::println(prefix, data, info);
}

TEST_CASE(debug, time){
    int64_t info = platon_nano_time();
    printf("time:%lld\t\n", info);
}

class WhtType {
 public:
  WhtType() {}
  WhtType(std::string name, uint16_t age, uint16_t weight)
      : m_name_(name), m_age_(age), m_weight_(weight) {}
  friend RLPStream& operator<<(RLPStream& rlp, const WhtType& one) {
    return rlp.appendList(3) << one.m_name_ << one.m_age_ << one.m_weight_;
  }

  friend RLPSize& operator<<(RLPSize& rlps, const WhtType& one) {
    return rlps << RLPSize::list_start() << one.m_name_ << one.m_age_
                << one.m_weight_ << RLPSize::list_end();
  }

  friend void fetch(RLP rlp, WhtType& one) {
    fetch(rlp[0], one.m_name_);
    fetch(rlp[1], one.m_age_);
    fetch(rlp[2], one.m_weight_);
  }
  friend bool operator==(const WhtType& lhs, const WhtType& rhs) {
    return lhs.m_age_ == rhs.m_age_ && lhs.m_name_ == rhs.m_name_ &&
           lhs.m_weight_ == rhs.m_weight_;
  }

 public:
  std::string m_name_;
  uint16_t m_age_;
  uint16_t m_weight_;
};

class WhtGroup {
 public:
  WhtGroup() {}
  WhtGroup(std::string info, uint16_t number, WhtType member)
      : m_info_(info), m_number_(number), m_member_(member) {}
  friend RLPStream& operator<<(RLPStream& rlp, const WhtGroup& group) {
    return rlp.appendList(3)
           << group.m_info_ << group.m_number_ << group.m_member_;
  }

  friend RLPSize& operator<<(RLPSize& rlps, const WhtGroup& group) {
    return rlps << RLPSize::list_start() << group.m_info_ << group.m_number_
                << group.m_member_ << RLPSize::list_end();
  }

  friend void fetch(RLP rlp, WhtGroup& group) {
    fetch(rlp[0], group.m_info_);
    fetch(rlp[1], group.m_number_);
    fetch(rlp[2], group.m_member_);
  }
  friend bool operator==(const WhtGroup& lhs, const WhtGroup& rhs) {
    return lhs.m_info_ == rhs.m_info_ && lhs.m_number_ == rhs.m_number_ &&
           lhs.m_member_ == rhs.m_member_;
  }

 public:
  std::string m_info_;
  uint16_t m_number_;
  WhtType m_member_;
};

class Parent {
 public:
  Parent() {}
  Parent(std::string info, uint16_t number, WhtType member)
      : m_info_(info), m_number_(number), m_member_(member) {}
  PLATON_SERIALIZE(Parent, (m_info_)(m_number_)(m_member_))

  friend bool operator==(const Parent& lhs, const Parent& rhs) {
    return lhs.m_info_ == rhs.m_info_ && lhs.m_number_ == rhs.m_number_ &&
           lhs.m_member_ == rhs.m_member_;
  }

 public:
  std::string m_info_;
  uint16_t m_number_;
  WhtType m_member_;
};

class Derived : public Parent {
 public:
  Derived() {}
  Derived(std::string info, uint16_t number, WhtType member, std::string other)
      : Parent(info, number, member), m_other_(other) {}
  PLATON_SERIALIZE_DERIVED(Derived, Parent, (m_other_))
  friend bool operator==(const Derived& lhs, const Derived& rhs) {
    return lhs.m_other_ == rhs.m_other_ &&
           dynamic_cast<const Parent&>(lhs) == dynamic_cast<const Parent&>(rhs);
  }

 public:
  std::string m_other_;
};

TEST_CASE(debug, rlp) {
  int64_t begin_time = platon_nano_time();
  const char* fn = "Derived";
  WhtType one("jatel", 30, 160);
  Derived test_derived("group", 3, one, "jatel_derived");
  for(int i = 0; i <1000; i++){
    auto size = pack_size(test_derived);
    RLPStream wht_stream;
    wht_stream.reserve(size);
    wht_stream << test_derived;
    bytesRef result = wht_stream.out();
    print_rlp_code("Derived", "test", result);
  }
  int64_t end_time = platon_nano_time();
  printf("spent time:%lld\t\n", end_time-begin_time);
}

constexpr bigint one_int = 0xff;
constexpr bigint four_int = 0xffffffff;
constexpr bigint eight_int = 0xffffffffffffffffllu;



TEST_CASE(debug, bigint) {
    bigint test_case = one_int;
    int64_t begin_time = platon_nano_time();
    for(int i = 0; i <1000000; i++){
        auto size = pack_size(test_case);
        RLPStream wht_stream;
        wht_stream.reserve(size);
        wht_stream << test_case;
        bytesRef result = wht_stream.out();
        //print_rlp_code("bigint", "test", result);
    }
    int64_t end_time = platon_nano_time();
    printf("spent time:%lld\t\n", (end_time-begin_time)/1000000000);
}


UNITTEST_MAIN() {
  // RUN_TEST(debug, time);

  // RUN_TEST(debug, rlp);

  RUN_TEST(debug, bigint);
}