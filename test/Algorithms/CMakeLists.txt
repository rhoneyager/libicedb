# Documents the app in Windows Explorer
generaterc("icedb-test-algs" "exe"
	"icedb"
	"Test standard algorithm code"
		"${CMAKE_CURRENT_BINARY_DIR}/app.rc"
		)
# The source files
set(srcs-algs test-algs.cpp)
IF (WIN32 AND NOT CYGWIN)
SET(srcs-algs ${srcs-algs} 
	"${CMAKE_CURRENT_BINARY_DIR}/app.rc")
ENDIF()

add_executable(icedb-test-algs ${srcs-algs})
target_link_libraries(icedb-test-algs icedb_testing icedb Boost::unit_test_framework)
	
addapp_test(icedb-test-algs Testing/Algorithms)
storebin(icedb-test-algs)

add_test (NAME alg-tests # Test basic algorithms
	COMMAND icedb-test-algs --
		--dll-no-default-locations
		--share-dir ${CMAKE_SOURCE_DIR}/share
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	)
