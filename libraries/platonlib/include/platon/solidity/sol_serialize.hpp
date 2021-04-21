#pragma once

#include <boost/preprocessor/seq/enum.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/seq.hpp>
#include <boost/preprocessor/seq/size.hpp>
#include <boost/preprocessor/stringize.hpp>
#include "platon/solidity/common.hpp"

#define SOLIDITY_REFLECT_MEMBER_DYN(r, OP, elem) \
  if (platon::solidity::abi::OP(elem)) {         \
    return true;                                 \
  }

#define SOLIDITY_REFLECT_MEMBER_TYPE_SIZE(r, OP, elem) \
  size += platon::solidity::abi::GetTypeSize(elem);

#define SOLIDITY_REFLECT_MEMBER_EACH(r, OP, elem) OP(elem);
/**
 *  Defines serialization and deserialization for a class
 *
 *  @brief Defines serialization and deserialization for a class
 *
 *  @param TYPE - the class to have its serialization and deserialization
 * defined
 *  @param MEMBERS - a sequence of member names.  (field1)(field2)(field3)
 */
#define SOLIDITY_SERIALIZE(TYPE, MEMBERS)                                      \
  bool IsDynamicType() const {                                                 \
    BOOST_PP_SEQ_FOR_EACH(SOLIDITY_REFLECT_MEMBER_DYN, IsDynamicType, MEMBERS) \
    return false;                                                              \
  }                                                                            \
  size_t GetTypeSize() const {                                                 \
    size_t size = 0;                                                           \
    BOOST_PP_SEQ_FOR_EACH(SOLIDITY_REFLECT_MEMBER_TYPE_SIZE, GetTypeSize,      \
                          MEMBERS)                                             \
    return size;                                                               \
  }                                                                            \
  template <typename Func>                                                     \
  void ForEachElement(Func &&f) const {                                        \
    BOOST_PP_SEQ_FOR_EACH(SOLIDITY_REFLECT_MEMBER_EACH, f, MEMBERS)            \
  }
