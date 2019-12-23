#pragma once

#include<platon/RLP.h>

namespace platon{

/**
 *  Serialize a tuple
 *
 *  @brief Serialize a tuple
 *  @param ds - The stream to write
 *  @param t - The value to serialize
 *  @tparam RLPStream - Type of RLPStream
 *  @tparam Args - Type of the objects contained in the tuple
 *  @return RLPStream& - Reference to the RLPStream
 */
template<typename... Args>
RLPStream& operator<<( RLPStream& ds, const std::tuple<Args...>& t ) {
   boost::fusion::for_each( t, [&]( const auto& i ) {
       ds << i;
   });
   return ds;
}

}