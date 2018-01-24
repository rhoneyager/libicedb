cmake_minimum_required (VERSION 3.1)
# This project needs at least CMake 3.1. If your system
# does not provide this version, you can get it from
# https://cmake.org/download/

# This project requires a C++11 compiler.
project(icedb C CXX)

# Convenience functions
set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  "${CMAKE_SOURCE_DIR}/CMakeRules"
  "${CMAKE_CURRENT_SOURCE_DIR}"
)
# Global definitions
set(CMAKE_CXX_STANDARD 14) # TODO: Switch to 11?
set(CMAKE_CXX_STANDARD_REQUIRED YES) # Force compiler switches
set(CMAKE_CXX_EXTENSIONS OFF) # No compiler-specific extensions
SET(CMAKE_POSITION_INDEPENDENT_CODE ON) # Building dynamic libraries
# Convenience functions
include(addBaseProject)
include(addBoostUniform)
include(adddocs)
include(addapp)
include(addlib)
include(generaterc)
include(GetGitRevisionDescription)
include(adddocs)

ENABLE_TESTING()
INCLUDE(CTest)

# Versioning information
get_git_head_revision(GITREFSPECVAR GITHASHVAR)
set(MAJOR 0)
set(MINOR 4)
set(REVISION 2)
set(icedb_VERSION ${MAJOR}.${MINOR}.${REVISION})
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
addBaseProject()

# This config file stores basic information about the build.
# Version number, git hash, ...
configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/cmake-settings.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake-settings.h"
    )

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

# Add in script-generated includes
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# The two dependencies: boost and HDF5
addBoostUniform(program_options )
find_package(HDF5 REQUIRED COMPONENTS C CXX HL)
# These are rather polluting
mark_as_advanced(HDF5_CXX_LIBRARY_dl HDF5_CXX_LIBRARY_hdf5 HDF5_CXX_LIBRARY_hdf5_cpp
	HDF5_CXX_LIBRARY_hdf5_hl HDF5_CXX_LIBRARY_hdf5_hl_cpp HDF5_CXX_LIBRARY_m
	HDF5_CXX_LIBRARY_pthread HDF5_CXX_LIBRARY_sz HDF5_CXX_LIBRARY_z 
	HDF5_C_LIBRARY_dl HDF5_C_LIBRARY_hdf5 HDF5_C_LIBRARY_hdf5_hl 
	HDF5_C_LIBRARY_m HDF5_C_LIBRARY_pthread HDF5_C_LIBRARY_sz HDF5_C_LIBRARY_z)

# The different components are in different subdirectories
add_subdirectory(lib)
add_subdirectory(apps)

# Generate doxygen documentation, if needed.
adddocs()


# CTest stuff. May switch from CTest in the future.
set(BUILDNAME "Unspecified_Build_Name_for_CTest" CACHE STRING "Build name variable for CTest")
set(SITE "Unknown_Site_Name_for_CTest" CACHE STRING "Site name")
mark_as_advanced(BUILDNAME SITE)
# A junk test, for now. I only care if the code builds.
add_test ( get_id command uname -a)

# CPack stuff

