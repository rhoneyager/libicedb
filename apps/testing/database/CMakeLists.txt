generaterc("database" "exe"
	"icedb"
	"libicedb example database application"
	 "${CMAKE_CURRENT_BINARY_DIR}/app.rc"
	 )
set(srcs fs-test.cpp)
IF (WIN32 AND NOT CYGWIN)
SET(srcs ${srcs} 
	"${CMAKE_CURRENT_BINARY_DIR}/app.rc")
ENDIF()
add_executable(database ${srcs})
set_target_properties(database PROPERTIES CXX_STANDARD 17)
target_include_directories(database SYSTEM PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(database SYSTEM
	PRIVATE ${HDF5_INCLUDE_DIRS} ${HDF5_INCLUDE_DIR_CPP}
	)
target_compile_definitions(database
	PRIVATE ${HDF5_DEFINITIONS})
target_link_libraries(database libicedb ${Boost_PROGRAM_OPTIONS_LIBRARY}) # ${HDF5_LIBRARIES})

addapp(database Testing)
storebin(database)
