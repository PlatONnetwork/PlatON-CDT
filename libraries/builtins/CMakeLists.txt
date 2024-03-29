cmake_minimum_required(VERSION 3.4.3)
project(CompilerRTBuiltins)


# added to a sub-directory and not this CMakeLists file.
set(GENERIC_SOURCES
  absvdi2.c
  absvsi2.c
  absvti2.c
  adddf3.c
  addsf3.c
  addtf3.c
  addvdi3.c
  addvsi3.c
  addvti3.c
  apple_versioning.c
  ashldi3.c
  ashlti3.c
  ashrdi3.c
  ashrti3.c
  bswapdi2.c
  bswapsi2.c
  clzdi2.c
  clzsi2.c
  clzti2.c
  cmpdi2.c
  cmpti2.c
  comparedf2.c
  comparesf2.c
  ctzdi2.c
  ctzsi2.c
  ctzti2.c
  divdc3.c
  divdf3.c
  divdi3.c
  divmoddi4.c
  divmodsi4.c
  divsc3.c
  divsf3.c
  divsi3.c
  divtc3.c
  divti3.c
  divtf3.c
  extendsfdf2.c
  extendhfsf2.c
  ffsdi2.c
  ffssi2.c
  ffsti2.c
  fixdfdi.c
  fixdfsi.c
  fixdfti.c
  fixsfdi.c
  fixsfsi.c
  fixsfti.c
  fixunsdfdi.c
  fixunsdfsi.c
  fixunsdfti.c
  fixunssfdi.c
  fixunssfsi.c
  fixunssfti.c
  floatdidf.c
  floatdisf.c
  floatsidf.c
  floatsisf.c
  floattidf.c
  floattisf.c
  floatundidf.c
  floatundisf.c
  floatunsidf.c
  floatunsisf.c
  floatuntidf.c
  floatuntisf.c
  int_util.c
  lshrdi3.c
  lshrti3.c
  moddi3.c
  modsi3.c
  modti3.c
  muldc3.c
  muldf3.c
  muldi3.c
  mulodi4.c
  mulosi4.c
  muloti4.c
  mulsc3.c
  mulsf3.c
  multi3.c
  multf3.c
  mulvdi3.c
  mulvsi3.c
  mulvti3.c
  negdf2.c
  negdi2.c
  negsf2.c
  negti2.c
  negvdi2.c
  negvsi2.c
  negvti2.c
  os_version_check.c
  paritydi2.c
  paritysi2.c
  parityti2.c
  popcountdi2.c
  popcountsi2.c
  popcountti2.c
  powidf2.c
  powisf2.c
  powitf2.c
  subdf3.c
  subsf3.c
  subvdi3.c
  subvsi3.c
  subvti3.c
  subtf3.c
  trampoline_setup.c
  truncdfhf2.c
  truncdfsf2.c
  truncsfhf2.c
  ucmpdi2.c
  ucmpti2.c
  udivdi3.c
  udivmoddi4.c
  udivmodsi4.c
  udivmodti4.c
  udivsi3.c
  udivti3.c
  umoddi3.c
  umodsi3.c
  umodti3.c
)

set(GENERIC_TF_SOURCES
  comparetf2.c
  extenddftf2.c
  extendsftf2.c
  fixtfdi.c
  fixtfsi.c
  fixtfti.c
  fixunstfdi.c
  fixunstfsi.c
  fixunstfti.c
  floatditf.c
  floatsitf.c
  floattitf.c
  floatunditf.c
  floatunsitf.c
  floatuntitf.c
  multc3.c
  trunctfdf2.c
  trunctfsf2.c
)

option(COMPILER_RT_EXCLUDE_ATOMIC_BUILTIN
  "Skip the atomic builtin (these should normally be provided by a shared library)"
  On)


if(COMPILER_RT_HAS_ATOMIC_KEYWORD AND NOT COMPILER_RT_EXCLUDE_ATOMIC_BUILTIN)
  set(GENERIC_SOURCES
    ${GENERIC_SOURCES}
    atomic.c
  )
endif()

set(wasm32_SOURCES
  ${GENERIC_TF_SOURCES}
  ${GENERIC_SOURCES}
)
set(wasm64_SOURCES
  ${GENERIC_TF_SOURCES}
  ${GENERIC_SOURCES}
)

add_library(builtins STATIC ${wasm32_SOURCES} )

target_include_directories(builtins PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/../libc/musl/arch/platon"
        "${CMAKE_CURRENT_SOURCE_DIR}/../libc/musl/include" )

set_target_properties(builtins
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${LIB_OUTPUT}
        )