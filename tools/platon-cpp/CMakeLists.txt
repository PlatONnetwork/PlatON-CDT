cmake_minimum_required(VERSION 3.4.3)

execute_process(COMMAND ${LLVM_CONFIG} --includedir
  OUTPUT_VARIABLE LLVM_INCLUDEDIR
  OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(COMMAND ${LLVM_CONFIG} --libdir
  OUTPUT_VARIABLE LLVM_LIBDIR
  OUTPUT_STRIP_TRAILING_WHITESPACE)


set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-fPIC -fvisibility-inlines-hidden -Werror=date-time -std=c++11 -Wall -W -Wno-unused-parameter -Wwrite-strings -Wcast-qual -Wno-missing-field-initializers -pedantic -Wno-long-long -Wno-uninitialized -Wdelete-non-virtual-dtor -Wno-comment -g  -fno-exceptions -fno-rtti -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS")

include_directories(${LLVM_INCLUDEDIR})

link_directories(${LLVM_LIBDIR})

add_executable(platon-cpp
  parseArgs.cpp
  platon-cpp.cpp
  Backend.cpp
  PCCPass.cpp
  RemoveAttrs.cpp
  )

file(COPY ${LLVM_BINDIR}/lld DESTINATION ${EXECUTABLE_OUTPUT_PATH}) 
file(RENAME ${EXECUTABLE_OUTPUT_PATH}/lld ${EXECUTABLE_OUTPUT_PATH}/platon-lld)

add_subdirectory(MakeAbi)
target_link_libraries(platon-cpp MakeAbi)


target_link_libraries(platon-cpp
  clangCodeGen 
  clangTooling 
  clangFrontend 
  clangASTMatchers 
  clangParse 
  clangSerialization 
  clangSema 
  clangEdit  
  clangAnalysis 
  clangDriver 
  clangFormat 
  clangToolingCore 
  clangAST 
  clangRewrite 
  clangLex 
  clangBasic
  )

target_link_libraries(platon-cpp
  LLVMLTO 
  LLVMPasses 
  LLVMObjCARCOpts 
  LLVMMIRParser 
  LLVMSymbolize 
  LLVMDebugInfoPDB 
  LLVMDebugInfoDWARF 
  LLVMCoverage 
  LLVMTableGen 
  LLVMDlltoolDriver 
  LLVMOrcJIT 

  LLVMOption

  LLVMWebAssemblyDisassembler
  LLVMWebAssemblyCodeGen
  LLVMWebAssemblyDesc
  LLVMWebAssemblyAsmPrinter
  LLVMWebAssemblyAsmParser
  LLVMWebAssemblyInfo

  LLVMAggressiveInstCombine

  LLVMGlobalISel 
  LLVMSelectionDAG 
  LLVMAsmPrinter 
  LLVMDebugInfoCodeView 
  LLVMDebugInfoMSF 
  LLVMMCDisassembler 
  LLVMMCJIT 
  LLVMLineEditor 
  LLVMInterpreter 
  LLVMExecutionEngine 
  LLVMRuntimeDyld 
  LLVMCodeGen 
  LLVMTarget 
  LLVMCoroutines 
  LLVMipo 
  LLVMInstrumentation 
  LLVMVectorize 
  LLVMScalarOpts 
  LLVMLinker 
  LLVMIRReader 
  LLVMAsmParser 
  LLVMInstCombine 
  LLVMTransformUtils 
  LLVMBitWriter 
  LLVMAnalysis 
  LLVMProfileData 
  LLVMObject 
  LLVMMCParser 
  LLVMMC 
  LLVMBitReader 
  LLVMCore 
  LLVMBinaryFormat 
  LLVMSupport 
  LLVMDemangle
  

)

target_link_libraries(platon-cpp
  z 
  rt 
  dl 
  tinfo 
  pthread 
  m
)
