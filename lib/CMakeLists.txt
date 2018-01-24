# The build routines for the core library go here.
include(addapp)
include(addlib)
include(generaterc)

# This is a tiny executable consumed by the build system.
# It reads the HDF5 version information and writes a header
# that describes certain unstable aspects of the HDF5 C++
# interface.
add_executable(h5tests related/h5tests/h5tests.cpp)
set_target_properties(h5tests PROPERTIES CXX_STANDARD 11)
target_include_directories(h5tests SYSTEM
	PRIVATE ${HDF5_INCLUDE_DIRS} ${HDF5_INCLUDE_DIR_CPP}
	)
addapp(h5tests Build_System_Internal)
# storebin(h5tests) # This temporary binary never gets installed.

# icedb_h5.h is system-specific and defines parameters only used
# by the core library. It is private.
add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/icedb_h5.h
	COMMAND h5tests
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	COMMENT "Generate HDF5 definitions for function matching"
	)

# These are the files required by the library, placed into groups.
set(related_cmake_objects
	../CMakeRules/addapp.cmake
	../CMakeRules/addBaseProject.cmake
	../CMakeRules/addBoostUniform.cmake
	../CMakeRules/adddocs.cmake
	../CMakeRules/addlib.cmake
	../CMakeRules/FindEigen3.cmake
	../CMakeRules/generaterc.cmake
	../CMakeRules/GetGitRevisionDescription.cmake
	../CMakeRules/signing.cmake)

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
	icedb/fs_backend.hpp
	src/fs_backend.cpp
	icedb/fs.hpp)
set(hdf_objs
	private/hdf5_supplemental.hpp
	icedb/compat/hdf5_load.h
	src/export-hdf5.cpp)
set(lib_objs
	icedb/Data_Types.hpp
	icedb/defs.h
	icedb/util.hpp
	private/resource.h
	src/util.cpp
	)

# On Windows, when producing a DLL, this generated file
# will add some information that can be read by Windows
# Explorer. Version info, basically.
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
source_group("CMake" FILES ${related_cmake_objects})
set(libsrcs ${lib_objs} ${gsl_objs} ${shp_objs} ${grp_objs}
	${att_objs} ${tbl_objs} ${db_objs} ${hdf_objs}
	${related_cmake_objects}
	${CMAKE_BINARY_DIR}/icedb_h5.h
    "${CMAKE_BINARY_DIR}/cmake-settings.h")
IF (WIN32 AND NOT CYGWIN)
SET(libsrcs ${libsrcs} 
	"${CMAKE_CURRENT_BINARY_DIR}/lib.rc")
source_group("Lib" FILES "${CMAKE_CURRENT_BINARY_DIR}/lib.rc")
ENDIF()

add_library(icedb STATIC ${libsrcs})
# related_includes contains the GSL.
target_include_directories(icedb SYSTEM
	PRIVATE ${HDF5_INCLUDE_DIRS} ${HDF5_INCLUDE_DIR_CPP}
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/related_includes
	)

# HDF5 is only exposed to the core library.
target_compile_definitions(icedb
	PRIVATE ${HDF5_DEFINITIONS})
target_link_libraries(icedb ${HDF5_LIBRARIES} ${HDF5_HL_LIBRARIES})

if("${CMAKE_HOST_SYSTEM_NAME}" MATCHES "Linux")
	IF(DEFINED CMAKE_COMPILER_IS_GNUCXX)
		target_link_libraries(icedb stdc++fs)
	endif()
endif()
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
	target_link_libraries(icedb stdc++fs)
endif()


addlib(icedb STATIC)
storebin(icedb)


