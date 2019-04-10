cmake_minimum_required(VERSION 3.5)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 0)

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
include(CMakeModules/LibrariesExternalProject.txt)

include(CMakeModules/InstallCDT.cmake)
