include(addlib)
set(DIRS_HH
	include/HH/Attributes.hpp
	include/HH/Datasets.hpp
	include/HH/defs.hpp
	include/HH/Files.hpp
	include/HH/Groups.hpp
	include/HH/Handles.hpp
	include/HH/Handles_HDF.hpp
	include/HH/Handles_Valid.hpp
	include/HH/HH.hpp
	include/HH/PropertyLists.hpp
	include/HH/Tags.hpp
	include/HH/Types.hpp
	include/HH/Types/Complex.hpp
	_hh_build.cpp
	)
add_library(HH STATIC ${DIRS_HH})
add_library(
    HH::HH ALIAS HH )
add_library(
    icedb::HH ALIAS HH )

# add definitions to the library and targets that consume it
target_compile_definitions(HH PUBLIC
    $<$<CXX_COMPILER_ID:MSVC>:
        # remove unnecessary warnings about unchecked iterators
        _SCL_SECURE_NO_WARNINGS
		_CRT_SECURE_NO_WARNINGS
        # remove deprecation warnings about std::uncaught_exception() (from catch)
        _SILENCE_CXX17_UNCAUGHT_EXCEPTION_DEPRECATION_WARNING
    >
	)
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
	target_include_directories(HH SYSTEM INTERFACE "${HDF5_C_INCLUDE_DIRS}")
endif()
if(DEFINED HDF5_C_LIBRARIES)
	target_link_libraries(HH INTERFACE ${HDF5_C_LIBRARIES})
endif()
if(DEFINED HDF5_HL_LIBRARIES)
	target_link_libraries(HH INTERFACE ${HDF5_HL_LIBRARIES})
endif()

target_link_libraries(HH INTERFACE hdf5::hdf5-shared hdf5::hdf5_hl-shared)
addlib(HH STATIC)
INSTALL(DIRECTORY include/HH
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/icedb-${icedb_VERSION}
	COMPONENT Headers)
