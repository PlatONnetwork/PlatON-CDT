#include "platon/storagetype.hpp"
#include "platon/rlp_extend.hpp"
#include <iostream>
#include <string>
#include <vector>

using namespace platon;
std::map<std::vector<byte>, std::vector<byte> > result;

#ifdef __cplusplus
extern "C" {
#endif

void platon_debug(const uint8_t *dst, size_t len){
    for (size_t i = 0; i < len; i++){   
        std::cout << *(char*)(dst + i);
    }
    std::cout << std::endl;
}

void platon_panic(){
    std::cout << "platon panic"<< std::endl;
}

std::vector<byte> getVector(const uint8_t* address, size_t len){
    byte *ptr = (byte *)address;
    std::vector<byte> vect_result;
    for(size_t i = 0; i < len; i++) {
        vect_result.push_back(*ptr);
        ptr++;
    }
    return vect_result;
}

void platon_set_state(const uint8_t* key, size_t klen, const uint8_t *value, size_t vlen){
    std::vector<byte> vect_key, vect_value;
    vect_key = getVector(key, klen);
    vect_value = getVector(value, vlen);
    result[vect_key] = vect_value;
}

size_t platon_get_state_length(const uint8_t* key, size_t klen){
    std::vector<byte> vect_key;
    vect_key = getVector(key, klen);
    return result[vect_key].size();
}

size_t platon_get_state(const uint8_t* key, size_t klen, uint8_t *value, size_t vlen){
    std::vector<byte> vect_key, vect_value;
    vect_key = getVector(key, klen);
    vect_value = result[vect_key];
    for(size_t i = 0; i < vlen && i < vect_value.size(); i++){
        *(value + i) = vect_value[i];
    }
    return vlen;
}

#ifdef __cplusplus
}
#endif

class testContract 
{
    public:
        testContract(){}

        void add_message(const std::string &one_message){
            info.self().push_back(one_message);
        }

        std::vector<std::string> get_message(){
            return info.self();
        }

    private:
        StorageType<"info"_n, std::vector<std::string>> info;
};

int main(int argc, char **argv) {

    {
        testContract test1;
        test1.add_message("test1");
    }
    
    {
        testContract test2;
        test2.add_message("test2");
        auto result = test2.get_message();
        for (auto one : result){
            std::cout << one;
        }
        std::cout << std::endl;
    }

    {
        testContract test3;
        test3.add_message("test3");
        auto result = test3.get_message();
        for (auto one : result){
            std::cout << one;
        }
        std::cout << std::endl;
    }
}