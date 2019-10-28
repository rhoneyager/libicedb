macro(addBoostUniform )

# Check for Anaconda
# If we are in a conda environment, ensure that the right Boost headers are being loaded
if (DEFINED ENV{CONDA_PREFIX})
	set(Boost_INCLUDE_DIR "$ENV{CONDA_PREFIX}/include" CACHE STRING "Include dir for boost")
	set(BOOST_INCLUDE_DIR "${Boost_INCLUDE_DIR}")
	set(BOOST_LIBRARYDIR "$ENV{CONDA_PREFIX}/lib" CACHE STRING "Library dir for boost")
	set(Boost_NO_BOOST_CMAKE ON)
endif()

# Bug in Boost 1.71.0 cmake build interacts with how HomeBrew installs both multithreaded
# and single-threaded Boost.
# See https://stackoverflow.com/questions/58081084/target-boostlibrary-already-has-an-imported-location-link-errors
if(APPLE)
	set(Boost_NO_BOOST_CMAKE ON)
endif()

set(Boost_FIND_QUIETLY ON)
set(BOOST_STATIC_LINK OFF)
set(Boost_USE_STATIC_RUNTIME OFF)
set(Boost_USE_STATIC_LIBS OFF)
add_definitions(-DBOOST_ALL_DYN_LINK)
add_definitions(-DBOOST_LOG_DYN_LINK)
#set(Boost_DEBUG ON)

#message("argv ${ARGV} ${args}")
#if (NOT WINBOOST_AUTOLINK)
#    add_definitions(-DBOOST_ALL_NO_LIB)
#    add_definitions(-DBOOST_LOG_DYN_LINK)
#else()
#    if (NOT BOOST_STATIC_LINK)
#        add_definitions(-DBOOST_ALL_DYN_LINK)
#        add_definitions(-DBOOST_LOG_DYN_LINK)
#    endif()
#endif()

find_package(Boost ${ARGV} ) #REQUIRED)

#include_directories(BEFORE SYSTEM ${Boost_INCLUDE_DIR})
#message("boost ${Boost_LIBRARY_DIR}")
#link_directories(${Boost_LIBRARY_DIR})
#link_directories(${Boost_LIBRARY_DIR_DEBUG})
#link_directories(${Boost_LIBRARY_DIR_RELEASE})



# Also install / pack the relevant boost libraries into the install tree
# Do extention swapping
#if(WIN32)
#   set(dllext ".dll")
#   set(libext "[.]lib")
#else()
#   set(dllext ".so")
#   set(libext "[.]a")
#endif()


endmacro(addBoostUniform args)


macro(amend_boost_libs libname resname)
	get_target_property(lib_debug ${libname} IMPORTED_LOCATION_DEBUG)
	get_target_property(lib_release ${libname} IMPORTED_LOCATION_RELEASE)
	
	string(REPLACE "\.lib" ".dll" dll_debug ${lib_debug} )
	string(REPLACE "\.lib" ".dll" dll_release ${lib_release} )

	add_library(${resname} UNKNOWN IMPORTED)
	set_property(TARGET ${resname} APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
	set_target_properties(${resname} PROPERTIES IMPORTED_LOCATION_RELEASE "${dll_release}")
	set_property(TARGET ${resname} APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
	set_target_properties(${resname} PROPERTIES IMPORTED_LOCATION_DEBUG "${dll_debug}")
endmacro(amend_boost_libs libname resname)

macro(amend_basic_libs libname resname)
	get_target_property(lib_debug ${libname} IMPORTED_LOCATION_DEBUG)
	get_target_property(lib_release ${libname} IMPORTED_LOCATION_RELEASE)
	if (WIN32 AND NOT CYGWIN)
		string(REPLACE "\.lib" ".dll" dll_debug_a ${lib_debug} )
		string(REPLACE "\.lib" ".dll" dll_release_a ${lib_release} )
		string(REPLACE "/lib/" "/bin/" dll_debug ${dll_debug_a} )
		string(REPLACE "/lib/" "/bin/" dll_release ${dll_release_a} )
	else()
		set(dll_debug ${lib_debug})
		set(dll_release ${lib_release})
	endif()

	add_library(${resname} UNKNOWN IMPORTED)
	set_property(TARGET ${resname} APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
	set_target_properties(${resname} PROPERTIES IMPORTED_LOCATION_RELEASE "${dll_release}")
	set_property(TARGET ${resname} APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
	set_target_properties(${resname} PROPERTIES IMPORTED_LOCATION_DEBUG "${dll_debug}")
	set_property(TARGET ${resname} APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
	set_target_properties(${resname} PROPERTIES IMPORTED_LOCATION_RELWITHDEBINFO "${dll_debug}")
	set_property(TARGET ${resname} APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
	set_target_properties(${resname} PROPERTIES IMPORTED_LOCATION_MINSIZEREL "${dll_debug}")
	#message("${dll_debug} ${dll_release}")
endmacro(amend_basic_libs libname resname)
