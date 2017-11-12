cmake_minimum_required (VERSION 3.1)
project(icedb C CXX)
set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  "${CMAKE_SOURCE_DIR}/CMakeRules"
  "${CMAKE_CURRENT_SOURCE_DIR}"
)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)
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
#include_directories(BEFORE SYSTEM ${Boost_INCLUDE_DIR})
#link_directories(${Boost_LIBRARY_DIR})
#link_directories(${Boost_LIBRARY_DIR_DEBUG})
#link_directories(${Boost_LIBRARY_DIR_RELEASE})

find_package(HDF5 REQUIRED COMPONENTS C CXX HL)
#include_directories(${HDF5_INCLUDE_DIRS} ${HDF5_INCLUDE_DIR_CPP})
#add_definitions(${HDF5_DEFINITIONS})
if(DEFINED HDF5_C_LIBRARIES)
	#set (h5libs ${HDF5_C_LIBRARIES} ${HDF5_CXX_LIBRARIES})
	set (h5libs ${HDF5_LIBRARIES})
	#message("${h5libs}")
endif()
#find_package(Eigen3 REQUIRED)

set(gsl_objs
	related_includes/gsl/gsl
	related_includes/gsl/gsl_algorithm
	related_includes/gsl/gsl_assert
	related_includes/gsl/gsl_byte
	related_includes/gsl/gsl_util
	related_includes/gsl/multi_span
	related_includes/gsl/pointers
	related_includes/gsl/span
	related_includes/gsl/string_span
	)
set(shp_objs
	icedb/shape.hpp
	icedb/shapeAlg.hpp
	private/Shape_impl.hpp
	src/Shapes.cpp)
set(grp_objs
	icedb/Group.hpp
	private/Group_impl.hpp
	src/Groups.cpp)
set(att_objs
	icedb/Attribute.hpp
	private/Attribute_impl.hpp
	src/Attributes.cpp)
set(tbl_objs
	icedb/Table.hpp
	private/Table_impl.hpp
	src/Tables.cpp)
set(db_objs
	icedb/Database.hpp
	private/Database_impl.hpp
	src/databases.cpp
	private/fs_backend.hpp
	src/fs_backend.cpp
	icedb/fs.hpp)
set(hdf_objs
	icedb/hdf5_supplemental.hpp
	icedb/compat/hdf5_load.h
	src/export-hdf5.cpp)
set(lib_objs
	icedb/Data_Types.hpp
	icedb/defs.h
	icedb/util.hpp
	private/resource.h
	src/util.cpp
	)

generaterc("libicedb" "dll"
	"libicedb"
	"Scattering utility library"
	 "${CMAKE_CURRENT_BINARY_DIR}/lib.rc"
	 )
source_group("GSL" FILES ${gsl_objs})
source_group("Lib" FILES ${lib_objs})
source_group("Shapes" FILES ${shp_objs})
source_group("Groups" FILES ${grp_objs})
source_group("Attributes" FILES ${att_objs})
source_group("Tables" FILES ${tbl_objs})
source_group("Databases" FILES ${db_objs})
source_group("HDF5" FILES ${hdf_objs})
set(libsrcs ${lib_objs} ${gsl_objs} ${shp_objs} ${grp_objs}
	${att_objs} ${tbl_objs} ${db_objs} ${hdf_objs})
IF (WIN32 AND NOT CYGWIN)
SET(libsrcs ${libsrcs} 
	"${CMAKE_CURRENT_BINARY_DIR}/lib.rc")
source_group("Lib" FILES "${CMAKE_CURRENT_BINARY_DIR}/lib.rc")
ENDIF()

add_library(libicedb STATIC ${libsrcs})
set_target_properties(libicedb PROPERTIES CXX_STANDARD 17)
# related_includes contains the GSL.
target_include_directories(libicedb SYSTEM
	PRIVATE ${HDF5_INCLUDE_DIRS} ${HDF5_INCLUDE_DIR_CPP}
	PUBLIC ${CMAKE_SOURCE_DIR}/related_includes
	)

target_compile_definitions(libicedb
	PRIVATE ${HDF5_DEFINITIONS})
target_link_libraries(libicedb ${HDF5_LIBRARIES} ${HDF5_HL_LIBRARIES})
addlib(libicedb STATIC)
storebin(libicedb)

add_subdirectory(apps)

adddocs()

set(BUILDNAME "Unspecified" CACHE STRING "Build name variable for CTest")
set(SITE "Unknown" CACHE STRING "Site name")
add_test ( get_id command uname -a)

