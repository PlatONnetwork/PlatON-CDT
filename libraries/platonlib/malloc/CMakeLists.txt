cmake_minimum_required(VERSION 3.4.3)
project(malloc)


SET(SRC_FILENAMES malloc.cpp)

add_library(malloc ${SRC_FILENAMES})

target_include_directories(malloc
                           PUBLIC 
						   ../../libc++/libcxx/include/
							../../libc/musl/include/
							../ ../../boost/include/
							../include/)

target_link_libraries( malloc c c++ )


set_target_properties(malloc
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${LIB_OUTPUT}
)


