cmake_minimum_required(VERSION 3.4.3)

if(NOT CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX /usr/local)
else()
  message("CMAKE_INSTALL_PREFIX already set to: ${CMAKE_INSTALL_PREFIX}")
endif()

add_subdirectory(depends)

execute_process(COMMAND ${LLVM_CONFIG} --bindir
  OUTPUT_VARIABLE LLVM_BINDIR
  OUTPUT_STRIP_TRAILING_WHITESPACE)

SET(CMAKE_C_COMPILER ${LLVM_BINDIR}/clang)
SET(CMAKE_CXX_COMPILER ${LLVM_BINDIR}/clang++)
SET(CMAKE_AR ${LLVM_BINDIR}/llvm-ar  CACHE FILEPATH "" FORCE)
SET(CMAKE_RANLIB ${LLVM_BINDIR}/llvm-ranlib)

project(PlatON-CDT)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/)

add_subdirectory(tools)
add_subdirectory(libraries)
