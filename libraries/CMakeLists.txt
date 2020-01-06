cmake_minimum_required(VERSION 3.4.3)
set(CMAKE_C_FLAGS " ${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -std=c++14 -Wall")

message(STATUS "libraries dir ${CMAKE_CURRENT_SOURCE_DIR}")

set(PLATON_CDT ${CMAKE_BINARY_DIR}/platon.cdt)
set(LIB_OUTPUT ${PLATON_CDT}/lib)
set(INC_OUTPUT ${PLATON_CDT}/include)

add_subdirectory(libc)
add_subdirectory(libc++)
add_subdirectory(platonlib/src)


file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/platonlib/include/ DESTINATION ${INC_OUTPUT})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/boost/include/ DESTINATION ${INC_OUTPUT})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/extern_symbol DESTINATION ${LIB_OUTPUT})


install(DIRECTORY ${PLATON_CDT} DESTINATION ${CMAKE_INSTALL_PREFIX})
