generaterc("3d_structures_beta" "exe"
	"icedb"
	"Program to read and write 3d structures to and from a database"
	 "${CMAKE_CURRENT_BINARY_DIR}/app.rc"
	 )
set(srcs shapes-main.cpp
	shape.hpp
	shapeIOtext.cpp
	shapeIOtext.hpp
	#shapeIOtextParsers.cpp
	shapeIOtextParsers2.cpp
	README.md)
IF (WIN32 AND NOT CYGWIN)
SET(srcs ${srcs} 
	"${CMAKE_CURRENT_BINARY_DIR}/app.rc")
ENDIF()
add_executable(3d_structures_beta ${srcs})
target_include_directories(3d_structures_beta SYSTEM PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(3d_structures_beta SYSTEM PRIVATE ${CMAKE_SOURCE_DIR})
target_include_directories(3d_structures_beta SYSTEM
	PRIVATE ${HDF5_INCLUDE_DIRS} ${HDF5_INCLUDE_DIR_CPP}
	)
target_compile_definitions(3d_structures_beta
	PRIVATE ${HDF5_DEFINITIONS})
target_link_libraries(3d_structures_beta libicedb ${Boost_PROGRAM_OPTIONS_LIBRARY}) # ${HDF5_LIBRARIES})

addapp(3d_structures_beta Level_0)
storebin(3d_structures_beta)
