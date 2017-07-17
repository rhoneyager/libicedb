generaterc("units-simple" "dll"
	"libicedb"
	"Simple units implementation"
	 "${CMAKE_CURRENT_BINARY_DIR}/app.rc"
	 )
	 set (tsrcs units_simple.cpp)
IF (WIN32 AND NOT CYGWIN)
SET(tsrcs ${tsrcs} 
	"${CMAKE_CURRENT_BINARY_DIR}/app.rc")
ENDIF()


add_library(units-simple SHARED ${tsrcs})
#target_link_libraries(testdll libicedb)
addplugin(units-simple Units icedb)
storeplugin(units-simple icedb)
