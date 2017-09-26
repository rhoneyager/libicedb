generaterc("shapes" "exe"
	"germany_api"
	"Program to read and manipulate shapes"
	 "${CMAKE_CURRENT_BINARY_DIR}/app.rc"
	 )

set(srcs shapes-app.cpp)
IF (WIN32 AND NOT CYGWIN)
SET(srcs ${srcs} 
	"${CMAKE_CURRENT_BINARY_DIR}/app.rc")
ENDIF()

if(WIN32 AND NOT CYGWIN)
	set(libs libicedb)
else()
	set (libs libicedb
		${Boost_PROGRAM_OPTIONS_LIBRARY})
endif()

add_executable(shapes ${srcs})
target_link_libraries(shapes ${libs})
#add_dependencies(shapes units-simple)
addapp(shapes Level_0)
storebin(shapes)

