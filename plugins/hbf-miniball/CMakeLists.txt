# Implementing a separate build of qhull here, with a better build system
# Only the qhull C++ library is needed.

include(addlib)

option (BUILD_PLUGIN_HBF_MINIBALL "Build the hbf miniball plugin" ON)

if (BUILD_PLUGIN_HBF_MINIBALL)

	set(miniball_srcs
		miniball/cpp/main/Seb.h
		miniball/cpp/main/Seb_configure.h
		#miniball/cpp/main/Seb_debug.C
		miniball/cpp/main/Seb_debug.h
		miniball/cpp/main/Seb_point.h
		miniball/cpp/main/Seb-inl.h
		miniball/cpp/main/Subspan.h
		miniball/cpp/main/Subspan-inl.h
		my_Seb_debug.cpp
		my_Seb_debug.h
	)
	
	# ---------------------------------------
	# Define shared library for reentrant qhull
	# ---------------------------------------

	add_library(miniball STATIC 
			${miniball_srcs})
	#if(UNIX)
	#	target_link_libraries(qhull_r m)
	#endif(UNIX)
	addlib(miniball STATIC)
	target_include_directories(miniball PUBLIC 
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/miniball/cpp/main>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
	target_compile_definitions(miniball PUBLIC SEB_DEBUG_H=1)

	# The plugin
	add_library(alg-hbf-miniball SHARED README.md plugin-miniball.hpp plugin-miniball.cpp)
	addplugin(alg-hbf-miniball Algorithms icedb)
	target_link_libraries(alg-hbf-miniball icedb miniball)
	#target_compile_definitions(io_adda PRIVATE _SILENCE_FPOS_SEEKPOS_DEPRECATION_WARNING=1)
endif()
