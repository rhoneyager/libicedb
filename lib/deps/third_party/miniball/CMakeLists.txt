include(addlib)
include(addapp)

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
		${miniball_srcs} README.md)
#if(UNIX)
#	target_link_libraries(qhull_r m)
#endif(UNIX)
addlib(miniball STATIC)
storebin(miniball)
target_include_directories(miniball PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/miniball/cpp/main>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
target_compile_definitions(miniball PUBLIC SEB_DEBUG_H=1)
add_library(
    icedb::miniball ALIAS miniball )
#list(APPEND ICEDB_TARGETS miniball)
# The plugin
#add_library(alg_hbf_miniball SHARED README.md plugin-miniball.hpp plugin-miniball.cpp)
#addplugin(alg_hbf_miniball Algorithms icedb)
#target_link_libraries(alg_hbf_miniball icedb miniball)
#target_compile_definitions(io_adda PRIVATE _SILENCE_FPOS_SEEKPOS_DEPRECATION_WARNING=1)


#if (BUILD_TESTING)
#	generaterc("icedb-test-plugin-miniball" "exe"
#		"icedb"
#		"Test hbf-miniball algorithm code"
#			"${CMAKE_CURRENT_BINARY_DIR}/app.rc"
#			)
#	set(srcs-test test-miniball.cpp)
#	IF (WIN32 AND NOT CYGWIN)
#		SET(srcs-test ${srcs-test} "${CMAKE_CURRENT_BINARY_DIR}/app.rc")
#	ENDIF()
#
#	add_executable(icedb-test-plugin-miniball ${srcs-test})
#	target_link_libraries(icedb-test-plugin-miniball icedb_testing icedb alg_hbf_miniball Boost::unit_test_framework)
#	addapp_test(icedb-test-plugin-miniball Testing/Plugins)
#	storebin(icedb-test-plugin-miniball)
#endif()
