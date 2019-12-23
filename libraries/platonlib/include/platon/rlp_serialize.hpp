#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/enum.hpp>
#include <boost/preprocessor/seq/size.hpp>
#include <boost/preprocessor/seq/seq.hpp>
#include <boost/preprocessor/stringize.hpp>
#include "platon/RLP.h"
#include "platon/common.h"
#include<vector>

#define PLATON_REFLECT_MEMBER_OP_INPUT( r, OP, elem ) \
  rlp = rlp OP t.elem;\
  vect_parames.push_back(rlp.out());\
  rlp.clear()

#define PLATON_REFLECT_MEMBER_OP_OUTPUT( r, OP, elem ) \
  rlp = RLP(vect_result[vect_index]);\
  rlp.OP(t.elem);
  vect_index++;\

/**
 *  Defines serialization and deserialization for a class
 *
 *  @brief Defines serialization and deserialization for a class
 *
 *  @param TYPE - the class to have its serialization and deserialization defined
 *  @param MEMBERS - a sequence of member names.  (field1)(field2)(field3)
 */
#define PLATON_SERIALIZE( TYPE,  MEMBERS ) \
friend RLPStream& operator << ( RLPStream& rlp, const TYPE& t ){ \
    rlp.clear()\
    std::vector<bytes> vect_parames;\
    BOOST_PP_SEQ_FOR_EACH( PLATON_REFLECT_MEMBER_OP_INPUT, <<, MEMBERS );\
    rlp.clear()\
    return rlp << vect_parames;\
}\
friend void fetch(RLP& rlp, TYPE& t){\
    std::vector<bytes> vect_result = rlp.toVector<bytes>();\
    int vect_index = 0;\
    BOOST_PP_SEQ_FOR_EACH( PLATON_REFLECT_MEMBER_OP_OUTPUT, fetch, MEMBERS );\
}
 

 /**
 *  Defines serialization and deserialization for a class which inherits from other classes that
 *  have their serialization and deserialization defined
 *
 *  @brief Defines serialization and deserialization for a class which inherits from other classes that
 *  have their serialization and deserialization defined
 *
 *  @param TYPE - the class to have its serialization and deserialization defined
 *  @param BASE - a sequence of base class names (basea)(baseb)(basec)
 *  @param MEMBERS - a sequence of member names.  (field1)(field2)(field3)
 */
#define PLATON_SERIALIZE_DERIVED( TYPE, BASE, MEMBERS ) \
friend RLPStream& operator << ( RLPStream& rlp, const TYPE& t ){ \
    rlp.clear()\
    std::vector<bytes> vect_parames;\
    rlp = rlp << static_cast<const BASE&>(t); \
    vect_parames.push_back(rlp.out());\
    rlp.clear()
    BOOST_PP_SEQ_FOR_EACH( PLATON_REFLECT_MEMBER_OP_INPUT, <<, MEMBERS );\
    rlp.clear()\
    return rlp << vect_parames;\
}\
friend void fetch(RLP& rlp, TYPE& t){\
    std::vector<bytes> vect_result = rlp.toVector<bytes>();\
    int vect_index = 0;\
    rlp = RLP(vect_result[vect_index]);\
    rlp.fetch(static_cast<BASE&>(t));\
    vect_index++;\
    BOOST_PP_SEQ_FOR_EACH( PLATON_REFLECT_MEMBER_OP_OUTPUT, fetch, MEMBERS );\
}