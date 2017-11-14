generaterc("3d_structures" "exe"
	"icedb"
	"Program to read and write 3d structures to and from a database"
	 "${CMAKE_CURRENT_BINARY_DIR}/app.rc"
	 )
set(srcs shapes-main.cpp)
IF (WIN32 AND NOT CYGWIN)
SET(srcs ${srcs} 
	"${CMAKE_CURRENT_BINARY_DIR}/app.rc")
ENDIF()
add_executable(3d_structures ${srcs})
set_target_properties(3d_structures PROPERTIES CXX_STANDARD 17)
target_include_directories(3d_structures SYSTEM PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(3d_structures SYSTEM PRIVATE ${CMAKE_SOURCE_DIR})
target_include_directories(3d_structures SYSTEM
	PRIVATE ${HDF5_INCLUDE_DIRS} ${HDF5_INCLUDE_DIR_CPP}
	)
target_compile_definitions(3d_structures
	PRIVATE ${HDF5_DEFINITIONS})
target_link_libraries(3d_structures libicedb ${Boost_PROGRAM_OPTIONS_LIBRARY}) # ${HDF5_LIBRARIES})

addapp(3d_structures Level_0)
storebin(3d_structures)
