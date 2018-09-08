
include(addBoostUniform)


# The two dependencies: boost and HDF5
addBoostUniform(1.45.0 REQUIRED COMPONENTS program_options unit_test_framework date_time regex filesystem system)
amend_boost_libs(Boost::filesystem impBoost::filesystem)
amend_boost_libs(Boost::system impBoost::system)
amend_boost_libs(Boost::program_options impBoost::program_options)
amend_boost_libs(Boost::date_time impBoost::date_time)
amend_boost_libs(Boost::regex impBoost::regex)
amend_boost_libs(Boost::unit_test_framework impBoost::unit_test_framework)
find_package(ZLIB REQUIRED)
amend_basic_libs(ZLIB::ZLIB impZLIB::ZLIB)

find_package(HDF5 REQUIRED COMPONENTS C HL )
# These get set: HDF5_C_INCLUDE_DIRS HDF5_C_LIBRARIES HDF5_C_HL_LIBRARIES HDF5_C_DEFINITIONS
# Unfortunately, the hdf5:: targets are not always available. Ugh.
if (NOT TARGET hdf5::hdf5-shared)
	add_library(hdf5::hdf5-shared UNKNOWN IMPORTED)
	set_property(TARGET hdf5::hdf5-shared APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
	#message("${HDF5_C_LIBRARIES}")
	list(GET HDF5_C_LIBRARIES 0 HDF5_MAIN_C_LIB)
	list(REMOVE_AT HDF5_C_LIBRARIES 0)
	#message("${HDF5_MAIN_C_LIB}")
	#message("${HDF5_C_LIBRARIES}")
	set_target_properties(hdf5::hdf5-shared PROPERTIES IMPORTED_LOCATION ${HDF5_MAIN_C_LIB})
	set_target_properties(hdf5::hdf5-shared PROPERTIES LINK_LIBRARIES "${HDF5_C_LIBRARIES}")
	if(DEFINED HDF5_C_DEFINITIONS)
		set_target_properties(hdf5::hdf5-shared PROPERTIES COMPILE_DEFINITIONS "${HDF5_C_DEFINITIONS}")
	endif()
	if(NOT "${HDF5_C_INCLUDE_DIRS}" STREQUAL "")
		set_target_properties(hdf5::hdf5-shared PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES ${HDF5_C_INCLUDE_DIRS})
	endif()
	#target_include_directories(hdf5::hdf5-shared SYSTEM INTERFACE ${HDF5_C_INCLUDE_DIRS})
endif()
if (NOT TARGET hdf5::hdf5_hl-shared)
	add_library(hdf5::hdf5_hl-shared UNKNOWN IMPORTED)
	set_property(TARGET hdf5::hdf5_hl-shared APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
	list(GET HDF5_HL_LIBRARIES 0 HDF5_MAIN_HL_LIB)
	list(REMOVE_AT HDF5_HL_LIBRARIES 0)

	#message("${HDF5_C_HL_LIBRARIES} ... ${HDF5_HL_LIBRARIES}")
	# HDF5_hdf5_hl_LIBRARY_RELEASE
	if(NOT "${HDF5_MAIN_HL_LIB}" STREQUAL "")
		set_target_properties(hdf5::hdf5_hl-shared PROPERTIES IMPORTED_LOCATION ${HDF5_MAIN_HL_LIB})
		set_target_properties(hdf5::hdf5_hl-shared PROPERTIES LINK_LIBRARIES "${HDF5_HL_LIBRARIES}")

	endif()
endif()

# These are rather polluting
mark_as_advanced(
	HDF5_C_LIBRARY_dl HDF5_C_LIBRARY_hdf5 HDF5_C_LIBRARY_hdf5_hl 
	HDF5_C_LIBRARY_m HDF5_C_LIBRARY_pthread HDF5_C_LIBRARY_sz HDF5_C_LIBRARY_z HDF5_DIR)

# CMake find_package script creator stuff

find_package(Eigen3)
