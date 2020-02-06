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

install(TARGETS platon-cpp RUNTIME DESTINATION bin)

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
  lldDriver
  lldWasm
  lldCommon
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

  LLVMXCoreDisassembler
  LLVMXCoreCodeGen
  LLVMXCoreDesc
  LLVMXCoreInfo
  LLVMXCoreAsmPrinter
  LLVMSystemZDisassembler
  LLVMSystemZCodeGen
  LLVMSystemZAsmParser
  LLVMSystemZDesc
  LLVMSystemZInfo
  LLVMSystemZAsmPrinter
  LLVMSparcDisassembler
  LLVMSparcCodeGen
  LLVMSparcAsmParser
  LLVMSparcDesc
  LLVMSparcInfo
  LLVMSparcAsmPrinter
  LLVMPowerPCDisassembler
  LLVMPowerPCCodeGen
  LLVMPowerPCAsmParser
  LLVMPowerPCDesc
  LLVMPowerPCInfo
  LLVMPowerPCAsmPrinter
  LLVMNVPTXCodeGen
  LLVMNVPTXDesc
  LLVMNVPTXInfo
  LLVMNVPTXAsmPrinter
  LLVMMSP430Disassembler
  LLVMMSP430CodeGen
  LLVMMSP430AsmParser
  LLVMMSP430Desc
  LLVMMSP430Info
  LLVMMSP430AsmPrinter
  LLVMMipsDisassembler
  LLVMMipsCodeGen
  LLVMMipsAsmParser
  LLVMMipsDesc
  LLVMMipsInfo
  LLVMMipsAsmPrinter
  LLVMLanaiDisassembler
  LLVMLanaiCodeGen
  LLVMLanaiAsmParser
  LLVMLanaiDesc
  LLVMLanaiAsmPrinter
  LLVMLanaiInfo
  LLVMHexagonDisassembler
  LLVMHexagonCodeGen
  LLVMHexagonAsmParser
  LLVMHexagonDesc
  LLVMHexagonInfo
  LLVMBPFDisassembler
  LLVMBPFCodeGen
  LLVMBPFAsmParser
  LLVMBPFDesc
  LLVMBPFInfo
  LLVMBPFAsmPrinter
  LLVMARMDisassembler
  LLVMARMCodeGen
  LLVMARMAsmParser
  LLVMARMDesc
  LLVMARMInfo
  LLVMARMAsmPrinter
  LLVMARMUtils
  LLVMAMDGPUDisassembler
  LLVMAMDGPUCodeGen
  LLVMAMDGPUAsmParser
  LLVMAMDGPUDesc
  LLVMAMDGPUInfo
  LLVMAMDGPUAsmPrinter
  LLVMAMDGPUUtils
  LLVMAArch64Disassembler
  LLVMAArch64CodeGen
  LLVMAArch64AsmParser
  LLVMAArch64Desc
  LLVMAArch64Info
  LLVMAArch64AsmPrinter
  LLVMAArch64Utils
  LLVMObjectYAML
  LLVMLibDriver
  LLVMOption
  LLVMOptRemarks
  LLVMWindowsManifest
  LLVMTextAPI
  LLVMFuzzMutate
  LLVMX86Disassembler
  LLVMX86AsmParser
  LLVMX86CodeGen
  LLVMGlobalISel
  LLVMSelectionDAG
  LLVMAsmPrinter
  LLVMX86Desc
  LLVMMCDisassembler
  LLVMX86Info
  LLVMX86AsmPrinter
  LLVMX86Utils


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

if (APPLE)
  target_link_libraries(platon-cpp
    z
    dl
    pthread
    curses
    m)
else()
  target_link_libraries(platon-cpp
    z
    rt
    dl
    tinfo
    pthread
    m)
endif()
