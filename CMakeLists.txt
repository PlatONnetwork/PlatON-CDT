cmake_minimum_required(VERSION 3.5)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 5)

set(VERSION_FULL "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  message(WARNING "CMAKE_INSTALL_PREFIX is set to default path of ${CMAKE_INSTALL_PREFIX}, resetting to ${CMAKE_INSTALL_PREFIX}/platon.cdt")
  set(CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}/platon.cdt")
 elseif ("${CMAKE_INSTALL_PREFIX}" STREQUAL "/usr/local")
   message(WARNING "CMAKE_INSTALL_PREFIX is explicitly set to /usr/local. This is not recommanded.")
endif()

include(GNUInstallDirs)

set(BIN_SUFFIX)
if (WIN32)
  set(BIN_SUFFIX .exe)
endif()

include(CMakeModules/LLVMExternalProject.txt)
include(CMakeModules/BoostExternalProject.txt)
include(CMakeModules/ToolsExternalProject.txt)

set(CDT_ROOT_DIR ${CMAKE_BINARY_DIR})
configure_file(${CMAKE_SOURCE_DIR}/CMakeModules/platon.cdt-config.cmake ${CMAKE_BINARY_DIR}/lib/cmake/platon.cdt/platon.cdt-config.cmake @ONLY)
configure_file(${CMAKE_SOURCE_DIR}/CMakeModules/PlatonCDTMacros.cmake.in ${CMAKE_BINARY_DIR}/lib/cmake/platon.cdt/PlatonCDTMacros.cmake @ONLY)
configure_file(${CMAKE_SOURCE_DIR}/CMakeModules/PlatonWasmToolchain.cmake.in ${CMAKE_BINARY_DIR}/lib/cmake/platon.cdt/PlatonWasmToolchain.cmake @ONLY)

set(CDT_ROOT_DIR ${CMAKE_INSTALL_PREFIX})
configure_file(${CMAKE_SOURCE_DIR}/CMakeModules/platon.cdt-config.cmake ${CMAKE_BINARY_DIR}/CMakeModules/platon.cdt-config.cmake @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/CMakeModules/platon.cdt-config.cmake DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake/platon.cdt)
configure_file(${CMAKE_SOURCE_DIR}/CMakeModules/PlatonCDTMacros.cmake.in ${CMAKE_BINARY_DIR}/CMakeModules/PlatonCDTMacros.cmake @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/CMakeModules/PlatonCDTMacros.cmake DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake/platon.cdt)
configure_file(${CMAKE_SOURCE_DIR}/CMakeModules/PlatonWasmToolchain.cmake.in ${CMAKE_BINARY_DIR}/CMakeModules/PlatonWasmToolchain.cmake @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/CMakeModules/PlatonWasmToolchain.cmake DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake/platon.cdt)

set(CDT_ROOT_DIR "_PREFIX_")
configure_file(${CMAKE_SOURCE_DIR}/CMakeModules/platon.cdt-config.cmake ${CMAKE_BINARY_DIR}/CMakeModules/platon.cdt-config.cmake.package)
configure_file(${CMAKE_SOURCE_DIR}/CMakeModules/PlatonCDTMacros.cmake.in ${CMAKE_BINARY_DIR}/CMakeModules/PlatonCDTMacrosPackage.cmake)
configure_file(${CMAKE_SOURCE_DIR}/CMakeModules/PlatonWasmToolchain.cmake.in ${CMAKE_BINARY_DIR}/CMakeModules/PlatonWasmToolchainPackage.cmake)

include(CMakeModules/LibrariesExternalProject.txt)

include(CMakeModules/InstallCDT.cmake)
