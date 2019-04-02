set(CMAKE_C_FLAGS " ${CMAKE_C_FLAGS} -O3 -Wall -DWASM_COMPILE --target=wasm32")
set(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -O3 -Wall -DWASM_COMPILE --target=wasm32")

add_subdirectory(libc)
add_subdirectory(libc++)
