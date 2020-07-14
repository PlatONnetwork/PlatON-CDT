#include <platon/platon.hpp>
#include <vector>
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

CONTRACT user : public platon::Contract {
    public:
        ACTION void init(){}
        ACTION bool call_add_message(const std::string &target_address, const my_message &one_message, 
            uint64_t transfer_value, uint64_t gas_value) {
            platon::bytes paras = platon::cross_call_args("add_message", one_message);
            auto result = make_address(target_address);
            if(!result.second) return false;
            return platon::platon_call(result.first, paras, transfer_value, gas_value);
        }

        ACTION std::vector<my_message> delegate_call_add_message(const std::string &target_address, const my_message &one_message,
            uint64_t gas_value) {
            platon::bytes paras = platon::cross_call_args("add_message", one_message);
            auto result = make_address(target_address);
            if(!result.second) return std::vector<my_message>();
            platon::platon_delegate_call(result.first, paras, gas_value);
            std::vector<my_message> return_value;
            get_call_output(return_value);
            return return_value;
        }

         ACTION std::vector<my_message> call_direct(const std::string &target_address, const my_message &one_message,
            uint64_t gas_value) {
            auto result = make_address(target_address);
            if(!result.second) return std::vector<my_message>();
            std::vector<my_message> return_value;
            auto call_result = platon_delegate_call_with_return_value<std::vector<my_message>>(result.first, gas_value, "add_message", one_message);
            return call_result.first;
         }

         ACTION bool call_direct_void(const std::string &target_address, const my_message &one_message,
            uint64_t gas_value) {
            auto result = make_address(target_address);
            if(!result.second) return false;
            bool call_result = platon_delegate_call(result.first, gas_value, "add_message", one_message);
            return call_result;
         }
};

PLATON_DISPATCH(user, (init)(call_add_message)(delegate_call_add_message)(call_direct)(call_direct_void))