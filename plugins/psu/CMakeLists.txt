include(addlib)
option (BUILD_PLUGIN_PSU "Build the plugin for importing Aydin et al. data" ON)

if (BUILD_PLUGIN_PSU)
	set(io_psu_FILES
		defs.hpp
		plugin-psu.hpp
		plugin-psu.cpp
		shapeIOpsu.cpp
		)

	add_library(io_psu SHARED ${io_psu_FILES})
	addlib(io_psu SHARED)
	storebin(io_psu)
	addplugin(io_psu IO icedb)

	target_link_libraries(io_psu icedb)
	#target_include_directories(io_psu SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/lib/related/eigen)

endif()