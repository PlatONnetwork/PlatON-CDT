#pragma once

#include<platon/RLP.h>

namespace platon{

/**
 *  Deserialize a tuple
 *
 *  @brief Deserialize a tuple
 *  @param ds - The stream to read
 *  @param t - The destination for deserialized value
 *  @tparam DataStream - Type of datastream
 *  @tparam Args - Type of the objects contained in the tuple
 *  @return DataStream& - Reference to the datastream
 */
template<typename... Args>
void fetch(RLP rlp, std::tuple<Args...>& t ) {
    int vect_index = 0;
    boost::fusion::for_each( t, [&]( auto& i ) {
        fetch(rlp[vect_index], i);
        vect_index++;
    });
}

}