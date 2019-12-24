#pragma once

#include <boost/preprocessor/variadic/size.hpp>
#include <boost/preprocessor/variadic/to_tuple.hpp>
#include <boost/preprocessor/tuple/enum.hpp>
#include <boost/preprocessor/facilities/overload.hpp>

#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/fusion/include/std_tuple.hpp>

#include <boost/mp11/tuple.hpp>

#include <platon/service.hpp>
#include <type_traits>
#include <tuple>
#include<platon/RLP.h>

namespace platon {

	template<typename T>
	void platon_return(const T &t);
     /**
    * Unpack the received action and execute the correponding action handler
    *
    * @tparam T - The contract class that has the correponding action handler, this contract should be derived from ontio::contract
    * @tparam Q - The namespace of the action handler function
    * @tparam Args - The arguments that the action handler accepts, i.e. members of the action
    * @param obj - The contract object that has the correponding action handler
    * @param func - The action handler
    * @return true
    */
   template<typename T, typename R, typename... Args>
   void execute_action( RLP& rlp,  R (T::*func)(Args...)  ) {
      std::tuple<std::decay_t<Args>...> args;
      fetch(rlp, args);

      T inst;

      auto f2 = [&]( auto... a ) {
         R&& t = ((&inst)->*func)( a... );
		 platon_return<R>(t);
      };

      boost::mp11::tuple_apply( f2, args );
   }

   template<typename T, typename... Args>
   void execute_action(RLP& rlp, void (T::*func)(Args...)  ) {
      std::tuple<std::decay_t<Args>...> args;
      fetch(rlp, args);

      T inst;

      auto f2 = [&]( auto... a ) {
         ((&inst)->*func)( a... );
      };

      boost::mp11::tuple_apply( f2, args );
   }

    // Helper macro for PLATON_DISPATCH_INTERNAL
     #define PLATON_DISPATCH_INTERNAL( r, OP, elem ) \
	    case BOOST_PP_STRINGIZE(elem) : \
            PLATON::execute_action( rlp, &OP::elem ); \
            break;

    // Helper macro for PLATON_DISPATCH
    #define PLATON_DISPATCH_HELPER( TYPE,  MEMBERS ) \
        BOOST_PP_SEQ_FOR_EACH( PLATON_DISPATCH_INTERNAL, TYPE, MEMBERS )

    /**
     * @addtogroup dispatcher
     * Convenient macro to create contract apply handler
     *
     * @note To be able to use this macro, the contract needs to be derived from platon::contract
     * @param TYPE - The class name of the contract
     * @param MEMBERS - The sequence of available actions supported by this contract
     *
     * Example:
     * @code
     * PLATON_DISPATCH( hello, (init)(set_message)(change_message)(delete_message)(get_message) )
     * @endcode
     */
    #define PLATON_DISPATCH( TYPE, MEMBERS ) \
    extern "C" { \
    void invoke(void) {  \
        std::string method; \
        auto input = get_input(); \
        RLP rlp(input); \
        std::vector<bytes> vect_parames = rlp.toVector<bytes>();\
        rlp = RLP(vect_parames[0]);\
        fetch(rlp, method)\
        vect_parames.erase(vect_parames.begin());\
        RLPStream rlp_stream;\
        rlp_stream << vect_parames;\
        rlp = RLP(rlp_stream.out());\
        switch( method ) { \
            PLATON_DISPATCH_HELPER( TYPE, MEMBERS ) \
            default: \
                platon_assert(false, "no metod to call\n");\
                break; \
        } \
    } \
    }

}
