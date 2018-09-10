include(addlib)
option (BUILD_PLUGIN_EXAMPLE "Build the example plugin" OFF)

if (BUILD_PLUGIN_EXAMPLE)
	set(f
		defs.hpp
		plugin-example.hpp
		plugin-example.cpp
		shape-import-example.cpp
		#shape-export-example.cpp
		#scattering-data-import-example.cpp
		#scattering-data-export-example.cpp
		)

	add_library(plugin_example SHARED ${f})
	addplugin(plugin_example Examples icedb)
	target_link_libraries(plugin_example icedb)
endif()
