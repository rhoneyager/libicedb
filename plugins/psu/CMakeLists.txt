include(addlib)
option (BUILD_PLUGIN_PSU "Build the plugin for importing Aydin et al. data" ON)

if (BUILD_PLUGIN_PSU)
	set(io_psu_FILES
		defs.hpp
		exvIOpsu.cpp
		particle-ids.cpp
		plugin-psu.hpp
		plugin-psu.cpp
		shapeIOpsu.cpp
		)
	generaterc("io_psu" "dll"
		"icedb"
		"Read PSU files"
		 "${CMAKE_CURRENT_BINARY_DIR}/lib.rc"
		)
	if (WIN32 AND NOT CYGWIN)
		SET(io_psu_FILES ${io_psu_FILES} 
		"${CMAKE_CURRENT_BINARY_DIR}/lib.rc")
	endif()
	add_library(io_psu SHARED ${io_psu_FILES})
	addplugin(io_psu IO icedb)

	target_link_libraries(io_psu icedb)
	#target_include_directories(io_psu SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/lib/related/eigen)

endif()