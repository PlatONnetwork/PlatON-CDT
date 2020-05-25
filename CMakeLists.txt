cmake_minimum_required(VERSION 3.4.3)

# build type
if(NOT CDT_BUILD_TYPE)
  set(CDT_BUILD_TYPE Release)
endif()
message(STATUS "CDT build type: ${CDT_BUILD_TYPE}")
if(${CDT_BUILD_TYPE} STREQUAL "Release")
  add_definitions("-DNDEBUG")
  add_definitions("-DBOOST_DISABLE_ASSERTS")
  add_definitions("-DBOOST_EXCEPTION_DISABLE")
elseif(${CDT_BUILD_TYPE} STREQUAL "Debug")
  message(STATUS "Debug mode is on, assertion checking will be enabled, and log files will print debugging and error messages.")
else()
  message(FATAL_ERROR "Invalid build type, please select Debug or Release mode.")
endif()


# install directory
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
