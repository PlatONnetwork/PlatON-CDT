set(WASM_BIN ${CMAKE_SOURCE_DIR}/build/thirdparty/Source/PlatonCmdWasm/build/bin)
message(${WASM_BIN})
add_test(wasm
  ${WASM_BIN}/wasm unittest -dir ${CMAKE_BINARY_DIR}/tests/unit -outdir ${CMAKE_BINARY_DIR}/tests/unit)
