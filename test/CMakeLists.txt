include(addapp)
include(addlib)
set( CMAKE_DIRECTORY_LABELS "test" )
if (BUILD_TESTING)

	configure_file (
		"${CMAKE_CURRENT_SOURCE_DIR}/testing-settings.h.in"
		"${CMAKE_CURRENT_BINARY_DIR}/Testing/IntegratedTesting/testing-settings.h"
		)

	add_library(icedb_testing STATIC IntegratedTesting.hpp IntegratedTesting.cpp
		"${CMAKE_CURRENT_BINARY_DIR}/Testing/IntegratedTesting/testing-settings.h")
	addlib(icedb_testing STATIC)
	target_include_directories(icedb_testing PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/Testing>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
	target_link_libraries(icedb_testing icedb)

	# CTest stuff. May switch from CTest in the future.
	set(BUILDNAME "Unspecified_Build_Name_for_CTest" CACHE STRING "Build name variable for CTest")
	set(SITE "Unknown_Site_Name_for_CTest" CACHE STRING "Site name")
	mark_as_advanced(BUILDNAME SITE)

	file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/test)

	# The tests here are workflow tests. They exist to ensure that the 
	# user-callable programs will run without crashing in common workflows. The same programs may
	# be run multiple times, with different options.

	# A few tests need for NetCDF's programs to exist. We care about ncdump, as it can tell
	# us whether we have generated valid NetCDF4 files.
	find_package(netCDF QUIET)
	mark_as_advanced(netCDF_DIR)
	# The target name is netcdf. The INTERFACE_INCLUDE_DIRECTORIES target variable points
	# to the include directory. Go up to bin/ncdump.exe to get the executable that we want.
	# Note: This test, for now, only will run on Windows.
	if (WIN32 AND NOT CYGWIN)
		if (netCDF_DIR)
			get_target_property(nc_inc_dirs netcdf INTERFACE_INCLUDE_DIRECTORIES)
			find_program(ncdump name ncdump PATHS ${INTERFACE_INCLUDE_DIRECTORIES} 
				PATH_SUFFIXES ../bin
				DOC "Path to ncdump. Used in testing.")
			mark_as_advanced(ncdump)
			if (ncdump)
				set(DO_NCDUMP_TESTS 1)
			else()
				set(DO_NCDUMP_TESTS 0)
			endif()
		endif()
	else()
		set(DO_NCDUMP_TESTS 0)
	endif()
	add_subdirectory(io)
	add_subdirectory(Algorithms)

	if (BUILD_APPS)
		# Run a program without loading any plugins
		add_test(NAME plugin-test-load-none
			COMMAND icedb-version --dll-no-default-locations WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
	endif()
endif()
