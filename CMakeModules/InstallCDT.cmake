add_custom_command(TARGET PlatonLLVM POST_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin)
macro(platon_llvm_install file)
  set(BINARY_DIR ${CMAKE_BINARY_DIR}/platon_llvm/bin)
  add_custom_command(TARGET PlatonLLVM POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${BINARY_DIR}/${file}${BIN_SUFFIX} ${CMAKE_BINARY_DIR}/bin/${file}${BIN_SUFFIX})
  install(FILES ${BINARY_DIR}/${file}${BIN_SUFFIX}
    DESTINATION ${CMAKE_INSTALL_FULL_BINDIR}
    PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endmacro(platon_llvm_install)

macro(platon_tools_install file)
  set(BINARY_DIR ${CMAKE_BINARY_DIR}/tools/bin)
  add_custom_command(TARGET PlatonTools POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${BINARY_DIR}/${file}${BIN_SUFFIX} ${CMAKE_BINARY_DIR}/bin/${file}${BIN_SUFFIX})
  install(FILES ${BINARY_DIR}/${file}${BIN_SUFFIX}
    DESTINATION ${CMAKE_INSTALL_FULL_BINDIR}
    PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endmacro(platon_tools_install)

macro(platon_libraries_install)
   execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/lib)
   execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/include)
   install(DIRECTORY ${CMAKE_BINARY_DIR}/lib/ DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
   install(DIRECTORY ${CMAKE_BINARY_DIR}/include/ DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR})
   #install(DIRECTORY ${CMAKE_SOURCE_DIR}/libraries/boost/include/ DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR})
   #install(DIRECTORY ${CMAKE_SOURCE_DIR}/libraries/rapidjson/include/ DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR})
   #install(DIRECTORY ${CMAKE_SOURCE_DIR}/libraries/platonlib/include/ DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR})
endmacro(platon_libraries_install)

set(CLANG clang-7)
if (WIN32)
  set(CLANG clang)
endif()

platon_llvm_install(${CLANG})
platon_llvm_install(opt)
platon_llvm_install(llc)
platon_llvm_install(lld)
platon_llvm_install(wasm-ld)
platon_tools_install(platon-cpp)
platon_tools_install(platon-ld)
platon_tools_install(platon-init)
platon_tools_install(platon-abigen)
platon_libraries_install()
