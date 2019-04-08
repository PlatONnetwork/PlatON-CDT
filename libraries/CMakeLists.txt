set(CMAKE_C_FLAGS " ${CMAKE_C_FLAGS} -Oz -Wall --target=wasm32")
set(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -std=c++14 -Oz -Wall --target=wasm32")

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/boost/include/ DESTINATION ${CMAKE_BINARY_DIR}/include)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/rapidjson/include/ DESTINATION ${CMAKE_BINARY_DIR}/include)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/platonlib/include/ DESTINATION ${CMAKE_BINARY_DIR}/include)

add_subdirectory(libc)
add_subdirectory(libc++)

install(DIRECTORY ${CMAKE_BINARY_DIR}/lib/ DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(DIRECTORY ${CMAKE_BINARY_DIR}/include/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
