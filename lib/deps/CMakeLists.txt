# Code to collect the dependencies into the build directory on Windows.
# Manually collecting these all the time is rather aggrevating.
add_executable(placeholder ../src/_placeholder.cpp)
storebin(placeholder)
set_target_properties(placeholder PROPERTIES FOLDER "Build")
add_executable(placeholder2 ../src/_placeholder.cpp)
storebin(placeholder2)
set_target_properties(placeholder2 PROPERTIES FOLDER "Build")
if (WIN32 AND NOT CMAKE)
	add_custom_target(copy_dependencies_to_build_dirs
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:hdf5::hdf5-shared> $<TARGET_FILE_DIR:placeholder>
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:hdf5::hdf5_hl-shared> $<TARGET_FILE_DIR:placeholder>
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:hdf5::hdf5_tools-shared> $<TARGET_FILE_DIR:placeholder>
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:impBoost::filesystem> $<TARGET_FILE_DIR:placeholder>
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:impBoost::system> $<TARGET_FILE_DIR:placeholder>
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:impBoost::program_options> $<TARGET_FILE_DIR:placeholder>
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:impBoost::date_time> $<TARGET_FILE_DIR:placeholder>
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:impBoost::regex> $<TARGET_FILE_DIR:placeholder>
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:impBoost::unit_test_framework> $<TARGET_FILE_DIR:placeholder>
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:impZLIB::ZLIB> $<TARGET_FILE_DIR:placeholder>
		)
	set_target_properties( copy_dependencies_to_build_dirs PROPERTIES FOLDER "Build")
	add_dependencies(placeholder2 copy_dependencies_to_build_dirs)
	
	# copy_icedb_dependencies_to_build_dirs has an automatic dependency on placeholder.
	# This way, the files get copied only once.

	install(PROGRAMS 
		$<TARGET_FILE:hdf5::hdf5-shared> $<TARGET_FILE:hdf5::hdf5_hl-shared> $<TARGET_FILE:hdf5::hdf5_tools-shared>
		$<TARGET_FILE:impBoost::filesystem> $<TARGET_FILE:impBoost::system> $<TARGET_FILE:impBoost::program_options>
		$<TARGET_FILE:impBoost::date_time> $<TARGET_FILE:impBoost::regex> $<TARGET_FILE:impBoost::unit_test_framework>
		$<TARGET_FILE:impZLIB::ZLIB>
		DESTINATION ${CMAKE_INSTALL_BINDIR} #${RBIN_DIR}/$<CONFIG>
		COMPONENT Libraries)

	# Boost targets do not fully have their imported_locations set. The shared libraries are not included.
	# Query IMPORTED_LOCATION_DEBUG, IMPORTED_LOCATION_RELEASE and IMPORTED_LOCATION.
	# Only copy shared libraries (i.e. ones that do not start with lib_____).
		#COMMAND ${CMAKE_COMMAND} -E copy
		#	$<TARGET_PROPERTY:boost::filesystem,IMPORTED_LOCATION_RELEASE>
		#	$<TARGET_FILE_DIR:icedb>
ENDIF()



add_subdirectory(BetterThrow)
add_subdirectory(BinaryIO)
add_subdirectory(HH)
add_subdirectory(third_party)
