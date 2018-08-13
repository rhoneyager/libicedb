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

	#target_link_libraries(io_psu icedb Boost::date_time Boost::regex)
	target_include_directories(io_psu SYSTEM PUBLIC ${Boost_INCLUDE_DIRS})
	target_include_directories(io_psu SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/lib/related/eigen)
	target_include_directories(io_psu SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/lib)
	target_include_directories(io_psu SYSTEM
		PRIVATE ${HDF5_INCLUDE_DIRS} ${HDF5_INCLUDE_DIR_CPP} ${CMAKE_SOURCE_DIR}/lib/icedb/compat)

endif()