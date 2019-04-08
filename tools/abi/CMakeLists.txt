set( CMAKE_CXX_STANDARD 14 )
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -DRAPIDJSON_HAS_STDSTRING")

if (WIN32)
	set(SYSTEM_LIB version)
elseif (APPLE)
	set(SYSTEM_LIB z dl pthread m curses)
else()
	set(SYSTEM_LIB z rt dl tinfo pthread m)
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/main.cpp ${CMAKE_BINARY_DIR}/platon-abigen.cpp)
set(SOURCES ${CMAKE_BINARY_DIR}/platon-abigen.cpp AbiGenerator.cpp AbiMacro.cpp AbiJson.cpp Log.cpp StringUtil.cpp Template.cpp)
add_executable(platon-abigen ${SOURCES})

target_include_directories(platon-abigen PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../../libraries/rapidjson/include)
target_link_libraries(platon-abigen
  ${LLVM_LIBRARY_DIRS}/libclangBasic.a
  ${LLVM_LIBRARY_DIRS}/libclangFormat.a
  ${LLVM_LIBRARY_DIRS}/libclangRewrite.a
  ${LLVM_LIBRARY_DIRS}/libclangFrontendTool.a
  ${LLVM_LIBRARY_DIRS}/libclangTooling.a
  ${LLVM_LIBRARY_DIRS}/libclangFrontend.a
  ${LLVM_LIBRARY_DIRS}/libclangToolingCore.a
  ${LLVM_LIBRARY_DIRS}/libclangDriver.a
  ${LLVM_LIBRARY_DIRS}/libclangSerialization.a
  ${LLVM_LIBRARY_DIRS}/libclangParse.a
  ${LLVM_LIBRARY_DIRS}/libclangSema.a
  ${LLVM_LIBRARY_DIRS}/libclangAnalysis.a
  ${LLVM_LIBRARY_DIRS}/libclangAST.a
  ${LLVM_LIBRARY_DIRS}/libclangBasic.a
  ${LLVM_LIBRARY_DIRS}/libclangEdit.a
  ${LLVM_LIBRARY_DIRS}/libclangLex.a

  ${LLVM_LIBRARY_DIRS}/libLLVMExecutionEngine.a
  ${LLVM_LIBRARY_DIRS}/libLLVMRuntimeDyld.a
  ${LLVM_LIBRARY_DIRS}/libLLVMDebugInfoDWARF.a
  ${LLVM_LIBRARY_DIRS}/libLLVMX86Disassembler.a
  ${LLVM_LIBRARY_DIRS}/libLLVMX86AsmParser.a
  ${LLVM_LIBRARY_DIRS}/libLLVMX86CodeGen.a
  ${LLVM_LIBRARY_DIRS}/libLLVMAsmPrinter.a
  ${LLVM_LIBRARY_DIRS}/libLLVMDebugInfoCodeView.a
  ${LLVM_LIBRARY_DIRS}/libLLVMDebugInfoMSF.a
  ${LLVM_LIBRARY_DIRS}/libLLVMGlobalISel.a
  ${LLVM_LIBRARY_DIRS}/libLLVMSelectionDAG.a
  ${LLVM_LIBRARY_DIRS}/libLLVMX86Desc.a
  ${LLVM_LIBRARY_DIRS}/libLLVMX86AsmPrinter.a
  ${LLVM_LIBRARY_DIRS}/libLLVMX86Info.a
  ${LLVM_LIBRARY_DIRS}/libLLVMX86Utils.a
  ${LLVM_LIBRARY_DIRS}/libLLVMCodeGen.a
  ${LLVM_LIBRARY_DIRS}/libLLVMipo.a
  ${LLVM_LIBRARY_DIRS}/libLLVMBitWriter.a
  ${LLVM_LIBRARY_DIRS}/libLLVMIRReader.a
  ${LLVM_LIBRARY_DIRS}/libLLVMAsmParser.a
  ${LLVM_LIBRARY_DIRS}/libLLVMInstrumentation.a
  ${LLVM_LIBRARY_DIRS}/libLLVMLinker.a
  ${LLVM_LIBRARY_DIRS}/libLLVMVectorize.a
  ${LLVM_LIBRARY_DIRS}/libLLVMScalarOpts.a
  ${LLVM_LIBRARY_DIRS}/libLLVMInstCombine.a
  ${LLVM_LIBRARY_DIRS}/libLLVMTransformUtils.a
  ${LLVM_LIBRARY_DIRS}/libLLVMTarget.a
  ${LLVM_LIBRARY_DIRS}/libLLVMAnalysis.a
  ${LLVM_LIBRARY_DIRS}/libLLVMOption.a
  ${LLVM_LIBRARY_DIRS}/libLLVMMCDisassembler.a
  ${LLVM_LIBRARY_DIRS}/libLLVMProfileData.a
  ${LLVM_LIBRARY_DIRS}/libLLVMObject.a
  ${LLVM_LIBRARY_DIRS}/libLLVMMCParser.a
  ${LLVM_LIBRARY_DIRS}/libLLVMMC.a
  ${LLVM_LIBRARY_DIRS}/libLLVMCore.a
  ${LLVM_LIBRARY_DIRS}/libLLVMBitReader.a
  ${LLVM_LIBRARY_DIRS}/libLLVMSupport.a
  ${LLVM_LIBRARY_DIRS}/libLLVMDemangle.a
  ${LLVM_LIBRARY_DIRS}/libLLVMBinaryFormat.a
    
  libboost_filesystem.a
  libboost_system.a
  libboost_log.a
  libboost_log_setup.a
  libboost_thread.a
  libboost_random.a

  ${SYSTEM_LIB}
  )

  add_custom_command(TARGET platon-abigen POST_BUILD COMMAND mkdir -p ${CMAKE_BINARY_DIR}/bin)
  add_custom_command(TARGET platon-abigen POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:platon-abigen> ${CMAKE_BINARY_DIR}/bin)

  install(FILES ${CMAKE_BINARY_DIR}/bin/platon-abigen
    DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
