cmake_minimum_required(VERSION 3.4.3)
project(platonlib)


SET(SRC_FILENAMES RLP.cpp abort.cpp container/vector.cpp)

add_library(platonlib ${SRC_FILENAMES})

target_include_directories(platonlib
                           PUBLIC 
						   ../../libc++/libcxx/include/
							../../libc/musl/include/
							../ ../../boost/include/
							../include/)

target_link_libraries( platonlib c c++ )


set_target_properties(platonlib
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${LIB_OUTPUT}
)


