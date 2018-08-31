cmake_minimum_required (VERSION 3.1)
# This project needs at least CMake 3.1. If your system
# does not provide this version, you can get it from
# https://cmake.org/download/
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
project(icedb C CXX)
# Global definitions
set(CMAKE_CXX_STANDARD 14) # This project requires a C++14 compiler.
set(CMAKE_CXX_STANDARD_REQUIRED YES) # Force compiler switches
set(CMAKE_CXX_EXTENSIONS OFF) # No compiler-specific extensions
SET(CMAKE_POSITION_INDEPENDENT_CODE ON) # Building dynamic libraries
# Convenience functions
include(GNUInstallDirs)
include(addBaseProject)
include(addBoostUniform)
include(adddocs)
include(addapp)
include(addlib)
include(addInstallDirs)
include(generaterc)
include(GetGitRevisionDescription)
include(adddocs)

set(MAJOR 0)
set(MINOR 7)
set(REVISION 3)
set(icedb_VERSION ${MAJOR}.${MINOR}.${REVISION})
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
addBaseProject()
addInstallDirs(icedb)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Release Debug RelWithDebInfo MinSizeRel)
endif()

ENABLE_TESTING()
INCLUDE(CTest)

# Fix for the libexec install directory location on Debian and Ubuntu - they do not use /usr/libexec.
# Placing under /usr/lib per the Filesystem Hierarchy Standard (http://www.pathname.com/fhs/).
# Note: Debian and Ubuntu use FHS 2.3, and Fedora uses 3.0 (which re-adds libexec).
#message("${CMAKE_INSTALL_LIBDIR} ${CMAKE_INSTALL_LIBEXECDIR} ${CMAKE_INSTALL_BINDIR}")
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	find_program(LSB_RELEASE_EXEC lsb_release)
	mark_as_advanced(LSB_RELEASE_EXEC)
	execute_process(COMMAND ${LSB_RELEASE_EXEC} -is
		OUTPUT_VARIABLE LSB_DISTRIBUTION_NAME_SHORT
		OUTPUT_STRIP_TRAILING_WHITESPACE
		)
	execute_process(COMMAND ${LSB_RELEASE_EXEC} -rs
		OUTPUT_VARIABLE LSB_RELEASE_ID_SHORT
		OUTPUT_STRIP_TRAILING_WHITESPACE
		)
	if(${LSB_DISTRIBUTION_NAME_SHORT} STREQUAL "Ubuntu" OR ${LSB_DISTRIBUTION_NAME_SHORT} STREQUAL "Debian")
		set(CMAKE_INSTALL_LIBEXECDIR ${CMAKE_INSTALL_LIBDIR})
		set(CMAKE_INSTALL_FULL_LIBEXECDIR ${CMAKE_INSTALL_FULL_LIBDIR})
	endif()
endif()
SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")

# Versioning information
get_git_head_revision(GITREFSPECVAR GITHASHVAR)
if ("${GITREFSPECVAR}" STREQUAL "")
	set(GITBRANCH "GIT_BRANCH_UNKNOWN")
else()
	string(SUBSTRING "${GITREFSPECVAR}" 11 -1 GITBRANCH )
endif()

# This config file stores basic information about the build.
# Version number, git hash, ...
# This gets pulled into the icedb target.
configure_file (
	"${CMAKE_CURRENT_SOURCE_DIR}/lib/util/cmake-settings.h.in"
	"${CMAKE_CURRENT_BINARY_DIR}/lib/icedb-cmake-settings.h"
    )



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

find_package(HDF5 REQUIRED COMPONENTS C HL )
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
	if(DEFINED HDF5_C_DEFINITIONS)
		set_target_properties(hdf5::hdf5-shared PROPERTIES COMPILE_DEFINITIONS "${HDF5_C_DEFINITIONS}")
	endif()
	if(NOT "${HDF5_C_INCLUDE_DIRS}" STREQUAL "")
		set_target_properties(hdf5::hdf5-shared PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES ${HDF5_C_INCLUDE_DIRS})
	endif()
	#target_include_directories(hdf5::hdf5-shared SYSTEM INTERFACE ${HDF5_C_INCLUDE_DIRS})
endif()
if (NOT TARGET hdf5::hdf5_hl-shared)
	add_library(hdf5::hdf5_hl-shared UNKNOWN IMPORTED)
	set_property(TARGET hdf5::hdf5_hl-shared APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
	list(GET HDF5_HL_LIBRARIES 0 HDF5_MAIN_HL_LIB)
	list(REMOVE_AT HDF5_HL_LIBRARIES 0)

	#message("${HDF5_C_HL_LIBRARIES} ... ${HDF5_HL_LIBRARIES}")
	# HDF5_hdf5_hl_LIBRARY_RELEASE
	if(NOT "${HDF5_MAIN_HL_LIB}" STREQUAL "")
		set_target_properties(hdf5::hdf5_hl-shared PROPERTIES IMPORTED_LOCATION ${HDF5_MAIN_HL_LIB})
		set_target_properties(hdf5::hdf5_hl-shared PROPERTIES LINK_LIBRARIES "${HDF5_HL_LIBRARIES}")

	endif()
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
configure_file(cmake/Pkg_Config.cmake.in
	"${PROJECT_BINARY_DIR}/icedbConfig.cmake" @ONLY)
# ... and for the install tree
set(CONF_INCLUDE_DIRS "\${icedb_CMAKE_DIR}/${REL_INCLUDE_DIR}") # Before WiX
configure_file(cmake/Pkg_Config.cmake.in
	"${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/icedbConfig.cmake" @ONLY)
# ... for both
configure_file(cmake/Pkg_ConfigVersion.cmake.in
	"${PROJECT_BINARY_DIR}/icedbConfigVersion.cmake" @ONLY)

INSTALL(FILES "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/icedbConfig.cmake"
	"${PROJECT_BINARY_DIR}/icedbConfigVersion.cmake"
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/icedb COMPONENT Libraries)

INSTALL(EXPORT icedbTargets
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/icedb COMPONENT Libraries)

INSTALL(FILES LICENSE.txt README.md
	DESTINATION ${CMAKE_INSTALL_DOCDIR}
	)
INSTALL(DIRECTORY share/examples
	DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icedb
	COMPONENT Examples
	)

# Generate doxygen documentation, if needed.
adddocs()
# Tests get built if BUILD_TESTING is on
add_subdirectory(test)

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
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/docs/README-package.txt")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "icedb")
set(CPACK_PACKAGE_CONTACT "Ryan Honeyager (ryan@honeyager.info)")
set(CPACK_WIX_UPGRADE_GUID "A29C49DA-8C87-4B15-AC54-D163E5AC3C29")
set(CPACK_WIX_PRODUCT_GUID "07770F44-F916-4A21-9788-CA03F71355D7")
set(CPACK_WIX_PRODUCT_ICON "${CMAKE_CURRENT_SOURCE_DIR}/share/icons\\\\favicon.ico")
set(CPACK_WIX_CMAKE_PACKAGE_REGISTRY "icedb")
set(CPACK_WIX_PROPERTY_ARPURLINFOABOUT "https://rhoneyager.github.io/icedb")
if (NOT CPACK_SYSTEM_NAME)
	set(CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_PROCESSOR}")
	if (CPACK_SYSTEM_NAME STREQUAL "x86_64")
		set(CPACK_SYSTEM_NAME "amd64")
	endif ()
endif ()
if (CPACK_SYSTEM_NAME STREQUAL "AMD64")
	set(CPACK_SYSTEM_NAME "amd64")
endif ()
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	set(CPACK_SYSTEM_NAME "${CPACK_SYSTEM_NAME}-${LSB_DISTRIBUTION_NAME_SHORT}-${LSB_RELEASE_ID_SHORT}")
endif()
option(PACKAGE_CI_BUILD "Is this a testing (continuous integration) build?" OFF)
set(PACKAGE_GIT_BRANCH "${GITBRANCH}" CACHE STRING "What is the branch of the code used to produce this build?")
string(TIMESTAMP PACKAGE_TIMESTAMP "%Y%m%d")
if(PACKAGE_CI_BUILD)
	SET(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_SYSTEM_NAME}-${PACKAGE_GIT_BRANCH}-${CMAKE_BUILD_TYPE}-${PACKAGE_TIMESTAMP}")
endif()
#else()
#	SET(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_SYSTEM_NAME}-${CMAKE_BUILD_TYPE}")
#endif()
IF(WIN32 AND NOT UNIX)
	set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}\\\\LICENSE.txt")
	
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
	"cmake (>= 3.1), libhdf5-dev, zlib1g-dev, libboost-filesystem-dev (>= 1.48), libboost-system-dev (>= 1.48), libboost-program-options-dev (>=1.48), libboost-date-time.dev (>= 1.48), libboost-regex-dev (>= 1.48), libboost-test-dev (>=1.48)"
	)
if(BUILD_PLUGIN_SILO)
	set(CPACK_DEBIAN_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS}, libsilo-dev")
endif()
set(CPACK_DEBIAN_PACKAGE_RECOMMENDS "g++ (>= 6), hdf5-tools, git, doxygen")
set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")

# This must always be last!
include(CPack)

