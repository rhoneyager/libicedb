cmake_minimum_required(VERSION 3.1)
set( CMAKE_DIRECTORY_LABELS "HH" )
set(HH_MAJOR 0)
set(HH_MINOR 7)
set(HH_REVISION 1)
set(HH_VERSION ${HH_MAJOR}.${HH_MINOR}.${HH_REVISION})
project(HH
	VERSION ${HH_VERSION}
	LANGUAGES C CXX)
include(ExternalProject)
set(HH_STANDALONE_PROJECT OFF)
if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
	set(HH_STANDALONE_PROJECT ON)
	list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../cmake")
	set(CMAKE_CXX_STANDARD 14)
	set(CMAKE_CXX_STANDARD_REQUIRED YES)
	set(CMAKE_CXX_EXTENSIONS OFF)
	set_property(GLOBAL PROPERTY USE_FOLDERS ON)
	if(NOT CMAKE_BUILD_TYPE)
		set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
		set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Release Debug RelWithDebInfo MinSizeRel)
	endif()
	include(GetDependencies)
	#add_subdirectory(../
	add_subdirectory(../gsl gsl)
else()
endif()

option(HH_TEST "Generate HH tests" ON)

include(addapp)
set(DIRS_HH
	include/HH/Attributes.hpp
	include/HH/Datasets.hpp
	include/HH/defs.hpp
	include/HH/Errors.hpp
	include/HH/Files.hpp
	include/HH/Funcs.hpp
	include/HH/Groups.hpp
	include/HH/Handles.hpp
	include/HH/Handles_HDF.hpp
	include/HH/Handles_Valid.hpp
	include/HH/HH.hpp
	include/HH/PropertyLists.hpp
	include/HH/Tags.hpp
	include/HH/Types.hpp
	include/HH/Types/Complex.hpp
	src/_hh_build.cpp
	src/Attributes.cpp
	src/Datasets.cpp
	src/Files.cpp
	src/Funcs.cpp
	src/Groups.cpp
	src/Handles.cpp
	src/Types.cpp
	)
add_library(HH SHARED ${DIRS_HH})
add_library(
    HH::HH ALIAS HH )
add_library(
    icedb::HH ALIAS HH )

# add definitions to the library and targets that consume it
#target_compile_definitions(HH PUBLIC
#    $<$<CXX_COMPILER_ID:MSVC>:
        # remove unnecessary warnings about unchecked iterators
#        _SCL_SECURE_NO_WARNINGS
#		_CRT_SECURE_NO_WARNINGS
#    >
#	)
# add include folders to the library and targets that consume it
# the SYSTEM keyword suppresses warnings for users of the library
    target_include_directories(HH SYSTEM PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${INSTALL_INCLUDE_DIR}>	
    )
#if(DEFINED HDF5_C_DEFINITIONS) # Buggy because CMake want to prepend -D.
#	set_target_properties(HH PROPERTIES COMPILE_DEFINITIONS "${HDF5_C_DEFINITIONS}")
#endif()
if(NOT "${HDF5_C_INCLUDE_DIRS}" STREQUAL "")
	target_include_directories(HH SYSTEM PUBLIC "${HDF5_C_INCLUDE_DIRS}")
endif()


#if(DEFINED HDF5_C_LIBRARIES)
#	target_link_libraries(HH PUBLIC ${HDF5_C_LIBRARIES})
#endif()
#if(DEFINED HDF5_HL_LIBRARIES)
#	target_link_libraries(HH PUBLIC ${HDF5_HL_LIBRARIES})
#endif()

target_link_libraries(HH PUBLIC hdf5::hdf5-shared hdf5::hdf5_hl-shared GSL)
addlib(HH SHARED HH)
#addlib(HH STATIC)
#INSTALL(DIRECTORY include/HH
#	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/MiRS-${MiRS_VERSION}
#	COMPONENT Headers)
set_target_properties(HH PROPERTIES FOLDER "Libs")

if(HH_TEST)
	add_subdirectory(test)
endif()

INSTALL(DIRECTORY include/HH
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/icedb-${icedb_VERSION}
	COMPONENT Headers)
