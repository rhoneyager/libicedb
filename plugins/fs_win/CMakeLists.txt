if (WIN32 AND NOT CYGWIN)

generaterc("fs-win" "dll"
	"libicedb"
	"Windows filesystem plugin"
	 "${CMAKE_CURRENT_BINARY_DIR}/app.rc"
	 )
	 set (tsrcs fs_win.cpp fs_win.hpp)
IF (WIN32 AND NOT CYGWIN)
SET(tsrcs ${tsrcs} 
	"${CMAKE_CURRENT_BINARY_DIR}/app.rc")
ENDIF()


add_library(fs-win SHARED ${tsrcs})
#target_link_libraries(testdll libicedb)
addplugin(fs-win Filesystem icedb)
storeplugin(fs-win icedb)

endif(WIN32 AND NOT CYGWIN)
