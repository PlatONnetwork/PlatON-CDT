#include "platon/event.hpp"
#include "platon/common.h"
#include <iostream>

using namespace platon;

#ifdef __cplusplus
extern "C" {
#endif

void platon_event(const uint8_t* args, size_t argsLen){
    byte *ptr = (byte *)args;
    std::vector<byte> my_args;
    for(size_t i = 0; i < argsLen; i++) {
        my_args.push_back(*ptr);
        ptr++;
    }
    
    std::tuple<std::string, uint32_t> result;
    fetch(RLP(my_args), result);
    std::cout << std::get<0>(result) << ' ' << std::get<1>(result) << std::endl;
}

void platon_event1(const uint8_t* topic, size_t topicLen, const uint8_t* args, size_t argsLen){
    for (size_t i = 0; i < topicLen; i++){   
        std::cout << *(char*)(topic + i);
    }
    std::cout << std::endl;
    platon_event(args, argsLen);
}

void platon_event2(const uint8_t* topic1, size_t topic1Len, const uint8_t* topic2, size_t topic2Len, const uint8_t* args, size_t argsLen){
    for (size_t i = 0; i < topic1Len; i++){   
        std::cout << *(char*)(topic1 + i);
    }
    std::cout << std::endl;

    platon_event1(topic2, topic2Len, args, argsLen);
}

void platon_event3(const uint8_t* topic1, size_t topic1Len, const uint8_t* topic2, size_t topic2Len, 
const uint8_t* topic3, size_t topic3Len, uint8_t* args, size_t argsLen){
    for (size_t i = 0; i < topic1Len; i++){   
        std::cout << *(char*)(topic1 + i);
    }
    std::cout << std::endl;

    platon_event2(topic2, topic2Len, topic3, topic3Len, args, argsLen);
}

void platon_event4(const uint8_t* topic1, size_t topic1Len, const uint8_t* topic2, size_t topic2Len, 
const uint8_t* topic3, size_t topic3Len, const uint8_t* topic4, size_t topic4Len, uint8_t* args, size_t argsLen){
    for (size_t i = 0; i < topic1Len; i++){   
        std::cout << *(char*)(topic1 + i);
    }
    std::cout << std::endl;

    platon_event3(topic2, topic2Len, topic3, topic3Len, topic4, topic4Len, args, argsLen);
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

#ifdef __cplusplus
}
#endif

PLATON_EVENT(jatel_test, std::string, uint32_t)

PLATON_EVENT1(jatel_test1, std::string, std::string, uint32_t)

PLATON_EVENT2(jatel_test2, std::string, std::string, std::string, uint32_t)

PLATON_EVENT3(jatel_test3, std::string, std::string, std::string, std::string, uint32_t)

PLATON_EVENT4(jatel_test4, std::string, std::string, std::string, std::string, std::string, uint32_t)

int main(int argc, char **argv) {
    PLATON_EMIT_EVENT(jatel_test, "event", 0);
    PLATON_EMIT_EVENT1(jatel_test1, "index_1", "event1", 1);
    PLATON_EMIT_EVENT1(jatel_test1, "index_2", "event1", 1);
    PLATON_EMIT_EVENT2(jatel_test2, "index_1", "index_1", "event2", 2);
    PLATON_EMIT_EVENT2(jatel_test2, "index_2", "index_2", "event2", 2);
    PLATON_EMIT_EVENT3(jatel_test3, "index_3", "index_3", "index_3", "event3", 3);
    PLATON_EMIT_EVENT4(jatel_test4, "index_4", "index_4", "index_4", "index_4", "event4", 4);
}