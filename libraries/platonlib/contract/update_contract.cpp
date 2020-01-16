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

extern char const contract_info[] = "info";
extern const char contract_owner[] = "owner";
extern const char contract_target[] = "target";

CONTRACT user : public platon::Contract {
    public:
        ACTION uint32_t init(const std::string &target_address, const std::string &owner_address = ""){
            if (owner_address.empty()) {
                Address platon_address;
                platon_caller(platon_address); 
                contract_owner.self() = platon_address;
            } else {
                contract_owner.self() = Address(owner_address);
            }
            contract_target.self() = Address(target_address);
        }

        ACTION std::string update(const std::string &target_address, uint64_t call_transfer_value, 
        uint64_t call_gas_value, const bytes &init_arg, uint64_t transfer_value, uint64_t gas_value){
            Address platon_address;
            platon_origin_caller(platon_address);
            if (contract_owner.self() != platon_address){
                return "invalid address";
            }
            bytes paras = cross_call_args("migrate", init_arg, transfer_value, gas_value);
            platon_call(target_address, paras, call_transfer_value, call_gas_value);
            std::string return_value;
            get_call_output(return_value);
            contract_target.self() = Address(return_value);
            return return_value;
        }

   private:
      StorageType<contract_info, std::vector<my_message>> info;
      StorageType<contract_owner, Address> contract_owner;
      StorageType<contract_target, Address> contract_target;
}

PLATON_DISPATCH(user, (init)(update))