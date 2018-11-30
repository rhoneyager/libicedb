include(addapp)
include(addlib)
include(generaterc)

option (BUILD_PLUGIN_QHULL "Build the qhull plugin" OFF)

if (BUILD_PLUGIN_QHULL)
	include(addQhull) # Finds qhull in an OS-independent manner...
	
	generaterc("icedb_plugin_qhull" "dll"
		"icedb"
		"qhull bindings"
		 "${CMAKE_CURRENT_BINARY_DIR}/lib.rc"
		)
	set(DIRS_QHULL
		plugin-qhull.cpp
		plugin-qhull.hpp
		)
	if (WIN32 AND NOT CYGWIN)
		SET(DIRS_QHULL ${DIRS_QHULL} 
		"${CMAKE_CURRENT_BINARY_DIR}/lib.rc")
	endif()
	add_library(plugin_qhull SHARED ${DIRS_QHULL})
	target_link_libraries(plugin_qhull icedb)
	addplugin(plugin_qhull Algorithms icedb)

	#if (WIN32 AND NOT CMAKE)
	#	add_custom_target(copy_silo_dependencies_to_build_dirs
	#		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:silo> $<TARGET_FILE_DIR:icedb_base>
	#		)
	#	set_target_properties( copy_silo_dependencies_to_build_dirs PROPERTIES FOLDER "Build")
	#	add_dependencies(copy_icedb_dependencies_to_build_dirs copy_silo_dependencies_to_build_dirs)
	#
	#	install(PROGRAMS $<TARGET_FILE:silo>
	#		DESTINATION ${CMAKE_INSTALL_BINDIR} #${RBIN_DIR}/$<CONFIG>
	#		COMPONENT Plugins)
	#	# There is a bug in the NSIS generator that causes it to miss dlls.
	## Fix is here:
	##INSTALL(TARGETS icedb
	##	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	##	COMPONENT Libraries)
	#endif()
endif()
