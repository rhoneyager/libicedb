if (WIN32 AND NOT CYGWIN)
else()
generaterc("fs-posix" "dll"
	"libicedb"
	"Posix filesystem plugin"
	 "${CMAKE_CURRENT_BINARY_DIR}/app.rc"
	 )
	 set (tsrcs fs_posix.cpp fs_posix.hpp paths.cpp)
IF (WIN32 AND NOT CYGWIN)
SET(tsrcs ${tsrcs} 
	"${CMAKE_CURRENT_BINARY_DIR}/app.rc")
ENDIF()


add_library(fs-posix SHARED ${tsrcs})
addplugin(fs-posix Filesystem icedb)
storeplugin(fs-posix icedb)

endif(WIN32 AND NOT CYGWIN)
