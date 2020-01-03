cmake_minimum_required(VERSION 3.4.3)
project(platonlib)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-everything --target=wasm32 -nostdlib -fno-builtin -ffreestanding  -fno-threadsafe-statics  -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -Wno-everything --target=wasm32 -nostdlib -fno-builtin -ffreestanding  -fno-threadsafe-statics -O3 -fno-rtti")

SET(SRC_FILENAMES RLP.cpp malloc.cpp)

add_library(platonlib ${SRC_FILENAMES})

target_include_directories(platonlib
                           PUBLIC 
						   ../../libc++/libcxx/include/
							../../libc/musl/include/
							../ ../../boost/include/
							../include/)

target_link_libraries( platonlib c c++ )

INSTALL(TARGETS platonlib
		ARCHIVE DESTINATION lib)
