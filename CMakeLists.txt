cmake_minimum_required(VERSION 3.4.3)

project(PlatON-CDT)

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

execute_process(COMMAND ${LLVM_CONFIG} --bindir
  OUTPUT_VARIABLE LLVM_BINDIR
  OUTPUT_STRIP_TRAILING_WHITESPACE)



add_subdirectory(tools/platon-cpp)

include(CTest)
enable_testing()
add_subdirectory(tests)
