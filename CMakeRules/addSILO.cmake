# This script exists to facilitate finding SILO on both Windows and Linux.
# My Windows build of SILO provides a CMake configuration file, but on Linux
# package managers do not provide this feature, and a variant of FindSILO.cmake 
# must be implemented.

if (WIN32 AND NOT CYGWIN)
	find_package(silo REQUIRED)
else()
	FIND_PATH(SILO_INCLUDE_DIR silo.h
		/usr/local/include
		/usr/include
		)

	FIND_LIBRARY(SILO_LIBRARY NAMES silo siloh5
		/usr/lib
		/usr/local/lib
	)

	IF(SILO_INCLUDE_DIR AND SILO_LIBRARY)
		SET(SILO_LIBRARIES ${SILO_LIBRARY})
		SET(SILO_FOUND "YES" )

		add_library(silo UNKNOWN IMPORTED)
		set_target_properties(silo PROPERTIES
			INTERFACE_INCLUDE_DIRECTORIES "${SILO_INCLUDE_DIR}"
			INTERFACE_LINK_LIBRARIES "hdf5::hdf5-shared;hdf5::hdf5_hl-shared"
			)

		set_property(TARGET silo APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
		set_target_properties(silo PROPERTIES IMPORTED_LOCATION_RELEASE "${dll_release}")
		set_property(TARGET silo APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
		set_target_properties(silo PROPERTIES IMPORTED_LOCATION_DEBUG "${dll_debug}")
	else()
		SET(SILO_FOUND "NO")
		error("Cannot find SILO")
	endif()

endif()
