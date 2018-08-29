include(addapp)

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
	target_link_libraries(icedb-test-psu-shapes icedb io_psu Boost::unit_test_framework)
	if (BUILD_PLUGIN_SILO)
		add_dependencies(icedb-test-psu-shapes plugin_silo)
		target_compile_definitions(icedb-test-psu-shapes PRIVATE TEST_WITH_SILO=1)
	else()
		target_compile_definitions(icedb-test-psu-shapes PRIVATE TEST_WITH_SILO=0)
	endif()
	addapp(icedb-test-psu-shapes Testing/IO)
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
	target_link_libraries(icedb-test-adda-shapes icedb io_adda Boost::unit_test_framework)
	addapp(icedb-test-adda-shapes Testing/IO)
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
	target_link_libraries(icedb-test-ddscat7-shapes icedb io_ddscat Boost::unit_test_framework)
	addapp(icedb-test-ddscat7-shapes Testing/IO)
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
	target_link_libraries(icedb-test-ddscat6-shapes icedb io_ddscat Boost::unit_test_framework)
	addapp(icedb-test-ddscat6-shapes Testing/IO)
	storebin(icedb-test-ddscat6-shapes)
endif()