cmake_minimum_required(VERSION 3.4.3)
set(CMAKE_C_FLAGS " ${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS}  -Wall")

message(STATUS "libraries dir ${CMAKE_CURRENT_SOURCE_DIR}")

set(PLATON_CDT ${CMAKE_BINARY_DIR}/platon.cdt)
set(LIB_OUTPUT ${PLATON_CDT}/lib)
set(INC_OUTPUT ${PLATON_CDT}/include)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-everything --target=wasm32 -nostdlib -fno-builtin -ffreestanding  -fno-threadsafe-statics  -Oz")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-everything --target=wasm32 -nostdlib -nostdinc -fno-builtin -ffreestanding  -fno-threadsafe-statics  -Oz  -fno-rtti -fno-exceptions -std=c++17")

add_subdirectory(libc)
add_subdirectory(libc++)
add_subdirectory(builtins)
add_subdirectory(platonlib/src)


file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/platonlib/include/ DESTINATION ${INC_OUTPUT})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/boost/include/ DESTINATION ${INC_OUTPUT})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/extern_symbol DESTINATION ${LIB_OUTPUT})

install(DIRECTORY ${PLATON_CDT} DESTINATION ${CMAKE_INSTALL_PREFIX})
