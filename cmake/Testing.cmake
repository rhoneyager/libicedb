function(prep_test GITB)
	# Get hostname
	cmake_host_system_information(RESULT HN QUERY 
		HOSTNAME FQDN OS_NAME OS_RELEASE OS_VERSION OS_PLATFORM)
	list(GET HN 0 iHostname)
	list(GET HN 1 iFQDN)
	list(GET HN 2 iOS_NAME)
	list(GET HN 3 iOS_RELEASE)
	list(GET HN 4 iOS_VERSION)
	list(GET HN 5 iOS_PLATFORM)
	# Get OS info
	if("${CMAKE_HOST_SYSTEM_NAME}" MATCHES "Linux")
		find_program(LSB_RELEASE_EXEC lsb_release)
		mark_as_advanced(LSB_RELEASE_EXEC)
		if(LSB_RELEASE_EXEC)
			execute_process(COMMAND ${LSB_RELEASE_EXEC} -is
				OUTPUT_VARIABLE LSB_DISTRIBUTION_NAME_SHORT
				OUTPUT_STRIP_TRAILING_WHITESPACE
				)
			execute_process(COMMAND ${LSB_RELEASE_EXEC} -rs
				OUTPUT_VARIABLE LSB_RELEASE_ID_SHORT
				OUTPUT_STRIP_TRAILING_WHITESPACE
				)
			set(TESTING_SYSTEM_NAME "${LSB_DISTRIBUTION_NAME_SHORT}_${LSB_RELEASE_ID_SHORT}")
		else()
			message(STATUS "The lsb_release command cannot be found. Using defaults.")
			set(TESTING_SYSTEM_NAME "${iOS_NAME}_${iOS_RELEASE}_${iOS_VERSION}")
		endif()
	elseif("${CMAKE_HOST_SYSTEM_NAME}" MATCHES "FreeBSD")
		set(TESTING_SYSTEM_NAME "${iOS_NAME}_${iOS_RELEASE}_${iOS_VERSION}")
	elseif("${CMAKE_HOST_SYSTEM_NAME}" MATCHES "Darwin")
		set(TESTING_SYSTEM_NAME "${iOS_NAME}_${iOS_RELEASE}_${iOS_VERSION}")
	elseif("${CMAKE_HOST_SYSTEM_NAME}" MATCHES "Windows")
		set(TESTING_SYSTEM_NAME "${iOS_NAME}_${iOS_RELEASE}_${iOS_VERSION}")
	else()
		message(STATUS "Unknown OS ${CMAKE_HOST_SYSTEM_NAME}")
		set(TESTING_SYSTEM_NAME "Unknown")
	endif()

	# Compiler info
	#message("${CMAKE_CXX_COMPILER_ID}")
	set(TESTING_CXX_COMPILER_ID "${CMAKE_CXX_COMPILER_ID}_${CMAKE_CXX_COMPILER_VERSION}")
	set(TESTING_COMPILER "${TESTING_CXX_COMPILER_ID}")

	message(STATUS "Git Branch: ${GITB}")
	message(STATUS "System ID: ${TESTING_SYSTEM_NAME}")
	message(STATUS "Compilers: ${TESTING_COMPILER}")
	message(STATUS "HDF5 Version: ${HDF5_VERSION}")
	set(BUILDNAME_BASE "${TESTING_SYSTEM_NAME}_${TESTING_COMPILER}_H5-${HDF5_VERSION}_${GITB}")
	set(BUILDNAME "${BUILDNAME_BASE}" CACHE STRING "Build name variable for CTest" )
	set(SITE "${iFQDN}" CACHE STRING "Site name" )
	message(STATUS "Build ${BUILDNAME}")
	message(STATUS "Host FQDN ${iFQDN}")
	message(STATUS "Site ${SITE}")
	#mark_as_advanced(BUILDNAME SITE)
	#message("${HN}")

endfunction()

prep_test("${GITBRANCH}")


ENABLE_TESTING()
INCLUDE(CTest)

