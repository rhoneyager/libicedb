option (BUILD_PLUGIN_ADDA "Build the plugin for importing ADDA data" ON)

if (BUILD_PLUGIN_ADDA)
	set(io_adda_FILES
		defs.hpp
		plugin-adda.hpp
		plugin-adda.cpp
		shape-adda.cpp
		)
	generaterc("io_adda" "dll"
		"icedb"
		"Read ADDA files"
		 "${CMAKE_CURRENT_BINARY_DIR}/lib.rc"
		)
	if (WIN32 AND NOT CYGWIN)
		SET(io_adda_FILES ${io_adda_FILES} 
		"${CMAKE_CURRENT_BINARY_DIR}/lib.rc")
	endif()
	add_library(io_adda SHARED ${io_adda_FILES})
	addplugin(io_adda IO icedb)

	target_link_libraries(io_adda icedb)
	target_compile_definitions(io_adda PRIVATE _SILENCE_FPOS_SEEKPOS_DEPRECATION_WARNING=1)
endif()