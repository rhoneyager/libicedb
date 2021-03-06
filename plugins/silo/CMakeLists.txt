include(addapp)
include(addlib)
include(generaterc)

option (BUILD_PLUGIN_SILO "Build the Silo plugin for visualizing shapes" OFF)

if (BUILD_PLUGIN_SILO)
	include(addSILO) # Finds SILO in an OS-independent manner...
	#find_package(silo REQUIRED)
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

	if(NOT EIGEN3_FOUND)
		message(SEND_ERROR "The DDSCAT plugin needs Eigen3, but it was not found. An easy way to get it is to run \"git submodule update --init\" in the root of the repository.")
	endif()
	add_library(plugin_silo SHARED ${DIRS_SILO})
	target_link_libraries(plugin_silo silo icedb Eigen3)
	target_include_directories(plugin_silo SYSTEM PRIVATE
		# provides silo_exports.h on platforms that are missing it.
		${CMAKE_CURRENT_SOURCE_DIR}/fixes 
		)
	target_compile_definitions(plugin_silo PRIVATE HH_HAS_EIGEN=1)
	addplugin(plugin_silo IO icedb)

	if (WIN32 AND NOT CMAKE)
		add_custom_target(copy_silo_dependencies_to_build_dirs
			COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:silo> $<TARGET_FILE_DIR:placeholder>
			)
		set_target_properties( copy_silo_dependencies_to_build_dirs PROPERTIES FOLDER "Build")
		add_dependencies(copy_dependencies_to_build_dirs copy_silo_dependencies_to_build_dirs)

		install(PROGRAMS $<TARGET_FILE:silo>
			DESTINATION ${CMAKE_INSTALL_BINDIR} #${RBIN_DIR}/$<CONFIG>
			COMPONENT Plugins)
		# There is a bug in the NSIS generator that causes it to miss dlls.
	# Fix is here:
	#INSTALL(TARGETS icedb
	#	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	#	COMPONENT Libraries)
	endif()
endif()
