# The build routines for the core library go here.
include(addapp)
include(addlib)
include(generaterc)

# This is a tiny executable consumed by the build system.
# It reads the HDF5 version information and writes a header
# that describes certain unstable aspects of the HDF5 C++
# interface.
add_executable(icedb-h5tests related/h5tests/h5tests.cpp)
set_target_properties(icedb-h5tests PROPERTIES CXX_STANDARD 11)
target_include_directories(icedb-h5tests SYSTEM
	PRIVATE ${HDF5_INCLUDE_DIRS} ${HDF5_INCLUDE_DIR_CPP}
	)
addapp_base(icedb-h5tests Build_System_Internal)
# storebin(h5tests) # This temporary binary never gets installed.

# icedb_h5.h is system-specific and defines parameters only used
# by the core library. It is private.
add_custom_command(TARGET icedb-h5tests POST_BUILD
	#OUTPUT ${CMAKE_BINARY_DIR}/icedb_h5.h
	COMMAND icedb-h5tests
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
	icedb/compat/gsl/gsl
	icedb/compat/gsl/gsl_algorithm
	icedb/compat/gsl/gsl_assert
	icedb/compat/gsl/gsl_byte
	icedb/compat/gsl/gsl_util
	icedb/compat/gsl/multi_span
	icedb/compat/gsl/pointers
	icedb/compat/gsl/span
	icedb/compat/gsl/string_span
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
	private/hdf5_load.h
	src/export-hdf5.cpp)
set(units_objs
	icedb/units/units.hpp
	icedb/units/unitsPlugins.hpp
	private/unitsBackend.hpp
	src/units.cpp
	src/unitsPlugins.cpp
	src/unitsSimple.cpp
	)
set(refract_objs
	icedb/refract/refract.hpp
	icedb/refract/refractBase.hpp
	icedb/refract/refractBoost.hpp
	private/refractBackend.hpp
	src/refract.cpp
	src/refractImpl.cpp
	src/refractStore.cpp
	)
set(versioning_objs
	icedb/versioning/versioning.hpp
	icedb/versioning/versioningForwards.h
	icedb/versioning/versioningForwards.hpp
	src/versioning.cpp)
set(error_objs
	icedb/error.h
	icedb/error.hpp
	icedb/errorCodes.h
	icedb/errorCodes.hpp
	icedb/error_context.h
	icedb/errorTypes.hpp
	src/error.cpp
	src/errorCodes.cpp
	src/errorTypes.cpp
	src/error_context.cpp
)
set(lib_objs
	icedb/Data_Types.hpp
	icedb/defs.h
	icedb/optionsForwards.hpp
	icedb/util.hpp
	icedb/util.h
	icedb/logging.hpp
	icedb/splitSet.hpp
	icedb/zeros.hpp
	icedb/misc/os_functions.h
	icedb/misc/os_functions.hpp
	private/linterp.h
	private/resource.h
	private/options.hpp
	src/logging.cpp
	src/options.cpp
	src/os_functions.cpp
	src/splitSet.cpp
	src/util.cpp
	src/zeros.cpp
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
source_group("Errors" FILES ${error_objs})
source_group("Groups" FILES ${grp_objs})
source_group("Attributes" FILES ${att_objs})
source_group("Tables" FILES ${tbl_objs})
source_group("Databases" FILES ${db_objs})
source_group("HDF5" FILES ${hdf_objs})
source_group("Units" FILES ${units_objs})
source_group("Refract" FILES ${refract_objs})
source_group("Versioning" FILES ${versioning_objs})
source_group("CMake" FILES ${related_cmake_objects})
set(libsrcs ${lib_objs} ${gsl_objs} ${shp_objs} ${grp_objs}
	${att_objs} ${tbl_objs} ${db_objs} ${hdf_objs}
	${units_objs} ${refract_objs} ${versioning_objs} ${error_objs}
	${related_cmake_objects}
	#${CMAKE_BINARY_DIR}/icedb_h5.h # Auto-generated file, not listed in the build interface.
    "${CMAKE_BINARY_DIR}/cmake-settings.h")
IF (WIN32 AND NOT CYGWIN)
SET(libsrcs ${libsrcs} 
	"${CMAKE_CURRENT_BINARY_DIR}/lib.rc")
source_group("Lib" FILES "${CMAKE_CURRENT_BINARY_DIR}/lib.rc")
ENDIF()

add_library(icedb STATIC ${libsrcs})
add_dependencies(icedb icedb-h5tests) # Ensures that icedb_h5.h is generated before build.
# compat contains the GSL.
target_include_directories(icedb SYSTEM
	PRIVATE ${HDF5_INCLUDE_DIRS} ${HDF5_INCLUDE_DIR_CPP})
#target_include_directories(icedb SYSTEM
#	PUBLIC
#	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/icedb/compat>
#	$<INSTALL_INTERFACE:${INSTALL_CMAKE_DIR}/${REL_INCLUDE_DIR}>
#	$<INSTALL_INTERFACE:${INSTALL_CMAKE_DIR}/${REL_INCLUDE_DIR}/icedb/compat>
	#${CMAKE_CURRENT_SOURCE_DIR}/icedb/compat
	#	)

# HDF5 is only exposed to the core library.
target_compile_definitions(icedb
	PRIVATE ${HDF5_DEFINITIONS})
target_link_libraries(icedb ${HDF5_LIBRARIES} ${HDF5_HL_LIBRARIES})
# Let's prefer the boost filesystem library for now.
# It is more stable and more widely supported.
if(PROHIBIT_BUILD_USING_BOOST_FILESYSTEM OR ("${Boost_MINOR_VERSION}" LESS "45"))
	target_compile_definitions(icedb PUBLIC ICEDB_PROHIBIT_BOOST_FILESYSTEM)
endif()
if(Boost_FILESYSTEM_LIBRARY AND ("${Boost_MINOR_VERSION}" GREATER "44"))
	target_link_libraries(icedb
		${Boost_FILESYSTEM_LIBRARY}
		${Boost_SYSTEM_LIBRARY})
	target_compile_definitions(icedb PUBLIC ICEDB_HAS_COMPILED_BOOST_FILESYSTEM)
	target_include_directories(icedb SYSTEM PUBLIC ${Boost_INCLUDE_DIRS})
else()
	if("${CMAKE_HOST_SYSTEM_NAME}" MATCHES "Linux")
		IF(DEFINED CMAKE_COMPILER_IS_GNUCXX)
			target_link_libraries(icedb stdc++fs)
		endif()
	endif()
	if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
		target_link_libraries(icedb stdc++fs)
	endif()
endif()
if(USE_OLD_CPP03_ABI)
	target_compile_definitions(icedb PUBLIC _GLIBCXX_USE_CXX11_ABI=0)
endif()


addlib(icedb STATIC)
storebin(icedb)

INSTALL(DIRECTORY icedb
	DESTINATION ${INSTALL_CMAKE_DIR}/${REL_INCLUDE_DIR}
	COMPONENT Headers
	)

