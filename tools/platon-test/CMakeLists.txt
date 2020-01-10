execute_process(COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/build_test.sh)

file(COPY platon-test DESTINATION ${EXECUTABLE_OUTPUT_PATH})

file(REMOVE_RECURSE platon-test)