# The build routines for the core library go here.
include(addapp)
include(addlib)
include(generaterc)

add_subdirectory(deps)

# These are the files required by the library, placed into groups.
set(related_cmake_objects
	../cmake/addapp.cmake
	../cmake/addBaseProject.cmake
	../cmake/addBoostUniform.cmake
	../cmake/AddDocs.cmake
	../cmake/addlib.cmake
	../cmake/FindEigen3.cmake
	../cmake/generaterc.cmake
	../cmake/GetGitRevisionDescription.cmake
	../cmake/signing.cmake)
set(alg_objs
	icedb/Algorithms/Algorithms.hpp
	src/Algorithms/basic.cpp
	src/Algorithms/max_distance.cpp
	src/Algorithms/miniball.cpp
	src/Algorithms/quickhull.cpp
	icedb/Algorithms/ShapeAlgs.hpp
	src/Algorithms/ShapeAlgs.cpp
	icedb/Algorithms/depGraph.hpp
	src/Algorithms/depGraph.cpp
	)
set(shp_objs
	icedb/IO/PPP.hpp
	icedb/IO/Shapes.hpp
	src/IO/PPP.cpp
	src/IO/Shapes.cpp)
set(exv_objs
	icedb/IO/exv.hpp
	src/IO/exv.cpp)
set(db_objs
	icedb/IO/Common.hpp
	icedb/IO/fs_backend.hpp
	src/IO/Common.cpp
	src/IO/fs_backend.cpp
	icedb/IO/fs.hpp)
set(units_objs
	icedb/units/units.hpp
	icedb/units/unitsPlugins.hpp
	private/unitsBackend.hpp
	src/Units/units.cpp
	src/Units/unitsPlugins.cpp
	src/Units/unitsSimple.cpp
	)
set(refract_objs
	icedb/refract/refract.hpp
	icedb/refract/refractBase.hpp
	icedb/refract/refractBoost.hpp
	private/refractBackend.hpp
	src/Refracts/refract.cpp
	src/Refracts/refractImpl.cpp
	src/Refracts/refractStore.cpp
	)
set(versioning_objs
	icedb/versioning/versioning.hpp
	icedb/versioning/versioningForwards.h
	icedb/versioning/versioningForwards.hpp
	src/Utils/versioning.cpp)
set(error_objs
	icedb/Errors/error.h
	icedb/Errors/error.hpp
	icedb/Errors/errorCodes.h
	icedb/Errors/errorCodes.hpp
	icedb/Errors/error_context.h
	icedb/Errors/errorTypes.hpp
	src/Errors/error.cpp
	src/Errors/errorCodes.cpp
	src/Errors/errorTypes.cpp
	src/Errors/error_context.cpp
	)
set(extensible_objs
	icedb/IO/io.hpp
	icedb/Plugins/registry.hpp
	icedb/Plugins/plugin.hpp
	icedb/Utils/dlls.hpp
	src/Utils/registry.cpp
	src/Utils/dlls.cpp
	)
set(lib_objs
	icedb/defs.h
	icedb/icedb.h
	icedb/icedb.hpp
	icedb/misc/Constants.hpp
	icedb/Utils/optionsForwards.hpp
	icedb/Utils/util.hpp
	icedb/Utils/util.h
	icedb/Utils/logging.hpp
	icedb/Utils/splitSet.hpp
	icedb/misc/zeros.hpp
	icedb/misc/os_functions.h
	icedb/misc/os_functions.hpp
	private/linterp.h
	private/resource.h
	private/os_impl.hpp
	icedb/Utils/options.hpp
	src/Misc/Constants.cpp
	src/Utils/logging.cpp
	src/Utils/options.cpp
	src/Utils/os_functions.cpp
	src/Utils/splitSet.cpp
	src/Utils/util.cpp
	src/Misc/zeros.cpp
	)

# On Windows, when producing a DLL, this generated file
# will add some information that can be read by Windows
# Explorer. Version info, basically.
generaterc("icedb_base" "dll"
	"icedb"
	"Scattering utility library"
	 "${CMAKE_CURRENT_BINARY_DIR}/lib_base.rc"
	 )
generaterc("icedb" "dll"
	"icedb"
	"Scattering utility library"
	 "${CMAKE_CURRENT_BINARY_DIR}/lib.rc"
	 )
source_group("Extensibility" FILES ${extensible_objs})
source_group("Lib" FILES ${lib_objs})
source_group("Shapes" FILES ${shp_objs})
source_group("Shape Algorithms" FILES ${alg_objs})
source_group("Errors" FILES ${error_objs})
source_group("Extended Scattering" FILES ${exv_objs})
source_group("Databases" FILES ${db_objs})
source_group("Units" FILES ${units_objs})
source_group("Refract" FILES ${refract_objs})
source_group("Versioning" FILES ${versioning_objs})
source_group("CMake" FILES ${related_cmake_objects})
set(libhdrs_dllbase ${related_cmake_objects}
	"${CMAKE_CURRENT_BINARY_DIR}/icedb-cmake-settings.h")
set(libsrcs_dllbase src/Utils/dlls_placeholder.cpp)
set(libsrcs ${lib_objs} ${shp_objs} ${exv_objs} ${db_objs}
	${units_objs} ${refract_objs} ${versioning_objs} ${error_objs}
	${extensible_objs} ${alg_objs})
IF (WIN32 AND NOT CYGWIN)
	SET(libsrcs_dllbase ${libsrcs_dllbase} 
		"${CMAKE_CURRENT_BINARY_DIR}/lib_base.rc")
	source_group("Lib" FILES "${CMAKE_CURRENT_BINARY_DIR}/lib_base.rc")
	SET(libsrcs ${libsrcs} 
		"${CMAKE_CURRENT_BINARY_DIR}/lib.rc")
	source_group("Lib" FILES "${CMAKE_CURRENT_BINARY_DIR}/lib.rc")
ENDIF()

add_library(icedb_base STATIC ${libhdrs_dllbase} ${libsrcs_dllbase})
addlib(icedb_base STATIC)
storebin(icedb_base)

add_library(icedb SHARED ${libsrcs})
# compat contains the GSL and HH.
target_include_directories(icedb SYSTEM PUBLIC ${Boost_INCLUDE_DIRS})
target_include_directories(icedb_base SYSTEM PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(icedb_base PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/deps
	${CMAKE_CURRENT_SOURCE_DIR}/deps/third_party
	${CMAKE_CURRENT_BINARY_DIR}
	)
target_include_directories(icedb PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/deps/third_party
	${CMAKE_CURRENT_BINARY_DIR}
	)
target_include_directories(icedb SYSTEM
	INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/deps/third_party/eigen>
	$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${INSTALL_INCLUDE_DIR}>
	)

target_compile_definitions(icedb
	PUBLIC ${HDF5_DEFINITIONS})
# Boost warning acknowledgement. 
target_compile_definitions(icedb PRIVATE _SILENCE_FPOS_SEEKPOS_DEPRECATION_WARNING=1)
target_compile_definitions(icedb PUBLIC __STDC_WANT_LIB_EXT1__=1)
target_compile_definitions(icedb PUBLIC __STDC_WANT_SECURE_LIB__=1)
target_link_libraries(icedb icedb_base 
	icedb::GSL icedb::HH icedb::BinaryIO icedb::Eigen3 
	BetterThrow::BetterThrow
	icedb::quickhull
	icedb::miniball
	hdf5::hdf5-shared hdf5::hdf5_hl-shared
	) # ZLIB::ZLIB)
target_link_libraries(icedb
	Boost::filesystem
	Boost::system
	Boost::program_options)

addlib(icedb SHARED)
storebin(icedb)

INSTALL(DIRECTORY icedb
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/icedb-${icedb_VERSION}
	COMPONENT Headers)

# Lib installation is already covered in addlib.cmake.
add_dependencies(icedb_base placeholder2)
