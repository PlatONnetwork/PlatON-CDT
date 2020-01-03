cmake_minimum_required(VERSION 3.4.3)


if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  message(WARNING "CMAKE_INSTALL_PREFIX is set to default path of ${CMAKE_INSTALL_PREFIX}, resetting to ${CMAKE_BINARY_DIR}/platon.cdt")
  set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/platon.cdt")
 elseif ("${CMAKE_INSTALL_PREFIX}" STREQUAL "/usr/local")
   message(WARNING "CMAKE_INSTALL_PREFIX is explicitly set to /usr/local. This is not recommanded.")
endif()

if(NOT DEFINED LLVM_CONFIG)
  find_program(LLVM_CONFIG NAMES llvm-config-8.0)

  if(NOT LLVM_CONFIG AND EXISTS clang_llvm_8.0)
    set(LLVM_CONFIG clang_llvm_8.0/bin/llvm-config)
  endif()

  if(NOT LLVM_CONFIG)
    execute_process(COMMAND wget http://releases.llvm.org/8.0.0/clang+llvm-8.0.0-x86_64-linux-gnu-ubuntu-18.04.tar.xz)
    execute_process(COMMAND tar xavf clang+llvm-8.0.0-x86_64-linux-gnu-ubuntu-18.04.tar.xz)
    execute_process(COMMAND mv clang+llvm-8.0.0-x86_64-linux-gnu-ubuntu-18.04 clang_llvm_8.0)
    set(LLVM_CONFIG clang_llvm_8.0/bin/llvm-config)
  endif()

endif()

set(LLVM_CONFIG_FILE ${LLVM_CONFIG})

execute_process(COMMAND ${LLVM_CONFIG} --bindir
  OUTPUT_VARIABLE LLVM_BINDIR
  OUTPUT_STRIP_TRAILING_WHITESPACE)

SET(CMAKE_C_COMPILER ${LLVM_BINDIR}/clang)
SET(CMAKE_CXX_COMPILER ${LLVM_BINDIR}/clang++)
SET(CMAKE_AR ${LLVM_BINDIR}/llvm-ar  CACHE FILEPATH "" FORCE)
SET(CMAKE_RANLIB ${LLVM_BINDIR}/llvm-ranlib)

project(PlatON-CDT)

add_subdirectory(tools/platon-cpp)
add_subdirectory(libraries)


