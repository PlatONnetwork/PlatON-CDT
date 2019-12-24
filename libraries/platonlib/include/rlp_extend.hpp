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
void fetch(RLP& rlp, std::tuple<Args...>& t ) {
    std::vector<bytes> vect_result = rlp.toVector<bytes>();
    int vect_index = 0;
    boost::fusion::for_each( t, [&]( auto& i ) {
        rlp = RLP(vect_result[vect_index]);
        fetch(rlp, i);
        vect_index++;
    });
}

}