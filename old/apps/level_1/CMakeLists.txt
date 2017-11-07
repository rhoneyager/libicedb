generaterc("shapes-setattr" "exe"
	"germany_api"
	"Example program demonstrating how to manipulate shape attributes"
	 "${CMAKE_CURRENT_BINARY_DIR}/app-attr.rc"
	 )

set(srcs-attr shapes-setattr-app.cpp)
IF (WIN32 AND NOT CYGWIN)
SET(srcs-attr ${srcs-attr} 
	"${CMAKE_CURRENT_BINARY_DIR}/app-attr.rc")
ENDIF()

if(WIN32 AND NOT CYGWIN)
	set(libs libicedb)
else()
	set (libs libicedb
		${Boost_PROGRAM_OPTIONS_LIBRARY})
endif()

add_executable(shapes-setattr ${srcs-attr})
target_link_libraries(shapes-setattr ${libs})
addapp(shapes-setattr Level_1)
storebin(shapes-setattr)



generaterc("shapes-settbl" "exe"
	"germany_api"
	"Example program demonstrating how to manipulate shape tables"
	 "${CMAKE_CURRENT_BINARY_DIR}/app-tbl.rc"
	 )

set(srcs-tbl shapes-settbl-app.cpp)
IF (WIN32 AND NOT CYGWIN)
SET(srcs-tbl ${srcs-tbl} 
	"${CMAKE_CURRENT_BINARY_DIR}/app-tbl.rc")
ENDIF()

add_executable(shapes-settbl ${srcs-tbl})
target_link_libraries(shapes-settbl ${libs})
addapp(shapes-settbl Level_1)
storebin(shapes-settbl)

