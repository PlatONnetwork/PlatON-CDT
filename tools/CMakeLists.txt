set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/)

add_subdirectory(platon-cpp)
add_subdirectory(platon-sol)

option(TEST "This is a default option for TEST" OFF)
message(STATUS "build platon test switch TEST: ${TEST}")
if(${TEST} STREQUAL "ON")
    message(STATUS "start to build platon test.")
    add_subdirectory(platon-test)
elseif(${TEST} STREQUAL "OFF")
   message(STATUS "Don't build platon test.")
endif(${TEST} STREQUAL "ON")
