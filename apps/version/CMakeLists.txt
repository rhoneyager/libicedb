include(addapp)

# Documents the app in Windows Explorer
generaterc("icedb-version" "exe"
	"icedb"
	"Prints version information for icedb. Useful for logging and debugging"
	 "${CMAKE_CURRENT_BINARY_DIR}/app.rc"
	 )
# The source files
set(srcs version.cpp)
IF (WIN32 AND NOT CYGWIN)
SET(srcs ${srcs} 
	"${CMAKE_CURRENT_BINARY_DIR}/app.rc")
ENDIF()

add_executable(icedb-version ${srcs})
target_link_libraries(icedb-version icedb)
addapp(icedb-version Debugging)
storebin(icedb-version)
