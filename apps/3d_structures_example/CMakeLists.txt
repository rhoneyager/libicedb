include(addapp)

# Documents the app in Windows Explorer
generaterc("icedb-3d_structures_example" "exe"
	"icedb"
	"Example program to read and write icedb-3d structures to a database"
	 "${CMAKE_CURRENT_BINARY_DIR}/app.rc"
	 )
# The source files
set(srcs shapes-main.cpp
	shape.hpp
	shapeIOtext.cpp
	shapeIOtext.hpp
	shapeIOtextParsers2.cpp # This has my custon text reader.
	README.md)
IF (WIN32 AND NOT CYGWIN)
SET(srcs ${srcs} 
	"${CMAKE_CURRENT_BINARY_DIR}/app.rc")
ENDIF()

add_executable(icedb-3d_structures_example ${srcs})
target_include_directories(icedb-3d_structures_example SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/lib)
target_include_directories(icedb-3d_structures_example SYSTEM PRIVATE ${Boost_INCLUDE_DIRS})
# TODO: remove the HDF5 dependency here.
target_include_directories(icedb-3d_structures_example SYSTEM
	PRIVATE ${HDF5_INCLUDE_DIRS} ${HDF5_INCLUDE_DIR_CPP}
	)
target_compile_definitions(icedb-3d_structures_example
	PRIVATE ${HDF5_DEFINITIONS})

target_link_libraries(icedb-3d_structures_example icedb ${Boost_PROGRAM_OPTIONS_LIBRARY})

addapp(icedb-3d_structures_example Level_0)
storebin(icedb-3d_structures_example)
