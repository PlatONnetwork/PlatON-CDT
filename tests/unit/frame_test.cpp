#include "platon/contract.hpp"
#include "platon/rlp_serialize.hpp"
#include "platon/print.hpp"
#include "platon/dispatcher.hpp"
#include "platon/name.hpp"
#include "unit_test.hpp"
#include <string>
#include <vector>

using namespace platon;

std::vector<byte> get_input(const std::string &str_method){
    uint64_t t_method = Name(str_method).value;
    RLPStream stream(2);
    stream << t_method << uint64_t(5678);
    return stream.out();
}

#ifdef __cplusplus
extern "C" {
#endif

std::vector<byte> input_result = get_input("global");

size_t platon_get_input_length(void){
    return input_result.size();
}

void platon_get_input(uint8_t *value){
    for (auto one :input_result) {
        *value = one;
        value++;
    }
}

#ifdef __cplusplus
}
#endif

CONTRACT FrameTest : public platon::Contract{
    public:
        ACTION uint64_t init(const uint64_t & num){
            DEBUG("init", num);
            return num;
        }
        ACTION uint64_t add(const uint64_t & num){
            DEBUG("add", num);
            return num;
        }
        ACTION uint64_t get(const uint64_t & num){
            DEBUG("get", num);
            return num;
        }
        ACTION uint64_t test(const uint64_t & num){
            DEBUG("test", num);
            return num;
        }
        ACTION uint64_t info(const uint64_t & num){
            DEBUG("info", num);
            return num;
        }
        ACTION uint64_t message(const uint64_t & num){
            DEBUG("message", num);
            return num;
        }
        ACTION uint64_t storage(const uint64_t & num){
            DEBUG("storage", num);
            return num;
        }
        ACTION uint64_t global(const uint64_t & num){
            DEBUG("global", num);
            return num;
        }
};

PLATON_DISPATCH(FrameTest, (init)(add)(get)(test)(info)(message)(storage)(global))