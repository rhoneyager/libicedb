cmake_minimum_required (VERSION 3.1)
project(icedb C CXX)
set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  "${CMAKE_SOURCE_DIR}/CMakeRules"
  "${CMAKE_CURRENT_SOURCE_DIR}"
)
set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)
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

get_git_head_revision(GITREFSPECVAR GITHASHVAR)
set(MAJOR 0)
set(MINOR 3)
set(REVISION 1)
set(icedb_VERSION ${MAJOR}.${MINOR}.${REVISION})
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
addBaseProject()

configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake-settings.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake-settings.h"
    )

# Add in script-generated includes
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

addBoostUniform(program_options )
#find_package(Eigen3 REQUIRED)

set(gsl_objs
	icedb/gsl/gsl
	icedb/gsl/gsl_algorithm
	icedb/gsl/gsl_assert
	icedb/gsl/gsl_byte
	icedb/gsl/gsl_util
	icedb/gsl/multi_span
	icedb/gsl/pointers
	icedb/gsl/span
	icedb/gsl/string_span
	)

set(lib_objs
	icedb/fs.hpp
	icedb/hdf5_supplemental.hpp
	icedb/shape.hpp
	icedb/shapeAlg.hpp
	private/fs_backend.hpp
	private/resource.h
	src/databases.cpp
	src/export-hdf5.cpp
	src/fs_backend.cpp
	)

generaterc("libicedb" "dll"
	"libicedb"
	"Scattering utility library"
	 "${CMAKE_CURRENT_BINARY_DIR}/lib.rc"
	 )
set(libsrcs ${lib_objs} ${gsl_objs})
source_group("GSL" FILES ${gsl_objs})
source_group("Lib" FILES ${lib_objs})
IF (WIN32 AND NOT CYGWIN)
SET(libsrcs ${libsrcs} 
	"${CMAKE_CURRENT_BINARY_DIR}/lib.rc")
ENDIF()

add_library(libicedb STATIC ${libsrcs})
addlib(libicedb STATIC)
storebin(libicedb)

add_subdirectory(apps)

adddocs()

set(BUILDNAME "Unspecified" CACHE STRING "Build name variable for CTest")
set(SITE "Unknown" CACHE STRING "Site name")
add_test ( get_id command uname -a)

