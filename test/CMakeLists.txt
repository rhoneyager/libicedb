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

if(BUILD_PLUGIN_PSU)
	add_test (NAME io-psu-gmm-boost # Test PSU file import
		COMMAND icedb-test-psu-shapes
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		)
	add_test (NAME io-psu-gmm-1 # Test PSU file import
		COMMAND icedb-3d_structures_importer
			--in ${CMAKE_SOURCE_DIR}/share/examples/shapes/PSU-GMM/psuaydinetal_geometry_aggregate_00004_GMM.nc
			--truncate
			--in-format psu --out ${CMAKE_BINARY_DIR}/test/psu-gmm-a04.h5
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		)
	add_test (NAME io-psu-gmm-2 # Test import without the --truncate option
		COMMAND icedb-3d_structures_importer
			--in ${CMAKE_SOURCE_DIR}/share/examples/shapes/PSU-GMM/psuaydinetal_geometry_aggregate_00004_GMM.nc
			--in-format psu --out ${CMAKE_BINARY_DIR}/test/psu-gmm-a04-2.h5
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		)
	if (BUILD_PLUGIN_SILO)
		add_test (NAME io-psu-gmm-to-silo-1 # Write a GMM file into the SILO visualization format
			COMMAND icedb-3d_structures
				--in ${CMAKE_BINARY_DIR}/test/psu-gmm-a04.h5
				--db-path "shape/Aggregate 00004"
				--out-format silo --out ${CMAKE_BINARY_DIR}/test/psu-gmm-a04.silo
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
			)
	endif()
endif()

if(BUILD_PLUGIN_ADDA)
	add_test (NAME io-adda-boost # Test ADDA file import
		COMMAND icedb-test-adda-shapes
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		)
	add_test (NAME io-adda-1 # Test ADDA file import
		COMMAND icedb-3d_structures_importer
			--in ${CMAKE_SOURCE_DIR}/share/examples/shapes/ADDA/rawtext_nocomments.adda
			--truncate
			--in-format adda --out ${CMAKE_BINARY_DIR}/test/adda-rawtext_nocomments.h5
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		)
	add_test (NAME io-adda-2 # Test ADDA file import
		COMMAND icedb-3d_structures_importer
			--in ${CMAKE_SOURCE_DIR}/share/examples/shapes/ADDA/sphere.geom
			--truncate
			--in-format adda --out ${CMAKE_BINARY_DIR}/test/adda-sphere-geom.h5
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		)
	if (BUILD_PLUGIN_SILO)
		add_test (NAME io-adda-to-silo-1 # Write a file into the SILO visualization format
			COMMAND icedb-3d_structures
				--in ${CMAKE_BINARY_DIR}/test/adda-sphere-geom.h5
				--db-path "shape/sphere.geom-particle-1"
				--out-format silo --out ${CMAKE_BINARY_DIR}/test/adda-sphere-geom.silo
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
			)
		add_test (NAME io-adda-to-silo-2 # Write a file into the SILO visualization format
			COMMAND icedb-3d_structures
				--in ${CMAKE_BINARY_DIR}/test/adda-rawtext_nocomments.h5
				--db-path "shape/rawtext_nocomments.adda-particle-1"
				--out-format silo --out ${CMAKE_BINARY_DIR}/test/adda-adda-rawtext_nocomments.silo
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
			)
	endif()
endif()

if(BUILD_PLUGIN_DDSCAT)
	execute_process(
		COMMAND ${CMAKE_COMMAND} -E tar xzf ${CMAKE_SOURCE_DIR}/share/examples/shapes/DDSCAT/1mm5shape.tar.gz 
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test/
	)

	add_test (NAME io-ddscat-boost-1 # Test DDSCAT file import
		COMMAND icedb-test-ddscat7-shapes
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		)
	add_test (NAME io-ddscat-boost-2 # Test DDSCAT file import
		COMMAND icedb-test-ddscat6-shapes
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		)
	add_test (NAME io-ddscat7-1 # Test DDSCAT file import
		COMMAND icedb-3d_structures_importer
			--in ${CMAKE_SOURCE_DIR}/share/examples/shapes/DDSCAT/ddscat7ice.dat
			--truncate
			--in-format ddscat --out ${CMAKE_BINARY_DIR}/test/ddscat7ice.h5
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		)
	add_test (NAME io-ddscat7-2 # Test DDSCAT file import
		COMMAND icedb-3d_structures_importer
			--in ${CMAKE_SOURCE_DIR}/share/examples/shapes/DDSCAT/ddscat7melting.dat
			--truncate
			--in-format ddscat --out ${CMAKE_BINARY_DIR}/test/ddscat7melting.h5
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		)
	add_test (NAME io-ddscat7-3 # Test DDSCAT file import, without the --truncate option
		COMMAND icedb-3d_structures_importer
			--in ${CMAKE_SOURCE_DIR}/share/examples/shapes/DDSCAT/ddscat7melting.dat
			--in-format ddscat --out ${CMAKE_BINARY_DIR}/test/ddscat7ice.h5
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		)
	add_test (NAME io-ddscat7-4 # Test DDSCAT file import
		COMMAND icedb-3d_structures_importer
			--in ${CMAKE_BINARY_DIR}/test/1mm5shape.txt
			--truncate
			--in-format ddscat --out ${CMAKE_BINARY_DIR}/test/1mm5shape.h5
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		)
	# TODO: Add DDSCAT 6 tests
	# TODO: Add DDSCAT 6 silo tests
	if (BUILD_PLUGIN_SILO)
		add_test (NAME io-ddscat7-to-silo-1 # Write a file into the SILO visualization format
			COMMAND icedb-3d_structures
				--in ${CMAKE_BINARY_DIR}/test/ddscat7ice.h5
				--db-path "shape/ddscat7ice.dat-particle-1"
				--out-format silo --out ${CMAKE_BINARY_DIR}/test/ddscat7ice.silo
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
			)
		add_test (NAME io-ddscat7-to-silo-2 # Write a file into the SILO visualization format
			COMMAND icedb-3d_structures
				--in ${CMAKE_BINARY_DIR}/test/ddscat7melting.h5
				--db-path "shape/ddscat7melting.dat-particle-1"
				--out-format silo --out ${CMAKE_BINARY_DIR}/test/ddscat7melting.silo
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
			)
		add_test (NAME io-ddscat7-to-silo-3 # Write a file into the SILO visualization format
			COMMAND icedb-3d_structures
				--in ${CMAKE_BINARY_DIR}/test/1mm5shape.h5
				--db-path "shape/1mm5shape.txt-particle-1"
				--out-format silo --out ${CMAKE_BINARY_DIR}/test/1mm5shape.silo
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
			)
	endif()
endif()