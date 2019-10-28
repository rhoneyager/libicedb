include(addapp)

# Documentation for Windows Explorer
generaterc("icedb-database" "exe"
	"icedb"
	"libicedb example icedb-database application"
	 "${CMAKE_CURRENT_BINARY_DIR}/app.rc"
	 )
# Only one source file
set(srcs fs-test.cpp)
IF (WIN32 AND NOT CYGWIN)
SET(srcs ${srcs} 
	"${CMAKE_CURRENT_BINARY_DIR}/app.rc")
ENDIF()

add_executable(icedb-database ${srcs})
target_include_directories(icedb-database SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/lib)
# Using Boost's program options library for command line parsing.
target_include_directories(icedb-database SYSTEM PRIVATE ${Boost_INCLUDE_DIRS})
#target_include_directories(icedb-database SYSTEM
#	PRIVATE ${HDF5_INCLUDE_DIRS} ${HDF5_INCLUDE_DIR_CPP}
#	)
#target_compile_definitions(icedb-database
#	PRIVATE ${HDF5_DEFINITIONS})
target_link_libraries(icedb-database icedb ${Boost_PROGRAM_OPTIONS_LIBRARY}) # ${HDF5_LIBRARIES})

addapp_test(icedb-database Testing)
storebin(icedb-database)
