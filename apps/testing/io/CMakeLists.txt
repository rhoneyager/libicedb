include(addapp)

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
target_link_libraries(icedb-test-psu-shapes icedb Boost::unit_test_framework)
if (BUILD_PLUGIN_SILO)
	add_dependencies(icedb-test-psu-shapes plugin_silo)
	target_compile_definitions(icedb-test-psu-shapes PRIVATE TEST_WITH_SILO=1)
else()
	target_compile_definitions(icedb-test-psu-shapes PRIVATE TEST_WITH_SILO=0)
endif()
addapp(icedb-test-psu-shapes Testing/IO)
storebin(icedb-test-psu-shapes)
