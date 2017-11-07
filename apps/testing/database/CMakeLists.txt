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
target_include_directories(database SYSTEM PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(database libicedb ${Boost_PROGRAM_OPTIONS_LIBRARY} ${h5libs})

addapp(database Testing)
storebin(database)
