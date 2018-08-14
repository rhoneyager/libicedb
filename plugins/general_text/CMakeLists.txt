include(addlib)
option (BUILD_PLUGIN_GENERAL_TEXT_IO "Build the plugin for importing text-based data" ON)

if (BUILD_PLUGIN_GENERAL_TEXT_IO)
	set(f
		plugin-general.hpp
		plugin-general.cpp
		shape.hpp
		shapeIOtext.cpp
		shapeIOtext.hpp
		shapeIOtextParsers2.cpp
		)

	add_library(io_general SHARED ${f})
	addplugin(io_general IO icedb)

	target_include_directories(io_general SYSTEM PRIVATE)

endif()