include(addapp)

# Documents the app in Windows Explorer
generaterc("icedb-3d_structures_importer" "exe"
	"icedb"
	"importer program to read and write icedb-3d structures to a database"
	 "${CMAKE_CURRENT_BINARY_DIR}/app.rc"
	 )
# The source files
set(srcs shapes-main.cpp
	#shape.hpp
	#shapeIOtext.cpp
	#shapeIOtext.hpp
	#shapeIOtextParsers2.cpp # This has my custom text reader.
	#shapeIOpsu.cpp
	README.md)
IF (WIN32 AND NOT CYGWIN)
SET(srcs ${srcs} 
	"${CMAKE_CURRENT_BINARY_DIR}/app.rc")
ENDIF()

add_executable(icedb-3d_structures_importer ${srcs})
target_link_libraries(icedb-3d_structures_importer icedb)

if (BUILD_PLUGIN_ADDA)
	add_dependencies(icedb-3d_structures_importer io_adda)
endif()
if (BUILD_PLUGIN_DDSCAT)
	add_dependencies(icedb-3d_structures_importer io_ddscat)
endif()
if (BUILD_PLUGIN_PSU)
	add_dependencies(icedb-3d_structures_importer io_psu)
endif()

addapp(icedb-3d_structures_importer Level_0)
storebin(icedb-3d_structures_importer)
