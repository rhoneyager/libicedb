include(addapp)
include(addlib)
include(generaterc)

option (BUILD_PLUGIN_SILO "Build the Silo plugin for visualizing shapes" OFF)

if (BUILD_PLUGIN_SILO)
	find_package(silo REQUIRED)
	generaterc("icedb_plugin_silo" "dll"
		"icedb"
		"Visualize shapes with the SILO file format"
		 "${CMAKE_CURRENT_BINARY_DIR}/lib.rc"
		)
	set(DIRS_SILO
		WritePoints.cpp
		WritePoints.h
		plugin-silo.cpp
		plugin-silo.hpp
		silo-shapes.cpp
		)
	if (WIN32 AND NOT CYGWIN)
		SET(DIRS_SILO ${DIRS_SILO} 
		"${CMAKE_CURRENT_BINARY_DIR}/lib.rc")
	endif()
	#source_group("SILO" FILES ${DIRS_SILO})

	add_library(plugin_silo SHARED ${DIRS_SILO})
	target_include_directories(plugin_silo SYSTEM PRIVATE
		${CMAKE_SOURCE_DIR}/lib/related/eigen
		${silo_INCLUDE_DIRS}
		${CMAKE_SOURCE_DIR}/lib
		${CMAKE_SOURCE_DIR}/lib/icedb/compat
		)
	target_link_libraries(plugin_silo silo icedb ${Boost_PROGRAM_OPTIONS_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})
	addlib(plugin_silo SHARED)
	addplugin(plugin_silo IO icedb)
endif()
