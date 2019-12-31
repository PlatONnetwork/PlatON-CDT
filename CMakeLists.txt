cmake_minimum_required(VERSION 3.4.3)


add_subdirectory(tools/platon-cpp)

include(CTest)
enable_testing()
add_subdirectory(tests)
