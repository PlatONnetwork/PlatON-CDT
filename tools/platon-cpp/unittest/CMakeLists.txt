cmake_minimum_required(VERSION 3.4.3)

add_executable(abi-test
  ABITest.cpp
  )

target_link_libraries(abi-test MakeAbi)


target_link_libraries(abi-test
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
  LLVMRemarks
  LLVMBitstreamReader
  LLVMBinaryFormat
  LLVMSupport
  LLVMDemangle
)

if (APPLE)
  target_link_libraries(abi-test
    z
    dl
    pthread
    curses
    m)
else()
  target_link_libraries(abi-test
    z
    rt
    dl
    tinfo
    pthread
    m)
endif()

