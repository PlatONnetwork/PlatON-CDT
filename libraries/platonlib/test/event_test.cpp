#include "platon/event.hpp"
#include <iostream>
#include <array>
#include <vector>

using namespace platon;

#ifdef __cplusplus
extern "C" {
#endif

void platon_event(const uint8_t* topic, size_t topicLen, const uint8_t* args, size_t argsLen){
    if (NULL != topic && 0 != topicLen) {
        byte *topic_ptr = (byte *)topic;
        std::vector<byte> my_topic;
        for(size_t i = 0; i < topicLen; i++) {
            my_topic.push_back(*topic_ptr);
            topic_ptr++;
        }
        RLP rlp(my_topic);
        size_t num = rlp.itemCountStrict();
        std::cout << num << std::endl;
        switch (num) {
            case 1:
                {
                    std::array<std::string, 1> topic_info;
                    fetch(rlp, topic_info);
                    std::cout << topic_info[0] << std::endl;
                    break;
                }

            case 2:
                {
                    std::array<std::string, 2> topic_info;
                    fetch(rlp, topic_info);
                    std::cout << topic_info[0] << ' ' << topic_info[1] << std::endl;
                    break;
                }

            case 3:
                {
                    std::array<std::string, 3> topic_info;
                    fetch(rlp, topic_info);
                    std::cout << topic_info[0] << ' ' << topic_info[1] << ' ' << topic_info[2] << std::endl;
                    break;
                }

            case 4:
                {
                    std::array<std::string, 4> topic_info;
                    fetch(rlp, topic_info);
                    std::cout << topic_info[0] << ' ' << topic_info[1] << ' ' << topic_info[2] << ' ' << topic_info[3] << std::endl;
                    break;
                }

            default:
                platon_panic(); 
        } 
    }

    byte *args_ptr = (byte *)args;
    std::vector<byte> my_args;
    for(size_t i = 0; i < argsLen; i++) {
        my_args.push_back(*args_ptr);
        args_ptr++;
    }
    std::tuple<std::string, uint32_t> result;
    fetch(RLP(my_args), result);
    std::cout << std::get<0>(result) << ' ' << std::get<1>(result) << std::endl;
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