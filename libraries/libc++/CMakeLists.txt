cmake_minimum_required(VERSION 3.4.3)
project(libc++)


SET(SRC_FILENAMES algorithm.cpp any.cpp bind.cpp condition_variable.cpp functional.cpp
                  future.cpp ios.cpp iostream.cpp locale.cpp memory.cpp mutex.cpp new.cpp optional.cpp
                  regex.cpp stdexcept.cpp string.cpp strstream.cpp system_error.cpp exception.cpp
                  typeinfo.cpp utility.cpp valarray.cpp variant.cpp vector.cpp platon.cpp)

SET(SRC_FILES "")
FOREACH(FN ${SRC_FILENAMES})
   LIST(APPEND SRC_FILES "libcxx/src/${FN}")
ENDFOREACH(FN)

add_library(c++
            ${SRC_FILES})

target_include_directories(c++
                           PUBLIC 
                           ${CMAKE_CURRENT_SOURCE_DIR}/libcxx/include
                           ${CMAKE_CURRENT_SOURCE_DIR}/../libc/musl/arch/platon
                           ${CMAKE_CURRENT_SOURCE_DIR}/../libc/musl/include)

target_link_libraries(c++ c)

set_target_properties(c++
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${LIB_OUTPUT}
)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/libcxx/include/ DESTINATION ${INC_OUTPUT}/libcxx)
