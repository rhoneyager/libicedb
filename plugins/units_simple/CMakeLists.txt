generaterc("units-simple" "dll"
	"germany_api"
	"Simple units implementation"
	 "${CMAKE_CURRENT_BINARY_DIR}/app.rc"
	 )
	 set (tsrcs units_simple.cpp)
IF (WIN32 AND NOT CYGWIN)
SET(tsrcs ${tsrcs} 
	"${CMAKE_CURRENT_BINARY_DIR}/app.rc")
ENDIF()


add_library(units-simple SHARED ${tsrcs})
#target_link_libraries(testdll germany_api)
addplugin(units-simple Testing icedb)
storeplugin(units-simple icedb)
