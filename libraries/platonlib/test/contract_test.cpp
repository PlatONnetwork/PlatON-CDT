#include <platon/platon.hpp>
#include <iostream>
#include <string>
#include <map>

using namespace platon;

class message {
   public:
      message(){}
      message(const std::string &p_head):head(p_head){}
      friend std::ostream &operator<<( std::ostream &output, const message &one_message){
        return output << one_message.head;
      }
   private:
      std::string head;
      PLATON_SERIALIZE(message, (head))
};

class my_message : public message {
   public:
      my_message(){}
      my_message(const std::string &p_head, const std::string &p_body, const std::string &p_end):message(p_head), body(p_body), end(p_end){}
      friend std::ostream &operator<<( std::ostream &output, const my_message &group){
        return output << static_cast<const message&>(group) << ' ' << group.body << ' ' << group.end;
      }
   private:
      std::string body;
      std::string end;
      PLATON_SERIALIZE_DERIVED(my_message, message, (body)(end))
};

extern char const contract_info[] = "info";

CONTRACT hello : public platon::Contract{
   public:
      ACTION void init(){}
      ACTION std::vector<my_message> add_message(const my_message &one_message){
          info.self().push_back(one_message);
          return info.self();
      }
      CONST std::vector<my_message> get_message(const std::string &name){
          std::cout << name << std::endl;
          return info.self();
      }
   private:
      platon::StorageType<contract_info, std::vector<my_message>> info;
};

PLATON_DISPATCH(hello, (init)(add_message)(get_message))

std::vector<byte> input_result;
std::map<std::vector<byte>, std::vector<byte> > map_result;

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
    std::vector<my_message> info_return;
    fetch(RLP(my_return), info_return);
    for (auto const& i: info_return){
        std::cout << i << ' ';
    }
    std::cout << std::endl;
}

size_t platon_get_input_length(void){
    return input_result.size();
}

void platon_get_input(const uint8_t *value){
    byte *ptr = (byte *)value;
    for (auto one :input_result) {
        *ptr = one;
        ptr++;
    }
}

void platon_debug(uint8_t *dst, size_t len){
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
    map_result[vect_key] = vect_value;
}

size_t platon_get_state_length(const uint8_t* key, size_t klen){
    std::vector<byte> vect_key;
    vect_key = getVector(key, klen);
    return map_result[vect_key].size();
}

void platon_get_state(const uint8_t* key, size_t klen, uint8_t *value, size_t vlen){
    std::vector<byte> vect_key, vect_value;
    vect_key = getVector(key, klen);
    vect_value = map_result[vect_key];
    for(size_t i = 0; i < vlen && i < vect_value.size(); i++){
        *(value + i) = vect_value[i];
    }
}

#ifdef __cplusplus
}
#endif


int main(int argc, char **argv) {
    RLPStream wht_stream;
    my_message one("1_head", "1_body", "1_end");
    wht_stream.appendList(2) << "add_message" << one;
    input_result = wht_stream.out();
    invoke();

    wht_stream.clear();
    my_message two("2_head", "2_body", "2_end");
    wht_stream.appendList(2) << "add_message" << two;
    input_result = wht_stream.out();
    invoke();

    wht_stream.clear();
    wht_stream.appendList(2) << "get_message" << "jatel";
    input_result = wht_stream.out();
    invoke();

    platon_throw("test panic");
}