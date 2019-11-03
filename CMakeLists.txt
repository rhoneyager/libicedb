cmake_minimum_required (VERSION 3.1)
# This project needs at least CMake 3.1. If your system
# does not provide this version, you can get it from
# https://cmake.org/download/
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
project(icedb C CXX)

# Versioning information
set(MAJOR 0)
set(MINOR 7)
set(REVISION 99)
set(icedb_VERSION ${MAJOR}.${MINOR}.${REVISION})

# Set install directory locations
include(GNUInstallDirs)
# Global build options
include(BuildOptions)
include(addBaseProject)
addBaseProject()

# Generate doxygen documentation, if needed.
include(AddDocs)
# Find the necessary packages.
include(GetDependencies)
# This handles extra parts of the "make" command, used to build the code.
include(Building)
# Tests of the code. Handles the "make test" command.
include(Testing)

# The different components are in different subdirectories
add_subdirectory(lib)
add_subdirectory(plugins)
add_subdirectory(apps)
add_subdirectory(test)

# This handles the "make install" command.
include(Installing)
# Allows for the creation of binary and source packages. These are complete, precompiled distributions of MiRS.
# Supports .tar.gz, .tar.bz2, .deb and .rpm packages.
include(Packaging)
