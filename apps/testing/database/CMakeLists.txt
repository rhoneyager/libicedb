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
target_link_libraries(database libicedb ${Boost_PROGRAM_OPTIONS_LIBRARY})

addapp(database Testing)
storebin(database)
