#include <platon/platon.hpp>
#include <string>
using namespace platon;

class message {
    public:
        std::string head;
        PLATON_SERIALIZE(message, (head))
};

class my_message : public message {
    public:
        std::string body;
        std::string end;
        PLATON_SERIALIZE_DERIVED(my_message, message, (body)(end))
};

CONTRACT hello : public platon::Contract{
   public:
      ACTION std::string init(const std::string address = ""){
          if (address.empty()) {
              Address platon_address = platon_caller();
              contract_ower.self() = platon_address;
          } else {
              auto result = make_address(address);
              if(result.second){
                  contract_ower.self() = result.first;
              }  
          }
          
          DEBUG("init ower address:", contract_ower.self().toString())
          return contract_ower.self().toString();
      }

      ACTION std::vector<my_message> add_message(const my_message &one_message){
          info.self().push_back(one_message);
          return info.self();
      }

      CONST std::vector<my_message> get_message(const std::string &name){
          return info.self();
      }

      ACTION std::string get_ower() {
          Address platon_address = contract_ower.self();
          return platon_address.toString();
      }

      ACTION std::string destroy() {
          Address platon_address = platon_origin();
          DEBUG("destroy ower address:", contract_ower.self().toString(), ", caller address:", platon_address.toString())
          if (contract_ower.self() != platon_address){
              return "invalid address";
          }
          platon_destroy(platon_address);
          return platon_address.toString();
      }

      ACTION std::string migrate(const bytes &init_arg, uint64_t transfer_value, uint64_t gas_value){
            Address platon_address = platon_origin();
            if (contract_ower.self() != platon_address){
                return "invalid address";
            }

            Address return_address;
            platon_migrate_contract(return_address, init_arg, transfer_value, gas_value);
            DEBUG("return_address", return_address.toString());
            return return_address.toString();
      }

   private:
      StorageType<"info"_n, std::vector<my_message>> info;
      StorageType<"owner"_n, Address> contract_ower;
};

PLATON_DISPATCH(hello, (init)(add_message)(get_message)(get_ower)(destroy)(migrate))
