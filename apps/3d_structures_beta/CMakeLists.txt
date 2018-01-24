include(addapp)

# Documents the app in Windows Explorer
generaterc("3d_structures_beta" "exe"
	"icedb"
	"Program to read and write 3d structures to and from a database"
	 "${CMAKE_CURRENT_BINARY_DIR}/app.rc"
	 )
# The source files
set(srcs shapes-main.cpp
	shape.hpp
	shapeIOtext.cpp
	shapeIOtext.hpp
	#shapeIOtextParsers.cpp # This uses Boost to read the text files.
	shapeIOtextParsers2.cpp # This has my custon text reader.
	README.md)
IF (WIN32 AND NOT CYGWIN)
SET(srcs ${srcs} 
	"${CMAKE_CURRENT_BINARY_DIR}/app.rc")
ENDIF()

add_executable(3d_structures_beta ${srcs})
target_include_directories(3d_structures_beta SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/lib)
target_include_directories(3d_structures_beta SYSTEM PRIVATE ${Boost_INCLUDE_DIRS})
# TODO: remove the HDF5 dependency here.
target_include_directories(3d_structures_beta SYSTEM
	PRIVATE ${HDF5_INCLUDE_DIRS} ${HDF5_INCLUDE_DIR_CPP}
	)
target_compile_definitions(3d_structures_beta
	PRIVATE ${HDF5_DEFINITIONS})

target_link_libraries(3d_structures_beta icedb ${Boost_PROGRAM_OPTIONS_LIBRARY})

addapp(3d_structures_beta Level_0)
storebin(3d_structures_beta)
