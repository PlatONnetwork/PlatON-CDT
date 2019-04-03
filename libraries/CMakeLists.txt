set(CMAKE_C_FLAGS " ${CMAKE_C_FLAGS} -Oz -Wall --target=wasm32")
set(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -std=c++14 -Oz -Wall --target=wasm32")

add_subdirectory(libc)
add_subdirectory(libc++)
