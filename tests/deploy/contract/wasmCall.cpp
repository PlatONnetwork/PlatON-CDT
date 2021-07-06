#include <platon/platon.hpp>
#include <string>
#include <tuple>
using namespace platon;

class message {
 public:
  message() {}
  message(const std::string &p_head) : head(p_head) {}

 private:
  std::string head;
  PLATON_SERIALIZE(message, (head))
};

class my_message : public message {
 public:
  my_message() {}
  my_message(const std::string &p_head, const std::string &p_body,
             const std::string &p_end)
      : message(p_head), body(p_body), end(p_end) {}

 private:
  std::string body;
  std::string end;
  PLATON_SERIALIZE_DERIVED(my_message, message, (body)(end))
};

struct my_vector {
  std::vector<platon::bytes> vec_bytes;
  std::vector<std::tuple<int, int, std::string>> vec_tup;
  std::vector<my_message> vec_my_message;
  std::vector<std::array<int, 5>> vec_array_int;
  std::vector<std::array<std::map<int, int>, 2>> vect_arr_map;
  std::vector<std::map<int, std::string>> vec_map;
  std::vector<std::map<uint8_t, my_message>> vec_map_my_message;
  std::vector<std::map<int, std::vector<std::string>>> vec_map_vec;
  PLATON_SERIALIZE(my_vector,
                   (vec_bytes)(vec_tup)(vec_my_message)(vec_array_int)(
                       vect_arr_map)(vec_map)(vec_map_my_message)(vec_map_vec))
};

struct my_array {
  std::array<int32_t, 5> arr;
  std::array<platon::Address, 10> arr_address;
  std::array<my_message, 2> arr_my_message;
  std::array<std::vector<int>, 6> arr_vec_int;
  std::array<std::map<int, std::string>, 4> arr_map_str;
  std::array<std::array<std::map<int, std::string>, 4>, 2> arr_arr_map_str;
  std::array<std::tuple<int, std::tuple<int, std::string>>, 5> arr_tup;
  PLATON_SERIALIZE(my_array, (arr)(arr_address)(arr_my_message)(arr_vec_int)(
                                 arr_map_str)(arr_arr_map_str)(arr_tup))
};

struct my_map {
  std::map<uint32_t, std::string> mp;
  std::map<uint32_t, my_message> mp_my_message;
  std::map<uint32_t, std::vector<int>> mp_vec;
  std::map<int16_t, std::array<int, 3>> mp_arr;
  std::map<int16_t, std::array<std::vector<int>, 2>> mp_arr_vect;
  std::map<int16_t, std::map<int, int>> mp_mp;
  std::map<int16_t, std::tuple<int, std::map<int, std::tuple<int, int>>> > mp_tup;
  PLATON_SERIALIZE(my_map,
                   (mp)(mp_my_message)(mp_vec)(mp_arr)(mp_arr_vect)(mp_mp)(mp_tup))
};

struct info {
  uint32_t one;
  std::string two;
  PLATON_SERIALIZE(info, (one)(two))
};

CONTRACT wasmCall : public platon::Contract {
 public:
  CONST void init() {}

  ACTION void set_uint(uint64_t input) { pod_uint.self() = input; }

  CONST uint64_t get_uint() { return pod_uint.self(); }

  ACTION void set_int(int input) { pod_int.self() = input; }

  CONST int get_int() { return pod_int.self(); }

  ACTION void set_string(const std::string &input) { pod_string.self() = input; }

  CONST std::string get_string() { return pod_string.self(); }

  ACTION void set_address(const platon::Address &input) { pod_address.self() = input; }

  CONST platon::Address get_address() { return pod_address.self(); }

  ACTION void set_info(const info &input) { struct_info.self() = input; }

  CONST info get_info() { return struct_info.self(); }

  ACTION void add_message(const my_message &one_message) { struct_derived.self().push_back(one_message);}

  CONST std::vector<my_message> get_message() {return struct_derived.self();}

  ACTION void set_my_vector(const my_vector &one) { vector_info.self() = one; }

  CONST my_vector get_my_vector() { return vector_info.self(); }

  ACTION void set_my_array(const my_array &one) { array_info.self() = one; }

  CONST my_array get_my_array() { return array_info.self(); }

  ACTION void set_my_map(const my_map &one) { map_info.self() = one; }

  CONST my_map get_my_map() { return map_info.self(); }

 private:
  platon::StorageType<"uint"_n, uint64_t> pod_uint;
  platon::StorageType<"int"_n, int> pod_int;
  platon::StorageType<"bigint"_n, std::uint256_t> pod_uint256;
  platon::StorageType<"string"_n, std::string> pod_string;
  platon::StorageType<"address"_n, Address> pod_address;
  platon::StorageType<"info"_n, info> struct_info;
  platon::StorageType<"info"_n, std::vector<my_message>> struct_derived;
  platon::StorageType<"vector"_n, my_vector> vector_info;
  platon::StorageType<"array"_n, my_array> array_info;
  platon::StorageType<"map"_n, my_map> map_info;
};

PLATON_DISPATCH(wasmCall,
                (init)(set_uint)(get_uint)(set_int)(get_int)(set_string)(get_string)(set_address)(get_address)(set_info)(get_info)
                (add_message)(get_message)(set_my_vector)(get_my_vector)(set_my_array)(get_my_array)(set_my_map)(get_my_map))
