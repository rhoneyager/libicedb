if (BUILD_TESTING)

	# CTest stuff. May switch from CTest in the future.
	set(BUILDNAME "Unspecified_Build_Name_for_CTest" CACHE STRING "Build name variable for CTest")
	set(SITE "Unknown_Site_Name_for_CTest" CACHE STRING "Site name")
	mark_as_advanced(BUILDNAME SITE)

	file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/test)

	#add_subdirectory(core)
	#add_subdirectory(database)
	add_subdirectory(io)


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
	

	# Run a program without loading any plugins
	add_test(NAME plugin-test-load-none
		COMMAND icedb-version --dll-no-default-locations WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

	if(BUILD_PLUGIN_PSU)
		# Attempt to load and unload only the PSU plugin, and check if there is an error.
		add_test (NAME io-psu-plugin-test
			COMMAND icedb-version --dll-no-default-locations
				--dll-load-onelevel $<TARGET_FILE:io_psu>
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

		add_test (NAME io-psu-gmm-boost # Test PSU file import
			COMMAND icedb-test-psu-shapes 
				--dll-load-onelevel $<TARGET_FILE:io_psu>
				--dll-no-default-locations
				--share-dir ${CMAKE_SOURCE_DIR}/share
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
			)
		add_test (NAME io-psu-gmm-1 # Test PSU file import
			COMMAND icedb-3d_structures_importer
				--in ${CMAKE_SOURCE_DIR}/share/examples/shapes/PSU-GMM/psuaydinetal_geometry_aggregate_00004_GMM.nc
				--truncate
				--in-format psu --out ${CMAKE_BINARY_DIR}/test/psu-gmm-a04.h5
				--dll-load-onelevel $<TARGET_FILE:io_psu>
				--dll-no-default-locations
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
			)
		add_test (NAME io-psu-gmm-2 # Test import without the --truncate option.
			# Really just testing that the program does not crash. 
			COMMAND icedb-3d_structures_importer
				--in ${CMAKE_SOURCE_DIR}/share/examples/shapes/PSU-GMM/psuaydinetal_geometry_aggregate_00004_GMM.nc
				--in-format psu --out ${CMAKE_BINARY_DIR}/test/psu-gmm-a04-2.h5
				--dll-load-onelevel $<TARGET_FILE:io_psu>
				--dll-no-default-locations
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
			)
		if (BUILD_PLUGIN_SILO)
			add_test (NAME io-psu-gmm-to-silo-1 # Write a GMM file into the SILO visualization format
				COMMAND icedb-3d_structures
					--in ${CMAKE_BINARY_DIR}/test/psu-gmm-a04.h5
					--db-path "shape/Aggregate 00004"
					--out-format silo --out ${CMAKE_BINARY_DIR}/test/psu-gmm-a04.silo
					--dll-load-onelevel $<TARGET_FILE:io_psu>
					--dll-load-onelevel $<TARGET_FILE:plugin_silo>
					--dll-no-default-locations
				WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
				)
		endif()
		if (DO_NCDUMP_TESTS)
			add_test (NAME io-psu-gmm-ncdump
				COMMAND ncdump -h ${CMAKE_BINARY_DIR}/test/psu-gmm-a04.h5)
		endif()
	endif()

	if(BUILD_PLUGIN_ADDA)
		# Attempt to load and unload only the ADDA plugin, and check if there is an error.
		add_test (NAME io-adda-plugin-test
			COMMAND icedb-version --dll-no-default-locations
				--dll-load-onelevel $<TARGET_FILE:io_adda>
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
		add_test (NAME io-adda-boost # Test ADDA file import
			COMMAND icedb-test-adda-shapes 
				--dll-load-onelevel $<TARGET_FILE:io_adda>
				--dll-no-default-locations
				--share-dir ${CMAKE_SOURCE_DIR}/share
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
			)
		add_test (NAME io-adda-1 # Test ADDA file import
			COMMAND icedb-3d_structures_importer
				--in ${CMAKE_SOURCE_DIR}/share/examples/shapes/ADDA/rawtext_nocomments.adda
				--truncate
				--in-format adda --out ${CMAKE_BINARY_DIR}/test/adda-rawtext_nocomments.h5
				--dll-load-onelevel $<TARGET_FILE:io_adda>
				--dll-no-default-locations
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
			)
		add_test (NAME io-adda-2 # Test ADDA file import
			COMMAND icedb-3d_structures_importer
				--in ${CMAKE_SOURCE_DIR}/share/examples/shapes/ADDA/sphere.geom
				--truncate
				--in-format adda --out ${CMAKE_BINARY_DIR}/test/adda-sphere-geom.h5
				--dll-load-onelevel $<TARGET_FILE:io_adda>
				--dll-no-default-locations
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
			)
		if (BUILD_PLUGIN_SILO)
			add_test (NAME io-adda-to-silo-1 # Write a file into the SILO visualization format
				COMMAND icedb-3d_structures
					--in ${CMAKE_BINARY_DIR}/test/adda-sphere-geom.h5
					--db-path "shape/sphere.geom-particle-1"
					--out-format silo --out ${CMAKE_BINARY_DIR}/test/adda-sphere-geom.silo
					--dll-load-onelevel $<TARGET_FILE:io_adda>
					--dll-load-onelevel $<TARGET_FILE:plugin_silo>
					--dll-no-default-locations
				WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
				)
			add_test (NAME io-adda-to-silo-2 # Write a file into the SILO visualization format
				COMMAND icedb-3d_structures
					--in ${CMAKE_BINARY_DIR}/test/adda-rawtext_nocomments.h5
					--db-path "shape/rawtext_nocomments.adda-particle-1"
					--out-format silo --out ${CMAKE_BINARY_DIR}/test/adda-adda-rawtext_nocomments.silo
					--dll-load-onelevel $<TARGET_FILE:io_adda>
					--dll-load-onelevel $<TARGET_FILE:plugin_silo>
					--dll-no-default-locations
				WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
				)
		endif()
		if (DO_NCDUMP_TESTS)
			add_test (NAME io-adda-ncdump
				COMMAND ncdump -h ${CMAKE_BINARY_DIR}/test/adda-rawtext_nocomments.h5)
		endif()
	endif()

	if(BUILD_PLUGIN_DDSCAT)
		# Attempt to load and unload only the DDSCAT plugin, and check if there is an error.
		add_test (NAME io-ddscat-plugin-test
			COMMAND icedb-version --dll-no-default-locations
				--dll-load-onelevel $<TARGET_FILE:io_ddscat>
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

		execute_process(
			COMMAND ${CMAKE_COMMAND} -E tar xzf ${CMAKE_SOURCE_DIR}/share/examples/shapes/DDSCAT/1mm5shape.tar.gz 
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test/
		)

		add_test (NAME io-ddscat-boost-1 # Test DDSCAT file import
			COMMAND icedb-test-ddscat7-shapes
				--dll-load-onelevel $<TARGET_FILE:io_ddscat>
				--dll-no-default-locations
				--share-dir ${CMAKE_SOURCE_DIR}/share
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
			)
		add_test (NAME io-ddscat-boost-2 # Test DDSCAT file import
			COMMAND icedb-test-ddscat6-shapes
				--dll-load-onelevel $<TARGET_FILE:io_ddscat>
				--dll-no-default-locations
				--share-dir ${CMAKE_SOURCE_DIR}/share
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
			)
		add_test (NAME io-ddscat7-1 # Test DDSCAT file import
			COMMAND icedb-3d_structures_importer
				--in ${CMAKE_SOURCE_DIR}/share/examples/shapes/DDSCAT/ddscat7ice.dat
				--truncate
				--in-format ddscat --out ${CMAKE_BINARY_DIR}/test/ddscat7ice.h5
				--dll-load-onelevel $<TARGET_FILE:io_ddscat>
				--dll-no-default-locations
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
			)
		add_test (NAME io-ddscat7-2 # Test DDSCAT file import
			COMMAND icedb-3d_structures_importer
				--in ${CMAKE_SOURCE_DIR}/share/examples/shapes/DDSCAT/ddscat7melting.dat
				--truncate
				--in-format ddscat --out ${CMAKE_BINARY_DIR}/test/ddscat7melting.h5
				--dll-load-onelevel $<TARGET_FILE:io_ddscat>
				--dll-no-default-locations
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
			)
		add_test (NAME io-ddscat7-3 # Test DDSCAT file import, without the --truncate option
			COMMAND icedb-3d_structures_importer
				--in ${CMAKE_SOURCE_DIR}/share/examples/shapes/DDSCAT/ddscat7melting.dat
				--in-format ddscat --out ${CMAKE_BINARY_DIR}/test/ddscat7ice.h5
				--dll-load-onelevel $<TARGET_FILE:io_ddscat>
				--dll-no-default-locations
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
			)
		add_test (NAME io-ddscat7-4 # Test DDSCAT file import
			COMMAND icedb-3d_structures_importer
				--in ${CMAKE_BINARY_DIR}/test/1mm5shape.txt
				--truncate
				--in-format ddscat --out ${CMAKE_BINARY_DIR}/test/1mm5shape.h5
				--dll-load-onelevel $<TARGET_FILE:io_ddscat>
				--dll-no-default-locations
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
			)
		add_test (NAME io-ddscat6-1 # Test DDSCAT file import
			COMMAND icedb-3d_structures_importer
				--in ${CMAKE_SOURCE_DIR}/share/examples/shapes/DDSCAT/ddscat6ice.dat
				--truncate
				--in-format ddscat --out ${CMAKE_BINARY_DIR}/test/ddscat6ice.h5
				--dll-load-onelevel $<TARGET_FILE:io_ddscat>
				--dll-no-default-locations
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
			)
		add_test (NAME io-ddscat6-2 # Test DDSCAT file import
			COMMAND icedb-3d_structures_importer
				--in ${CMAKE_SOURCE_DIR}/share/examples/shapes/DDSCAT/ddscat6melting.dat
				--truncate
				--in-format ddscat --out ${CMAKE_BINARY_DIR}/test/ddscat6melting.h5
				--dll-load-onelevel $<TARGET_FILE:io_ddscat>
				--dll-no-default-locations
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
			)
		# TODO: Add DDSCAT 6 silo tests?
		if (BUILD_PLUGIN_SILO)
			add_test (NAME io-ddscat7-to-silo-1 # Write a file into the SILO visualization format
				COMMAND icedb-3d_structures
					--in ${CMAKE_BINARY_DIR}/test/ddscat7ice.h5
					--db-path "shape/ddscat7ice.dat-particle-1"
					--out-format silo --out ${CMAKE_BINARY_DIR}/test/ddscat7ice.silo
					--dll-load-onelevel $<TARGET_FILE:io_ddscat>
					--dll-load-onelevel $<TARGET_FILE:plugin_silo>
					--dll-no-default-locations
				WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
				)
			add_test (NAME io-ddscat7-to-silo-2 # Write a file into the SILO visualization format
				COMMAND icedb-3d_structures
					--in ${CMAKE_BINARY_DIR}/test/ddscat7melting.h5
					--db-path "shape/ddscat7melting.dat-particle-1"
					--out-format silo --out ${CMAKE_BINARY_DIR}/test/ddscat7melting.silo
					--dll-load-onelevel $<TARGET_FILE:io_ddscat>
					--dll-load-onelevel $<TARGET_FILE:plugin_silo>
					--dll-no-default-locations
				WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
				)
			add_test (NAME io-ddscat7-to-silo-3 # Write a file into the SILO visualization format
				COMMAND icedb-3d_structures
					--in ${CMAKE_BINARY_DIR}/test/1mm5shape.h5
					--db-path "shape/1mm5shape.txt-particle-1"
					--out-format silo --out ${CMAKE_BINARY_DIR}/test/1mm5shape.silo
					--dll-load-onelevel $<TARGET_FILE:io_ddscat>
					--dll-load-onelevel $<TARGET_FILE:plugin_silo>
					--dll-no-default-locations
				WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
				)
		endif()
		if (DO_NCDUMP_TESTS)
			add_test (NAME io-ddscat7-ncdump
				COMMAND ncdump -h ${CMAKE_BINARY_DIR}/test/ddscat7ice.h5)
		endif()
	endif()

	if(BUILD_PLUGIN_SILO)
		# Attempt to load and unload only the silo plugin, and check if there is an error.
		add_test (NAME io-silo-plugin-test
			COMMAND icedb-version --dll-no-default-locations
			--dll-load-onelevel $<TARGET_FILE:plugin_silo>
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
	endif()

endif()
