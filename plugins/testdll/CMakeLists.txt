generaterc("plugin-testdll" "dll"
	"libicedb"
	"Test dll"
	 "${CMAKE_CURRENT_BINARY_DIR}/app.rc"
	 )
	 set (tsrcs testdll.cpp)
IF (WIN32 AND NOT CYGWIN)
SET(tsrcs ${tsrcs} 
	"${CMAKE_CURRENT_BINARY_DIR}/app.rc")
ENDIF()


add_library(testdll SHARED ${tsrcs})
#target_link_libraries(testdll libicedb)
addplugin(testdll Testing icedb)
storeplugin(testdll icedb)
