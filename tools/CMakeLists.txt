cmake_minimum_required(VERSION 3.5)
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
link_libraries(${LLVM_LIBRARY_DIRS})
add_definitions(${LLVM_DEFINITIONS})

macro(add_tool name)
  set(LLVM_LINK_COMPOENTS support)

  include_directories(include)

  add_executable(${name} ${CMAKE_CURRENT_SOURCE_DIR}/${name}.cpp)
  set_property(TARGET ${name} PROPERTY CXX_STANDARD 14)
  target_include_directories(${name} PUBLIC ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../include ${LLVM_INCLUDE_DIRS})
  target_link_libraries(${name}
    LLVMipo
    LLVMScalarOpts
    LLVMInstCombine
    LLVMTransformUtils
    LLVMAnalysis
    LLVMTarget
    LLVMOption
    LLVMMCParser
    LLVMMC
    LLVMObject
    LLVMBitReader
    LLVMCore
    LLVMSupport
    LLVMDemangle

    libboost_filesystem.a
    )

  add_custom_command(TARGET ${name} POST_BUILD COMMAND mkdir -p ${CMAKE_BINARY_DIR}/bin)
  add_custom_command(TARGET ${name} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${name}> ${CMAKE_BINARY_DIR}/bin)
endmacro()

add_subdirectory(cpp)
