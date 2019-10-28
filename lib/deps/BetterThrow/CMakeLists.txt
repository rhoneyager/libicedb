cmake_minimum_required (VERSION 3.1)
# This project needs at least CMake 3.1. If your system
# does not provide this version, you can get it from
# https://cmake.org/download/
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
#cmake_policy(SET CMP0076 NEW)

set(BT_MAJOR 0)
set(BT_MINOR 9)
set(BT_REVISION 1)
set(BETTERTHROW_VERSION ${BT_MAJOR}.${BT_MINOR}.${BT_REVISION})

project(Betterthrow
	VERSION ${BETTERTHROW_VERSION}
	LANGUAGES C CXX)

include(ExternalProject)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
set(BT_STANDALONE_PROJECT OFF)
if ( CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME )
    set( BT_STANDALONE_PROJECT ON )
else()
    set( BT_STANDALONE_PROJECT OFF )
endif()

# Global definitions
if(BT_STANDALONE_PROJECT)
	set(CMAKE_CXX_STANDARD 11)
	set(CMAKE_CXX_STANDARD_REQUIRED YES)
	set(CMAKE_CXX_EXTENSIONS OFF)
	set_property(GLOBAL PROPERTY USE_FOLDERS ON)
	if(NOT CMAKE_BUILD_TYPE)
		set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
		set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Release Debug RelWithDebInfo MinSizeRel)
	endif()
endif()

include(AddDocs)

# TODO: Get dependencies (Boost).

option(BT_TEST "Generate BetterThrow tests." ON) #${BT_STANDALONE_PROJECT})
option(BT_TEST_WITH_BOOST "Test the Boost bindings in the code." ON)

add_subdirectory(src)
if (BT_TEST)
	add_subdirectory(test)
	add_subdirectory(Examples)
endif ()
