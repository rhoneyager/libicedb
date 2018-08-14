include(addapp)
# Documents the app in Windows Explorer
generaterc("icedb-units" "exe"
	"icedb"
	"Program to perform unit conversions"
	 "${CMAKE_CURRENT_BINARY_DIR}/app.rc"
	 )
# The source files
set(srcs main.cpp README.md)
IF (WIN32 AND NOT CYGWIN)
SET(srcs ${srcs} 
	"${CMAKE_CURRENT_BINARY_DIR}/app.rc")
ENDIF()

add_executable(icedb-units ${srcs})
target_link_libraries(icedb-units icedb)

addapp(icedb-units Utility)
storebin(icedb-units)
