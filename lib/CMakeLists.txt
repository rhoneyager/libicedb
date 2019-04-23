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
set(shp_objs
	icedb/shape.hpp
	icedb/shapeAlg.hpp
	src/Shapes.cpp)
set(exv_objs
	icedb/exv.hpp
	src/exv.cpp)
set(db_objs
	icedb/fs_backend.hpp
	src/fs_backend.cpp
	icedb/fs.hpp)
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
set(extensible_objs
	icedb/io.hpp
	icedb/registry.hpp
	icedb/plugin.hpp
	icedb/dlls.hpp
	src/registry.cpp
	src/dlls.cpp
	)
set(lib_objs
	icedb/defs.h
	icedb/Constants.hpp
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
	private/os_impl.hpp
	icedb/options.hpp
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
source_group("Errors" FILES ${error_objs})
source_group("Extended Scattering" FILES ${exv_objs})
source_group("Databases" FILES ${db_objs})
source_group("Units" FILES ${units_objs})
source_group("Refract" FILES ${refract_objs})
source_group("Versioning" FILES ${versioning_objs})
source_group("CMake" FILES ${related_cmake_objects})
set(libhdrs_dllbase ${related_cmake_objects}
	"${CMAKE_CURRENT_BINARY_DIR}/icedb-cmake-settings.h")
set(libsrcs_dllbase src/dlls_placeholder.cpp)
set(libsrcs ${lib_objs} ${shp_objs} ${exv_objs} ${db_objs}
	${units_objs} ${refract_objs} ${versioning_objs} ${error_objs}
	${extensible_objs})
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
target_link_libraries(icedb icedb_base icedb::GSL icedb::HH icedb::BinaryIO icedb::Eigen3 ) # ZLIB::ZLIB)
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

# Code to collect the icedb dependencies into the build directory on Windows.
# Manually collecting these all the time is rather aggrevating.
if (WIN32 AND NOT CMAKE)
# all deps: hdf5: hdf5-shared, hdf5_hl-shared - handle here.
# boost: program_options unit_test_framework date_time regex filesystem system
#		--- not all in icedb core, but convenient to include for plugins.
#		--- on windows, just change the .lib to .dll.
# zlib
# silo: silo --- handle in the silo plugin
	add_executable(placeholder src/_placeholder.cpp)
	storebin(placeholder)
	set_target_properties(placeholder PROPERTIES FOLDER "Build")
	add_executable(placeholder2 src/_placeholder.cpp)
	storebin(placeholder2)
	set_target_properties(placeholder2 PROPERTIES FOLDER "Build")

	add_custom_target(copy_icedb_dependencies_to_build_dirs
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:hdf5::hdf5-shared> $<TARGET_FILE_DIR:placeholder>
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:hdf5::hdf5_hl-shared> $<TARGET_FILE_DIR:placeholder>
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:hdf5::hdf5_tools-shared> $<TARGET_FILE_DIR:placeholder>
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:impBoost::filesystem> $<TARGET_FILE_DIR:placeholder>
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:impBoost::system> $<TARGET_FILE_DIR:placeholder>
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:impBoost::program_options> $<TARGET_FILE_DIR:placeholder>
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:impBoost::date_time> $<TARGET_FILE_DIR:placeholder>
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:impBoost::regex> $<TARGET_FILE_DIR:placeholder>
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:impBoost::unit_test_framework> $<TARGET_FILE_DIR:placeholder>
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:impZLIB::ZLIB> $<TARGET_FILE_DIR:placeholder>
		)
	set_target_properties( copy_icedb_dependencies_to_build_dirs PROPERTIES FOLDER "Build")
	add_dependencies(icedb_base placeholder2)
	add_dependencies(placeholder2 copy_icedb_dependencies_to_build_dirs)
	# copy_icedb_dependencies_to_build_dirs has an automatic dependency on placeholder.
	# This way, the files get copied only once.

	install(PROGRAMS 
		$<TARGET_FILE:hdf5::hdf5-shared> $<TARGET_FILE:hdf5::hdf5_hl-shared> $<TARGET_FILE:hdf5::hdf5_tools-shared>
		$<TARGET_FILE:impBoost::filesystem> $<TARGET_FILE:impBoost::system> $<TARGET_FILE:impBoost::program_options>
		$<TARGET_FILE:impBoost::date_time> $<TARGET_FILE:impBoost::regex> $<TARGET_FILE:impBoost::unit_test_framework>
		$<TARGET_FILE:impZLIB::ZLIB>
		DESTINATION ${CMAKE_INSTALL_BINDIR} #${RBIN_DIR}/$<CONFIG>
		COMPONENT Libraries)

	# Boost targets do not fully have their imported_locations set. The shared libraries are not included.
	# Query IMPORTED_LOCATION_DEBUG, IMPORTED_LOCATION_RELEASE and IMPORTED_LOCATION.
	# Only copy shared libraries (i.e. ones that do not start with lib_____).
		#COMMAND ${CMAKE_COMMAND} -E copy
		#	$<TARGET_PROPERTY:boost::filesystem,IMPORTED_LOCATION_RELEASE>
		#	$<TARGET_FILE_DIR:icedb>
ENDIF()
