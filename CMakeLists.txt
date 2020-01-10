cmake_minimum_required(VERSION 3.4.3)

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

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/)

add_subdirectory(tools)
add_subdirectory(libraries)

enable_testing()
add_test(NAME unit-test COMMAND "${EXECUTABLE_OUTPUT_PATH}/platon-test" ../../tests/unit --verbose)

