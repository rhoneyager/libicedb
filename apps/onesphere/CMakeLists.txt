include(addapp)
# Documents the app in Windows Explorer
generaterc("icedb-onesphere" "exe"
	"icedb"
	"Program to determine scattering by a single sphere"
	 "${CMAKE_CURRENT_BINARY_DIR}/app.rc"
	 )
# The source files
set(srcs main.cpp)
IF (WIN32 AND NOT CYGWIN)
SET(srcs ${srcs} 
	"${CMAKE_CURRENT_BINARY_DIR}/app.rc")
ENDIF()

add_executable(icedb-onesphere ${srcs})
target_link_libraries(icedb-onesphere icedb)

addapp(icedb-onesphere Scattering)
storebin(icedb-onesphere)

