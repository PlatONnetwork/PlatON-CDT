SET(SRC_FILENAMES algorithm.cpp any.cpp bind.cpp condition_variable.cpp exception.cpp functional.cpp
                  future.cpp ios.cpp iostream.cpp locale.cpp memory.cpp mutex.cpp new.cpp optional.cpp
                  regex.cpp shared_mutex.cpp stdexcept.cpp string.cpp strstream.cpp system_error.cpp
                  thread.cpp typeinfo.cpp utility.cpp valarray.cpp variant.cpp vector.cpp)

#SET(SRC_FILENAMES exception.cpp)

SET(SRC_FILES "")
FOREACH(FN ${SRC_FILENAMES})
   LIST(APPEND SRC_FILES "libcxx/src/${FN}")
ENDFOREACH(FN)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -emit-llvm -Wno-everything -ffreestanding -fno-threadsafe-statics -fno-rtti -fno-exceptions -w")

add_library(c++ ${SRC_FILES})

include_directories(SYSTEM
  ${CMAKE_CURRENT_SOURCE_DIR}/../libc/musl/include)

target_include_directories(c++
                           PUBLIC
                           ${CMAKE_CURRENT_SOURCE_DIR}/../libc/musl/include
                           ${CMAKE_CURRENT_SOURCE_DIR}/../libc/musl/src/internal
                           ${CMAKE_CURRENT_SOURCE_DIR}/../libc/musl/src/crypt
                           ${CMAKE_CURRENT_SOURCE_DIR}/../libc/musl/arch/platon
                           ${CMAKE_CURRENT_SOURCE_DIR}/libcxx/include)

target_link_libraries(c++ c)

add_custom_command(TARGET c++ POST_BUILD COMMAND mkdir -p ${BINARY_DIR}/lib)
add_custom_command( TARGET c++ POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:c++> ${BINARY_DIR}/lib/ )

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/libcxx/include/ DESTINATION ${BINARY_DIR}/include/libcxx)
