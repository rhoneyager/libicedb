include(addapp)
# Documents the app in Windows Explorer
generaterc("icedb-refract" "exe"
	"icedb"
	"Program to determine refractive indices"
	 "${CMAKE_CURRENT_BINARY_DIR}/app.rc"
	 )
# The source files
set(srcs main.cpp README.md)
IF (WIN32 AND NOT CYGWIN)
SET(srcs ${srcs} 
	"${CMAKE_CURRENT_BINARY_DIR}/app.rc")
ENDIF()

add_executable(icedb-refract ${srcs})
target_link_libraries(icedb-refract icedb)

addapp(icedb-refract Utility)
storebin(icedb-refract)

