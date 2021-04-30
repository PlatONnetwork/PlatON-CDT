#include <platon/platon.hpp>
#include <string>
using namespace platon;

class message {
   public:
      message(){}
      message(const std::string &p_head):head(p_head){}
   private:
      std::string head;
      PLATON_SERIALIZE(message, (head))
};

class my_message : public message {
   public:
      my_message(){}
      my_message(const std::string &p_head, const std::string &p_body, const std::string &p_end):message(p_head), body(p_body), end(p_end){}
   private:
      std::string body;
      std::string end;
      PLATON_SERIALIZE_DERIVED(my_message, message, (body)(end))
};

struct my_vector{
   // std::vector<platon::bytes> vec_bytes;
   // std::vector<my_message> vec_my_message;
   // std::vector<std::array<int, 5>> vec_array_int;
   // std::vector<std::array<std::map<int, int>, 10>> vect_arr_map;
   // std::vector<std::map<int, std::string>> vec_map;
   // std::vector<std::map<uint8_t, my_message>> vec_map_my_message;
   // std::vector<std::map<int, std::vector<platon::Address>>> vec_map_vec;
   // PLATON_SERIALIZE(my_vector, (vec_bytes)(vec_my_message)(vec_array_int)(vect_arr_map)(vec_map)(vec_map_my_message)(vec_map_vec))

   std::vector<int> vec;
   std::array<uint8_t, 2> arr;
   std::map<int, std::string> mp;
   PLATON_SERIALIZE(my_vector, (vec)(arr)(mp))
};

struct my_array{
   std::array<int32_t, 5> arr;
   std::array<platon::Address, 10> arr_address;
   std::array<my_message, 2> arr_my_message;
   std::array<std::vector<int>, 6> arr_vec_int;
   std::array<std::map<int, std::string>, 4> arr_map_str;
   std::array<std::array<std::map<int, std::string>, 4>, 2> arr_arr_map_str;
   PLATON_SERIALIZE(my_array, (arr)(arr_address)(arr_my_message)(arr_vec_int)(arr_map_str)(arr_arr_map_str))
};

struct my_map{
      std::map<uint32_t, std::string> mp;
      std::map<uint32_t, my_message> mp_my_message;
      std::map<uint32_t, std::vector<int>> mp_vec;
      std::map<int16_t, std::array<int, 3>> mp_arr;
      std::map<int16_t, std::array<std::vector<int>, 2>> mp_arr_vect;
      std::map<int16_t, std::map<int, int>>mp_mp;
      // std::tuple<std::string, int, int32_t> tup;
      PLATON_SERIALIZE(my_map, (mp)(mp_my_message)(mp_vec)(mp_arr)(mp_arr_vect)(mp_mp))
};

struct info {
    uint32_t one;
    platon::Address two;
    PLATON_SERIALIZE(info, (one)(two))
};

CONTRACT wasmCall : public platon::Contract{
   public:
      PLATON_EVENT1(hello_event, std::string, std::string, uint32_t)

      CONST void init(){}

    ACTION uint64_t set(uint64_t input){
        storedData.self() = input;
        return input;
    }
    
    CONST uint64_t get(){
        return storedData.self();
    }

    ACTION void set_int(int input){
        storedInt.self() = input;		
    }
    
    CONST int get_int(){
        return storedInt.self();
    }

    ACTION void set_string(const std::string &input){
        storedString.self() = input;		
    }
    
    CONST std::string get_string(){
        return storedString.self();
    }

    ACTION void set_info(const info &input){
        storedInfo.self() = input;		
    }
    
    CONST info get_info(){
        return storedInfo.self();
    }

      ACTION std::vector<my_message> add_message(const my_message &one_message){
          PLATON_EMIT_EVENT1(hello_event, "add_message", "event1", 1);
          info.self().push_back(one_message);
          return info.self();
      }
      CONST std::vector<my_message> get_message(const std::string &name){
          PLATON_EMIT_EVENT1(hello_event, "get_message", "event2", 2);
          return info.self();
      }

      ACTION void set_my_vector(const my_vector &one){
         vector_info.self() = one;
      }

      CONST my_vector get_my_vector(){
         return vector_info.self();
      }

      ACTION void set_my_array(const my_array &one){
         array_info.self() = one;
      }

      CONST my_array get_my_array(){
         return array_info.self();
      }

      ACTION void set_my_map(const my_map &one){
         map_info.self() = one;
      }

      CONST my_map get_my_map(){
         return map_info.self();
      }

      ACTION void hello_abort(){platon_assert(0, "hello abort");}

      ACTION void hello_panic(){platon_panic();}

   private:
      platon::StorageType<"int"_n, int> storedInt;
      platon::StorageType<"string"_n, std::string> storedString;
      platon::StorageType<"info"_n, info> storedInfo;
      platon::StorageType<"sstored"_n, uint64_t> storedData;
      platon::StorageType<"migrate_address"_n, Address> migrateAddress;
      platon::StorageType<"info"_n, std::vector<my_message>> info;
      platon::StorageType<"vector"_n, my_vector> vector_info;
      platon::StorageType<"array"_n, my_array> array_info;
      platon::StorageType<"map"_n, my_map> map_info;
};

PLATON_DISPATCH(wasmCall, (init)(set)(get)(set_int)(get_int)(set_string)(get_string)(set_info)(get_info)
    (add_message)(get_message)(set_my_vector)(get_my_vector)(get_my_array)(get_my_map)(hello_abort)(hello_panic))
