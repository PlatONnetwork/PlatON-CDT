cmake_minimum_required(VERSION 3.5)
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${LLVM_SRCDIR}/tools/clang/include)
include_directories(${LLVM_BINDIR}/tools/clang/include)
include_directories(${LLVM_SRCDIR}/include)
include_directories(${LLVM_BINDIR}/include)
link_libraries(${LLVM_LIBRARY_DIRS})
add_definitions(${LLVM_DEFINITIONS})

set(Boost_USE_STATIC_LIBS ON)  # only find static libs
find_package(Boost REQUIRED COMPONENTS system filesystem log random thread)

include_directories(${Boost_INCLUDE_DIRS})
link_libraries(${Boost_LIBRARY_DIRS})

macro(add_tool name)
  set(LLVM_LINK_COMPONENTS support)
  include_directories(include)

  add_executable(${name} ${CMAKE_BINARY_DIR}/${name}.cpp)
  set_property(TARGET ${name} PROPERTY CXX_STANDARD 14)
  target_include_directories(${name} PUBLIC ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../include ${CMAKE_SOURCE_DIR}/platon_llvm/tools/clang/include ${LLVM_INCLUDE_DIR})
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

    ${Boost_LIBRARIES}
    )

  add_custom_command(TARGET ${name} POST_BUILD COMMAND mkdir -p ${CMAKE_BINARY_DIR}/bin)
  add_custom_command(TARGET ${name} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${name}> ${CMAKE_BINARY_DIR}/bin)
endmacro()

add_subdirectory(cpp)
add_subdirectory(ld)
add_subdirectory(init)
add_subdirectory(abi)
