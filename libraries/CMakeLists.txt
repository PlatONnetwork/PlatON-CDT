cmake_minimum_required(VERSION 3.5)
set(CMAKE_C_FLAGS " ${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -std=c++14 -Wall")

message(STATUS "libraries dir ${CMAKE_CURRENT_SOURCE_DIR}")

add_subdirectory(libc)
add_subdirectory(libc++)
add_subdirectory(platonlib/src)

INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/platonlib/include/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include/)
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/boost/include/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include/)