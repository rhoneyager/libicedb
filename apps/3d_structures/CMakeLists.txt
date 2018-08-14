include(addapp)

# Documents the app in Windows Explorer
generaterc("icedb-3d_structures" "exe"
	"icedb"
	"importer program to read and write icedb-3d structures to a database"
	 "${CMAKE_CURRENT_BINARY_DIR}/app.rc"
	 )
# The source files
set(srcs 3ds-main.cpp
	README.md)
IF (WIN32 AND NOT CYGWIN)
SET(srcs ${srcs} 
	"${CMAKE_CURRENT_BINARY_DIR}/app.rc")
ENDIF()

add_executable(icedb-3d_structures ${srcs})
target_link_libraries(icedb-3d_structures icedb)
if (BUILD_PLUGIN_SILO)
	add_dependencies(icedb-3d_structures plugin_silo)
endif()
addapp(icedb-3d_structures Level_0)
storebin(icedb-3d_structures)
