#include "platon/dispatcher.hpp"
#include <string>
#include <iostream>
#include <vector>

using namespace platon;
std::vector<byte> result;

#ifdef __cplusplus
extern "C" {
#endif

void platon_return(const uint8_t *value, const size_t len){
    byte *ptr = (byte *)value;
    std::vector<byte> my_return;
    for(size_t i = 0; i < len; i++) {
        my_return.push_back(*ptr);
        ptr++;
    }
    std::string str_return;
    fetch(RLP(my_return), str_return);
    std::cout << str_return << std::endl;
}

size_t platon_get_input_length(void){
    return result.size();
}

void platon_get_input(uint8_t *value){
    for (auto one :result) {
        *value = one;
        value++;
    }
}

void platon_debug(const uint8_t *dst, size_t len){
    for (size_t i = 0; i < len; i++){   
        std::cout << *(char*)(dst + i);
    }
    std::cout << std::endl;
}

void platon_panic(){
    std::cout << "platon panic"<< std::endl;
}

#ifdef __cplusplus
}
#endif


class hello {
    public:
        hello(){m_info["jatel"] = "jatel_info";}
        void init(){}

        void set_message(const std::string &name, const std::string &one_message){
            m_info[name] = one_message;
        }

        void change_message(const std::string &name, const std::string  &one_message){
            m_info[name] = one_message;
        }

        void delete_message(const std::string &name){
            m_info.erase(name);
        }

        std::string get_message(const std::string &name){
            return m_info[name];
        }
    private:
        std::map<std::string, std::string> m_info; 
};

PLATON_DISPATCH(hello, (init)(set_message)(change_message)(delete_message)(get_message))

int main(int argc, char **argv) {
    RLPStream wht_stream;
    wht_stream.appendList(2) << "get_message" << "jatel";
    result = wht_stream.out();
    invoke();
}