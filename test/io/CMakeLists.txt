include(addapp)
set( CMAKE_DIRECTORY_LABELS "io" )

if (BUILD_PLUGIN_PSU)
	# Documents the app in Windows Explorer
	generaterc("icedb-test-psu-shapes" "exe"
		"icedb"
		"Test PSU shape reading and writing code"
		 "${CMAKE_CURRENT_BINARY_DIR}/app.rc"
		 )
	# The source files
	set(srcs-psu test-psu-shapes.cpp)
	IF (WIN32 AND NOT CYGWIN)
	SET(srcs-psu ${srcs-psu} 
		"${CMAKE_CURRENT_BINARY_DIR}/app.rc")
	ENDIF()

	add_executable(icedb-test-psu-shapes ${srcs-psu})
	target_link_libraries(icedb-test-psu-shapes icedb_testing icedb io_psu Boost::unit_test_framework)
	if (BUILD_PLUGIN_SILO)
		add_dependencies(icedb-test-psu-shapes plugin_silo)
		target_compile_definitions(icedb-test-psu-shapes PRIVATE TEST_WITH_SILO=1)
	else()
		target_compile_definitions(icedb-test-psu-shapes PRIVATE TEST_WITH_SILO=0)
	endif()
	#add_dependencies(icedb-test-psu-shapes qh) # TODO: Remove after algorithm tests complete.
	addapp_test(icedb-test-psu-shapes Testing/IO)
	storebin(icedb-test-psu-shapes)
endif()

if (BUILD_PLUGIN_ADDA)
	# Documents the app in Windows Explorer
	generaterc("icedb-test-adda-shapes" "exe"
		"icedb"
		"Test ADDA shape reading and writing code"
		 "${CMAKE_CURRENT_BINARY_DIR}/app.rc"
		 )
	# The source files
	set(srcs-adda test-adda-shapes.cpp)
	IF (WIN32 AND NOT CYGWIN)
		SET(srcs-adda ${srcs-adda} "${CMAKE_CURRENT_BINARY_DIR}/app.rc")
	ENDIF()

	add_executable(icedb-test-adda-shapes ${srcs-adda})
	target_link_libraries(icedb-test-adda-shapes icedb_testing icedb io_adda Boost::unit_test_framework)
	addapp_test(icedb-test-adda-shapes Testing/IO)
	storebin(icedb-test-adda-shapes)
endif()

if (BUILD_PLUGIN_DDSCAT)
	# Documents the app in Windows Explorer
	generaterc("icedb-test-ddscat7-shapes" "exe"
		"icedb"
		"Test DDSCAT 7 shape reading and writing code"
		 "${CMAKE_CURRENT_BINARY_DIR}/app.rc"
		 )
	# The source files
	set(srcs-ddscat7 test-ddscat7-shapes.cpp)
	IF (WIN32 AND NOT CYGWIN)
		SET(srcs-ddscat7 ${srcs-ddscat7} "${CMAKE_CURRENT_BINARY_DIR}/app.rc")
	ENDIF()
	add_executable(icedb-test-ddscat7-shapes ${srcs-ddscat7})
	target_link_libraries(icedb-test-ddscat7-shapes icedb_testing icedb io_ddscat Boost::unit_test_framework)
	addapp_test(icedb-test-ddscat7-shapes Testing/IO)
	storebin(icedb-test-ddscat7-shapes)

	# Documents the app in Windows Explorer
	generaterc("icedb-test-ddscat6-shapes" "exe"
		"icedb"
		"Test DDSCAT 6 shape reading and writing code"
		 "${CMAKE_CURRENT_BINARY_DIR}/app.rc"
		 )
	# The source files
	set(srcs-ddscat6 test-ddscat6-shapes.cpp)
	IF (WIN32 AND NOT CYGWIN)
		SET(srcs-ddscat6 ${srcs-ddscat6} "${CMAKE_CURRENT_BINARY_DIR}/app.rc")
	ENDIF()
	add_executable(icedb-test-ddscat6-shapes ${srcs-ddscat6})
	target_link_libraries(icedb-test-ddscat6-shapes icedb_testing icedb io_ddscat Boost::unit_test_framework)
	addapp_test(icedb-test-ddscat6-shapes Testing/IO)
	storebin(icedb-test-ddscat6-shapes)
endif()

if(BUILD_APPS)
		if(BUILD_PLUGIN_PSU)
			# Attempt to load and unload only the PSU plugin, and check if there is an error.
			add_custom_target(io-psu-plugin-test
				COMMAND icedb-version -- --dll-no-default-locations
					--dll-load-onelevel $<TARGET_FILE:io_psu>
				WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
			register_test(io-psu-plugin-test "Testing/IO")

			add_custom_target(io-psu-gmm-boost # Test PSU file import
				COMMAND icedb-test-psu-shapes --
					--dll-load-onelevel $<TARGET_FILE:io_psu>
					--dll-no-default-locations
					--share-dir ${CMAKE_SOURCE_DIR}/share
				WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
				)
			register_test(io-psu-gmm-boost "Testing/IO")
			add_custom_target(io-psu-gmm-1 # Test PSU file import
				COMMAND icedb-3d_structures_importer
					--in ${CMAKE_SOURCE_DIR}/share/icedb/examples/shapes/PSU-GMM/psuaydinetal_geometry_aggregate_00004_GMM.nc
					--truncate
					--in-format psu --out ${CMAKE_BINARY_DIR}/test/psu-gmm-a04.h5
					--dll-load-onelevel $<TARGET_FILE:io_psu>
					--dll-no-default-locations
				WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
				)
			register_test(io-psu-gmm-1 "Testing/IO")
			add_custom_target(io-psu-gmm-2 # Test import without the --truncate option.
				# Really just testing that the program does not crash. 
				COMMAND icedb-3d_structures_importer
					--in ${CMAKE_SOURCE_DIR}/share/icedb/examples/shapes/PSU-GMM/psuaydinetal_geometry_aggregate_00004_GMM.nc
					--in-format psu --out ${CMAKE_BINARY_DIR}/test/psu-gmm-a04-2.h5
					--dll-load-onelevel $<TARGET_FILE:io_psu>
					--dll-no-default-locations
				WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
				)
			register_test(io-psu-gmm-2 "Testing/IO")
			if (BUILD_PLUGIN_SILO)
				add_custom_target(io-psu-gmm-to-silo-1 # Write a GMM file into the SILO visualization format
					COMMAND icedb-3d_structures
						--in ${CMAKE_BINARY_DIR}/test/psu-gmm-a04.h5
						--db-path "shape/Aggregate 00004"
						--out-format silo --out ${CMAKE_BINARY_DIR}/test/psu-gmm-a04.silo
						--dll-load-onelevel $<TARGET_FILE:io_psu>
						--dll-load-onelevel $<TARGET_FILE:plugin_silo>
						--dll-no-default-locations
					WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
					)
				register_test(io-psu-gmm-to-silo-1 "Testing/IO")
			endif()
			if (DO_NCDUMP_TESTS)
				add_custom_target(io-psu-gmm-ncdump
					COMMAND ncdump -h ${CMAKE_BINARY_DIR}/test/psu-gmm-a04.h5)
				register_test(io-psu-gmm-ncdump "Testing/IO")
			endif()
		endif()

		if(BUILD_PLUGIN_ADDA)
			# Attempt to load and unload only the ADDA plugin, and check if there is an error.
			add_custom_target(io-adda-plugin-test
				COMMAND icedb-version --dll-no-default-locations
					--dll-load-onelevel $<TARGET_FILE:io_adda>
				WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
			register_test(io-adda-plugin-test "Testing/IO")
			add_custom_target(io-adda-boost # Test ADDA file import
				COMMAND icedb-test-adda-shapes -- 
					--dll-load-onelevel $<TARGET_FILE:io_adda>
					--dll-no-default-locations
					--share-dir ${CMAKE_SOURCE_DIR}/share
				WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
				)
			register_test(io-adda-boost "Testing/IO")
			add_custom_target(io-adda-1 # Test ADDA file import
				COMMAND icedb-3d_structures_importer
					--in ${CMAKE_SOURCE_DIR}/share/icedb/examples/shapes/ADDA/rawtext_nocomments.adda
					--truncate
					--in-format adda --out ${CMAKE_BINARY_DIR}/test/adda-rawtext_nocomments.h5
					--dll-load-onelevel $<TARGET_FILE:io_adda>
					--dll-no-default-locations
				WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
				)
			register_test(io-adda-1 "Testing/IO")
			add_custom_target(io-adda-2 # Test ADDA file import
				COMMAND icedb-3d_structures_importer
					--in ${CMAKE_SOURCE_DIR}/share/icedb/examples/shapes/ADDA/sphere.geom
					--truncate
					--in-format adda --out ${CMAKE_BINARY_DIR}/test/adda-sphere-geom.h5
					--dll-load-onelevel $<TARGET_FILE:io_adda>
					--dll-no-default-locations
				WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
				)
			register_test(io-adda-2 "Testing/IO")
			if (BUILD_PLUGIN_SILO)
				add_custom_target(io-adda-to-silo-1 # Write a file into the SILO visualization format
					COMMAND icedb-3d_structures
						--in ${CMAKE_BINARY_DIR}/test/adda-sphere-geom.h5
						--db-path "shape/sphere.geom-particle-1"
						--out-format silo --out ${CMAKE_BINARY_DIR}/test/adda-sphere-geom.silo
						--dll-load-onelevel $<TARGET_FILE:io_adda>
						--dll-load-onelevel $<TARGET_FILE:plugin_silo>
						--dll-no-default-locations
					WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
					)
				register_test(io-adda-to-silo-1 "Testing/IO")
				add_custom_target(io-adda-to-silo-2 # Write a file into the SILO visualization format
					COMMAND icedb-3d_structures
						--in ${CMAKE_BINARY_DIR}/test/adda-rawtext_nocomments.h5
						--db-path "shape/rawtext_nocomments.adda-particle-1"
						--out-format silo --out ${CMAKE_BINARY_DIR}/test/adda-adda-rawtext_nocomments.silo
						--dll-load-onelevel $<TARGET_FILE:io_adda>
						--dll-load-onelevel $<TARGET_FILE:plugin_silo>
						--dll-no-default-locations
					WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
					)
				register_test(io-adda-to-silo-2 "Testing/IO")
			endif()
			if (DO_NCDUMP_TESTS)
				add_custom_target(io-adda-ncdump
					COMMAND ncdump -h ${CMAKE_BINARY_DIR}/test/adda-rawtext_nocomments.h5)
				register_test(io-adda-ncdump "Testing/IO")
			endif()
		endif()

		if(BUILD_PLUGIN_DDSCAT)
			# Attempt to load and unload only the DDSCAT plugin, and check if there is an error.
			add_custom_target(io-ddscat-plugin-test
				COMMAND icedb-version --dll-no-default-locations
					--dll-load-onelevel $<TARGET_FILE:io_ddscat>
				WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
			register_test(io-ddscat-plugin-test "Testing/IO")

			execute_process(
				COMMAND ${CMAKE_COMMAND} -E tar xzf ${CMAKE_SOURCE_DIR}/share/icedb/examples/shapes/DDSCAT/1mm5shape.tar.gz 
				WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test/
			)

			add_custom_target(io-ddscat-boost-1 # Test DDSCAT file import
				COMMAND icedb-test-ddscat7-shapes -- 
					--dll-load-onelevel $<TARGET_FILE:io_ddscat>
					--dll-no-default-locations
					--share-dir ${CMAKE_SOURCE_DIR}/share
				WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
				)
			register_test(io-ddscat-boost-1 "Testing/IO")
			add_custom_target(io-ddscat-boost-2 # Test DDSCAT file import
				COMMAND icedb-test-ddscat6-shapes -- 
					--dll-load-onelevel $<TARGET_FILE:io_ddscat>
					--dll-no-default-locations
					--share-dir ${CMAKE_SOURCE_DIR}/share
				WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
				)
			register_test(io-ddscat-boost-2 "Testing/IO")
			add_custom_target(io-ddscat7-1 # Test DDSCAT file import
				COMMAND icedb-3d_structures_importer 
					--in ${CMAKE_SOURCE_DIR}/share/icedb/examples/shapes/DDSCAT/ddscat7ice.dat
					--truncate
					--in-format ddscat --out ${CMAKE_BINARY_DIR}/test/ddscat7ice.h5
					--dll-load-onelevel $<TARGET_FILE:io_ddscat>
					--dll-no-default-locations
				WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
				)
			register_test(io-ddscat7-1 "Testing/IO")
			add_custom_target(io-ddscat7-2 # Test DDSCAT file import
				COMMAND icedb-3d_structures_importer
					--in ${CMAKE_SOURCE_DIR}/share/icedb/examples/shapes/DDSCAT/ddscat7melting.dat
					--truncate
					--in-format ddscat --out ${CMAKE_BINARY_DIR}/test/ddscat7melting.h5
					--dll-load-onelevel $<TARGET_FILE:io_ddscat>
					--dll-no-default-locations
				WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
				)
			register_test(io-ddscat7-2 "Testing/IO")
			add_custom_target(io-ddscat7-3 # Test DDSCAT file import, without the --truncate option
				COMMAND icedb-3d_structures_importer
					--in ${CMAKE_SOURCE_DIR}/share/icedb/examples/shapes/DDSCAT/ddscat7melting.dat
					--in-format ddscat --out ${CMAKE_BINARY_DIR}/test/ddscat7ice.h5
					--dll-load-onelevel $<TARGET_FILE:io_ddscat>
					--dll-no-default-locations
				WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
				)
			register_test(io-ddscat7-3 "Testing/IO")
			add_custom_target(io-ddscat7-4 # Test DDSCAT file import
				COMMAND icedb-3d_structures_importer
					--in ${CMAKE_BINARY_DIR}/test/1mm5shape.txt
					--truncate
					--in-format ddscat --out ${CMAKE_BINARY_DIR}/test/1mm5shape.h5
					--dll-load-onelevel $<TARGET_FILE:io_ddscat>
					--dll-no-default-locations
				WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
				)
			register_test(io-ddscat7-4 "Testing/IO")
			add_custom_target(io-ddscat6-1 # Test DDSCAT file import
				COMMAND icedb-3d_structures_importer
					--in ${CMAKE_SOURCE_DIR}/share/icedb/examples/shapes/DDSCAT/ddscat6ice.dat
					--truncate
					--in-format ddscat --out ${CMAKE_BINARY_DIR}/test/ddscat6ice.h5
					--dll-load-onelevel $<TARGET_FILE:io_ddscat>
					--dll-no-default-locations
				WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
				)
			register_test(io-ddscat6-1 "Testing/IO")
			add_custom_target(io-ddscat6-2 # Test DDSCAT file import
				COMMAND icedb-3d_structures_importer
					--in ${CMAKE_SOURCE_DIR}/share/icedb/examples/shapes/DDSCAT/ddscat6melting.dat
					--truncate
					--in-format ddscat --out ${CMAKE_BINARY_DIR}/test/ddscat6melting.h5
					--dll-load-onelevel $<TARGET_FILE:io_ddscat>
					--dll-no-default-locations
				WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
				)
			register_test(io-ddscat6-2 "Testing/IO")
			# TODO: Add DDSCAT 6 silo tests?
			if (BUILD_PLUGIN_SILO)
				add_custom_target(io-ddscat7-to-silo-1 # Write a file into the SILO visualization format
					COMMAND icedb-3d_structures
						--in ${CMAKE_BINARY_DIR}/test/ddscat7ice.h5
						--db-path "shape/ddscat7ice.dat-particle-1"
						--out-format silo --out ${CMAKE_BINARY_DIR}/test/ddscat7ice.silo
						--dll-load-onelevel $<TARGET_FILE:io_ddscat>
						--dll-load-onelevel $<TARGET_FILE:plugin_silo>
						--dll-no-default-locations
					WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
					)
				register_test(io-ddscat7-to-silo-1 "Testing/IO")
				add_custom_target(io-ddscat7-to-silo-2 # Write a file into the SILO visualization format
					COMMAND icedb-3d_structures
						--in ${CMAKE_BINARY_DIR}/test/ddscat7melting.h5
						--db-path "shape/ddscat7melting.dat-particle-1"
						--out-format silo --out ${CMAKE_BINARY_DIR}/test/ddscat7melting.silo
						--dll-load-onelevel $<TARGET_FILE:io_ddscat>
						--dll-load-onelevel $<TARGET_FILE:plugin_silo>
						--dll-no-default-locations
					WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
					)
				register_test(io-ddscat7-to-silo-2 "Testing/IO")
				add_custom_target(io-ddscat7-to-silo-3 # Write a file into the SILO visualization format
					COMMAND icedb-3d_structures
						--in ${CMAKE_BINARY_DIR}/test/1mm5shape.h5
						--db-path "shape/1mm5shape.txt-particle-1"
						--out-format silo --out ${CMAKE_BINARY_DIR}/test/1mm5shape.silo
						--dll-load-onelevel $<TARGET_FILE:io_ddscat>
						--dll-load-onelevel $<TARGET_FILE:plugin_silo>
						--dll-no-default-locations
					WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
					)
				register_test(io-ddscat7-to-silo-3 "Testing/IO")
			endif()
			if (DO_NCDUMP_TESTS)
				add_custom_target(io-ddscat7-ncdump
					COMMAND ncdump -h ${CMAKE_BINARY_DIR}/test/ddscat7ice.h5)
				register_test(io-ddscat7-ncdump "Testing/IO")
			endif()
		endif()

		if(BUILD_PLUGIN_SILO)
			# Attempt to load and unload only the silo plugin, and check if there is an error.
			add_custom_target(io-silo-plugin-test
				COMMAND icedb-version -- --dll-no-default-locations
				--dll-load-onelevel $<TARGET_FILE:plugin_silo>
				WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
			register_test(io-silo-plugin-test "Testing/IO")
		endif()

endif()

