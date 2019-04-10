cmake_minimum_required(VERSION 3.5)
set(CMAKE_C_FLAGS " ${CMAKE_C_FLAGS} -Oz -Wall --target=wasm32")
set(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -std=c++14 -Oz -Wall --target=wasm32")

message("lib dir ${CMAKE_CURRENT_SOURCE_DIR}")
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/platonlib/include/ DESTINATION ${BINARY_DIR}/include/)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/boost/include/ DESTINATION ${BINARY_DIR}/include/)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/rapidjson/include/ DESTINATION ${BINARY_DIR}/include/)

add_subdirectory(libc)
add_subdirectory(libc++)
