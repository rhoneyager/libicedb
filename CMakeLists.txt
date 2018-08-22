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
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES) # Force compiler switches
set(CMAKE_CXX_EXTENSIONS OFF) # No compiler-specific extensions
SET(CMAKE_POSITION_INDEPENDENT_CODE ON) # Building dynamic libraries
# Convenience functions
include(addBaseProject)
include(addBoostUniform)
include(adddocs)
include(addapp)
include(addlib)
include(addInstallDirs)
include(generaterc)
include(GetGitRevisionDescription)
include(adddocs)

ENABLE_TESTING()
INCLUDE(CTest)

# Versioning information
get_git_head_revision(GITREFSPECVAR GITHASHVAR)
set(MAJOR 0)
set(MINOR 7)
set(REVISION 0)
set(icedb_VERSION ${MAJOR}.${MINOR}.${REVISION})
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
addBaseProject()
addInstallDirs(icedb)

# This config file stores basic information about the build.
# Version number, git hash, ...
# This gets pulled into the icedb target.
configure_file (
    "${CMAKE_SOURCE_DIR}/lib/cmake-settings.h.in"
    "${CMAKE_BINARY_DIR}/cmake-settings.h"
    )

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

# The two dependencies: boost and HDF5
addBoostUniform(1.45.0 REQUIRED COMPONENTS program_options unit_test_framework date_time regex filesystem system)
amend_boost_libs(Boost::filesystem impBoost::filesystem)
amend_boost_libs(Boost::system impBoost::system)
amend_boost_libs(Boost::program_options impBoost::program_options)
amend_boost_libs(Boost::date_time impBoost::date_time)
amend_boost_libs(Boost::regex impBoost::regex)
amend_boost_libs(Boost::unit_test_framework impBoost::unit_test_framework)
find_package(ZLIB REQUIRED)
amend_basic_libs(ZLIB::ZLIB impZLIB::ZLIB)

#if ("${Boost_MINOR_VERSION}" LESS "45")
#	error("Your version of Boost is too old to build icedb.")
#endif()
option(USE_OLD_CPP03_ABI "C++11 led to changes in the GLIB C++ ABI. Turn this on if you are linking against old system libraries" OFF)

find_package(HDF5 COMPONENTS C HL REQUIRED )
# These get set: HDF5_C_INCLUDE_DIRS HDF5_C_LIBRARIES HDF5_C_HL_LIBRARIES HDF5_C_DEFINITIONS
# Unfortunately, the hdf5:: targets are not always available. Ugh.
if (NOT TARGET hdf5::hdf5-shared)
	add_library(hdf5::hdf5-shared UNKNOWN IMPORTED)
	set_property(TARGET hdf5::hdf5-shared APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
	#message("${HDF5_C_LIBRARIES}")
	list(GET HDF5_C_LIBRARIES 0 HDF5_MAIN_C_LIB)
	list(REMOVE_AT HDF5_C_LIBRARIES 0)
	#message("${HDF5_MAIN_C_LIB}")
	#message("${HDF5_C_LIBRARIES}")
	set_target_properties(hdf5::hdf5-shared PROPERTIES IMPORTED_LOCATION ${HDF5_MAIN_C_LIB})
	set_target_properties(hdf5::hdf5-shared PROPERTIES LINK_LIBRARIES "${HDF5_C_LIBRARIES}")
	set_target_properties(hdf5::hdf5-shared PROPERTIES COMPILE_DEFINITIONS ${HDF5_C_DEFINITIONS})
	set_target_properties(hdf5::hdf5-shared PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES ${HDF5_C_INCLUDE_DIRS})
	#target_include_directories(hdf5::hdf5-shared SYSTEM INTERFACE ${HDF5_C_INCLUDE_DIRS})
endif()
if (NOT TARGET hdf5::hdf5_hl-shared)
	add_library(hdf5::hdf5_hl-shared UNKNOWN IMPORTED)
	set_property(TARGET hdf5::hdf5_hl-shared APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
	#message("${HDF5_C_HL_LIBRARIES}")
	set_target_properties(hdf5::hdf5_hl-shared PROPERTIES IMPORTED_LOCATION ${HDF5_C_HL_LIBRARIES})
endif()

# These are rather polluting
mark_as_advanced(
	HDF5_C_LIBRARY_dl HDF5_C_LIBRARY_hdf5 HDF5_C_LIBRARY_hdf5_hl 
	HDF5_C_LIBRARY_m HDF5_C_LIBRARY_pthread HDF5_C_LIBRARY_sz HDF5_C_LIBRARY_z)

# CMake find_package script creator stuff

# The different components are in different subdirectories
add_subdirectory(lib)
add_subdirectory(plugins)
add_subdirectory(apps)
export(TARGETS icedb icedb_base
		FILE "${PROJECT_BINARY_DIR}/icedbTargets.cmake")
export(PACKAGE icedb)


# CMake find_package script creator stuff # TODO: Put this in a separate script
set(pkgname "icedb")
set(pkglibs icedb)
set(pkgversion ${icedb_VERSION})
# ... for the build tree
set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/lib")
foreach(LOOP_CONFIG Debug Release RelWithDebInfo MinSizeRel)
	configure_file(lib/environment-module-buildtree.in
		"${PROJECT_BINARY_DIR}/environment-modules/buildtree/icedb/${LOOP_CONFIG}" @ONLY)
endforeach()
configure_file(CMakeRules/Pkg_Config.cmake.in
	"${PROJECT_BINARY_DIR}/icedbConfig.cmake" @ONLY)
# ... and for the install tree
set(CONF_INCLUDE_DIRS "\${icedb_CMAKE_DIR}/${REL_INCLUDE_DIR}") # Before WiX
foreach(LOOP_CONFIG Debug Release RelWithDebInfo MinSizeRel)
	configure_file(lib/environment-module.in
		"${PROJECT_BINARY_DIR}/environment-modules/install/icedb/${configbase}/${LOOP_CONFIG}" @ONLY)
endforeach()
#INSTALL(DIRECTORY "${PROJECT_BINARY_DIR}/environment-modules/install/icedb"
#	DESTINATION ${INSTALL_CMAKE_DIR}/${REL_SHARE_DIR}/environment-modules
#	COMPONENT Errata
#	)
configure_file(CMakeRules/Pkg_Config.cmake.in
	"${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/icedbConfig.cmake" @ONLY)
# ... for both
configure_file(CMakeRules/Pkg_ConfigVersion.cmake.in
	"${PROJECT_BINARY_DIR}/icedbConfigVersion.cmake" @ONLY)

INSTALL(FILES "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/icedbConfig.cmake"
	"${PROJECT_BINARY_DIR}/icedbConfigVersion.cmake"
	DESTINATION ${INSTALL_CMAKE_DIR} COMPONENT Libraries)

INSTALL(EXPORT icedbTargets
	DESTINATION ${INSTALL_CMAKE_DIR} COMPONENT Libraries
	)

INSTALL(FILES LICENSE.txt README.md
	DESTINATION ${INSTALL_CMAKE_DIR}/${REL_DOC_DIR}
	)
INSTALL(DIRECTORY share/examples
	DESTINATION ${INSTALL_CMAKE_DIR}/${REL_SHARE_DIR}
	COMPONENT Examples
	)

# Generate doxygen documentation, if needed.
adddocs()

# CTest stuff. May switch from CTest in the future.
set(BUILDNAME "Unspecified_Build_Name_for_CTest" CACHE STRING "Build name variable for CTest")
set(SITE "Unknown_Site_Name_for_CTest" CACHE STRING "Site name")
mark_as_advanced(BUILDNAME SITE)
# A junk test, for now. I only care if the code builds.
#add_test (NAME get_id COMMAND uname -a)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/test)
add_test (NAME io-psu-gmm 
	COMMAND icedb-3d_structures_importer
		--from ${CMAKE_SOURCE_DIR}/share/examples/shapes/PSU-GMM/psuaydinetal_geometry_aggregate_00004_GMM.nc
		--truncate
		--from-format psu --to ${CMAKE_BINARY_DIR}/test/psu-gmm-a04.h5
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	)


# CPack stuff
set(CPACK_PACKAGE_NAME "icedb")
set(CPACK_PACKAGE_VENDOR "Ryan Honeyager")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "icedb")
set(CPACK_PACKAGE_VERSION "${icedb_VERSION}")
set(CPACK_PACKAGE_VERSION_MAJOR "${MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${REVISION}")
set(CPACK_DEBIAN_PACKAGE_RELEASE "1")
set(CPACK_RPM_PACKAGE_RELEASE "1")

SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Ryan Honeyager")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README-package.txt")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "icedb")
set(CPACK_PACKAGE_CONTACT "Ryan Honeyager (ryan@honeyager.info)")
set(CPACK_WIX_UPGRADE_GUID "A29C49DA-8C87-4B15-AC54-D163E5AC3C29")
set(CPACK_WIX_PRODUCT_GUID "07770F44-F916-4A21-9788-CA03F71355D7")
set(CPACK_WIX_PRODUCT_ICON "${PROJECT_SOURCE_DIR}/share/icons\\\\favicon.ico")
set(CPACK_WIX_CMAKE_PACKAGE_REGISTRY "icedb")
set(CPACK_WIX_PROPERTY_ARPURLINFOABOUT "https://rhoneyager.github.io/icedb")
if (NOT CPACK_SYSTEM_NAME)
	set(CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_PROCESSOR}")
	if (CPACK_SYSTEM_NAME STREQUAL "x86_64")
		set(CPACK_SYSTEM_NAME "amd64")
	endif ()
endif ()
IF(WIN32 AND NOT UNIX)
	set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}\\\\LICENSE.txt")
	SET(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_SYSTEM_NAME}-${CMAKE_BUILD_TYPE}")
	# There is a bug in NSIS that does not handle full unix paths properly. Make
	# sure there is at least one set of four (4) backslashes.
	SET(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/share/icons\\\\favicon-96x96.png")
	SET(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/share/icons\\\\favicon.ico")
	SET(CPACK_NSYS_MUI_UNIICON "${CMAKE_SOURCE_DIR}/share/icons\\\\favicon.ico")

#	SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\MyExecutable.exe")
	SET(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} icedb")
	SET(CPACK_NSIS_HELP_LINK "https:\\\\\\\\rhoneyager.github.io/libicedb/")
	SET(CPACK_NSIS_URL_INFO_ABOUT "https:\\\\\\\\rhoneyager.github.io/libicedb/")
	SET(CPACK_NSIS_CONTACT "ryan@honeyager.info")
	SET(CPACK_NSIS_MODIFY_PATH ON)
ELSE(WIN32 AND NOT UNIX)
	set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE.txt")
	SET(CPACK_STRIP_FILES FALSE)
	SET(CPACK_SOURCE_STRIP_FILES FALSE)
ENDIF(WIN32 AND NOT UNIX)
#SET(CPACK_PACKAGE_EXECUTABLES "MyExecutable" "My Executable")

set (CPACK_COMPONENTS_ALL 
	Applications
	Documentation
	Examples
	Libraries 
	Headers
)

set(CPACK_COMPONENT_APPLICATIONS_DESCRIPTION 
	"All of the apps")
set(CPACK_COMPONENT_DOCUMENTATION_DESCRIPTION 
	"Doxygen HTML docs")
set(CPACK_COMPONENT_EXAMPLES_DESCRIPTION 
	"Basic example files")
set(CPACK_COMPONENT_LIBRARIES_DESCRIPTION 
	"The compiled libraries")
set(CPACK_COMPONENT_HEADERS_DESCRIPTION 
	"Headers for code development")

set(CPACK_COMPONENT_HEADERS_DEPENDS Libraries)
set(CPACK_COMPONENT_EXAMPLES_DEPENDS Libraries Applications Headers)
set(CPACK_COMPONENT_APPLICATIONS_DEPENDS Libraries Headers)

set(CPACK_COMPONENT_LIBRARIES_REQUIRED 1)

set(CPACK_RPM_PACKAGE_REQUIRES 
	"cmake >= 3.1, hdf5-devel, hdf5, git, zlib-devel, boost-devel >= 1.48, gcc-c++ >= 6"
	)

set(CPACK_DEBIAN_PACKAGE_DEPENDS
	"cmake (>= 3.1), libhdf5-dev, zlib1g-dev, libboost-filesystem-dev (>= 1.48), libboost-system-dev (>= 1.48), libboost-program-options-dev (>=1.48)"
	)
set(CPACK_DEBIAN_PACKAGE_RECOMMENDS "g++ (>= 6), hdf5-tools, git, doxygen")
set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")

# This must always be last!
include(CPack)

