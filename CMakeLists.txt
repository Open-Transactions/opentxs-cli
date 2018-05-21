cmake_minimum_required(VERSION 2.8)

project(opentxs-cli)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM LAST)
find_package(Git)
if(GIT_FOUND)
    execute_process(COMMAND git "describe" OUTPUT_VARIABLE GIT_VERSION WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(REGEX REPLACE "^([0-9]+)\\..*" "\\1" VERSION_MAJOR "${GIT_VERSION}")
    string(REGEX REPLACE "^${VERSION_MAJOR}\\.([0-9]+).*" "\\1" VERSION_MINOR "${GIT_VERSION}")
    string(REGEX REPLACE "^${VERSION_MAJOR}\\.${VERSION_MINOR}\\.([0-9]+).*" "\\1" VERSION_BUGFIX "${GIT_VERSION}")
    string(REGEX REPLACE "^${VERSION_MAJOR}\\.${VERSION_MINOR}\\.${VERSION_BUGFIX}-([0-9]+)-.*" "\\1" VERSION_NEW_COMMITS "${GIT_VERSION}")
    string(REGEX REPLACE "^${VERSION_MAJOR}\\.${VERSION_MINOR}\\.${VERSION_BUGFIX}-${VERSION_NEW_COMMITS}-(.*)" "\\1" VERSION_SHA1 "${GIT_VERSION}")
    if("${VERSION_NEW_COMMITS}" STREQUAL "${GIT_VERSION}")
        set(VERSION_STRING "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_BUGFIX}")
        set(VERSION_NEW_COMMITS "")
        set(VERSION_SHA1 "")
    else()
        set(VERSION_STRING "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_BUGFIX}-${VERSION_NEW_COMMITS}-${VERSION_SHA1}")
    endif()
    if ("${VERSION_STRING}" STREQUAL "..")
        message(FATAL_ERROR "Version string missing. Did you git clone using the --depth parameter?")
    endif()
else()
  message(FATAL_ERROR "Git not found.")
endif()


#-----------------------------------------------------------------------------
# Options for building

option(BUILD_VERBOSE       "Verbose build output." ON)
option(RPM                 "Build a RPM" OFF)
option(DEB                 "Build a DEB" OFF)

set(PACKAGE_CONTACT        ""                              CACHE <TYPE>  "Package Maintainer")
set(PACKAGE_VENDOR         "Open Transactions Developers"  CACHE <TYPE>  "Package Vendor")

if(BUILD_VERBOSE)
  set(CMAKE_VERBOSE_MAKEFILE true)
endif()

if(RPM OR DEB)
    if(NOT PACKAGE_CONTACT)
        message(FATAL_ERROR "Specify a PACKAGE_CONTACT to build packages.")
    endif()
endif()


#-----------------------------------------------------------------------------
# Print system information

message(STATUS "opentxs-cli version:      ${VERSION_STRING}")
message(STATUS "CMake version:            ${CMAKE_VERSION}")
message(STATUS "System:                   ${CMAKE_SYSTEM}")
message(STATUS "Processor:                ${CMAKE_SYSTEM_PROCESSOR}")

message(STATUS "Verbose:                  ${BUILD_VERBOSE}")
message(STATUS "Build RPM:                ${RPM}")
message(STATUS "Build DEB:                ${DEB}")
message(STATUS "Package Contact:          ${PACKAGE_CONTACT}")
message(STATUS "Package Vendor:           ${PACKAGE_VENDOR}")

#-----------------------------------------------------------------------------
# Collect all binaries into bin subdirectory

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH  ${CMAKE_CURRENT_BINARY_DIR}/lib)


#-----------------------------------------------------------------------------
# Check which compiler to use

INCLUDE(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG(-std=c++17 HAVE_STD17)

if(HAVE_STD17)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")
endif()

if(${CMAKE_CXX_COMPILER_ID} MATCHES GNU)
  set(GCC_WARN_FLAGS "-Wno-attributes -Wnoexcept -Wstrict-null-sentinel -Wlogical-op")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

elseif(${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
  set(CMAKE_COMPILER_IS_CLANGXX 1)

  if(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -stdlib=libc++")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
  endif()
endif()

if(WIN32)
  add_definitions(-D_UNICODE)
else()
  set(PEDANTIC_CXX_FLAGS "-Wall -Werror -Wextra -pedantic -Wno-missing-braces -Wcast-align -Wctor-dtor-privacy -Wdisabled-optimization -Winit-self -Wmissing-include-dirs -Wswitch-default -Wold-style-cast -Woverloaded-virtual -Wmissing-declarations -Wno-deprecated-declarations -Wno-unused-parameter -Wno-old-style-cast -Wunused-macros -Wundef -Wformat=2 -Wno-format-nonliteral ${GCC_WARN_FLAGS}")
endif()

if(CMAKE_COMPILER_IS_CLANGXX)
  set(PEDANTIC_CXX_FLAGS "${PEDANTIC_CXX_FLAGS} -Wno-unused-parameter -Wno-old-style-cast")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${PEDANTIC_CXX_FLAGS}")

message(STATUS "C++ Compiler ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "C++ Flags:       ${CMAKE_CXX_FLAGS}")
message(STATUS "C++ link flags:  ${CMAKE_CXX_LINK_FLAGS}")

#-----------------------------------------------------------------------------
# Dependencies

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

add_definitions(-DPROTOBUF_INLINE_NOT_IN_HEADERS=0)
find_package(opentxs REQUIRED)
find_package(opentxs-proto REQUIRED)
find_package(Protobuf REQUIRED)

#-----------------------------------------------------------------------------
# System libraries used for linking.

if(UNIX AND NOT APPLE)
  list(APPEND OPENTXS_CLI_SYSTEM_LIBRARIES rt)
  list(APPEND OPENTXS_CLI_SYSTEM_LIBRARIES dl)
endif()

#-----------------------------------------------------------------------------
# Force out-of-source build

if(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_BINARY_DIR})
message(FATAL_ERROR "CMake generation is not allowed within the source directory!
Remove the CMakeCache.txt file and try again from another folder, e.g.:

   rm CMakeCache.txt
   mkdir build
   cd build
   cmake ..
")
endif()


#-----------------------------------------------------------------------------
# Default to Release build

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
endif()


#-----------------------------------------------------------------------------
# Source Definitions

add_definitions(-DOPENTXS_CLI_VERSION_STRING="${VERSION_STRING}")

if(WIN32)
    add_definitions("-DEXPORT=__declspec(dllexport)")
else()
  add_definitions(-DEXPORT=)
endif()

#-----------------------------------------------------------------------------
# Build source

#Fix RPATH
if(APPLE)
  set(CMAKE_INSTALL_NAME_DIR @rpath)
endif()
if(NOT RPM)
  include(GNUInstallDirs)
  SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
  set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
endif()

LIST(APPEND CMAKE_PREFIX_PATH "/usr/share/cmake/Modules")
LIST(APPEND CMAKE_PREFIX_PATH "/usr/local/share/cmake/Modules")

set(dependency_include_dir
  ${CMAKE_CURRENT_SOURCE_DIR}/deps/
)

include_directories(SYSTEM
  ${dependency_include_dir}
  ${OPENTXS_INCLUDE_DIRS}
)

add_subdirectory(deps)
add_subdirectory(src)

#-----------------------------------------------------------------------------
# Uninstal
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake)


#-----------------------------------------------------------------------------
# Packaging
set(CPACK_GENERATOR "TGZ")

set(CPACK_PACKAGE_VERSION ${VERSION_STRING})
set(CPACK_PACKAGE_NAME "opentxs-cli")
set(CPACK_PACKAGE_CONTACT ${PACKAGE_CONTACT})
set(CPACK_PACKAGE_VENDOR ${PACKAGE_VENDOR})
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}.${CMAKE_SYSTEM_PROCESSOR}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Open Transactions CLI")
set(CPACK_PACKAGE_DESCRIPTION "
Open-Transactions democratizes financial and monetary actions.
You can use it for issuing currencies/stock, paying dividends, creating asset accounts, sending/receiving digital cash, writing/depositing cheques, cashier's cheques, creating basket currencies, trading on markets, scripting custom agreements, recurring payments, escrow, etc.
")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(OPEN_TRANSACTIONS_WEBSITE "http://opentransactions.org")

if(RPM)
  set(CPACK_GENERATOR "RPM")
  set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
  set(CPACK_RPM_CHANGELOG_FILE "${CMAKE_CURRENT_SOURCE_DIR}/ChangeLog")
endif(RPM)

if(DEB)
  set(CPACK_GENERATOR "DEB")
  set(CPACK_DEBIAN_PACKAGE_DEPENDS "opentxs-main")
  set(CPACK_DEBIAN_PACKAGE_PRIORITY "extra")
  set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
  set(CPACK_DEBIAN_PACKAGE_HOMEPAGE ${OPEN_TRANSACTIONS_WEBSITE})
  set(CPACK_DEBIAN_PACKAGE_MAINTAINER ${PACKAGE_CONTACT})
endif(DEB)

include(CPack)
