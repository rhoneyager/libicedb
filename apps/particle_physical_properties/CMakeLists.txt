include(addapp)

# Documents the app in Windows Explorer
generaterc("icedb-particle_physical_properties" "exe"
	"icedb"
	"Program to take 3-D shape structures and calculate derived properties"
	 "${CMAKE_CURRENT_BINARY_DIR}/app.rc"
	 )
# The source files
set(srcs ppp.cpp
	README.md)
IF (WIN32 AND NOT CYGWIN)
SET(srcs ${srcs} 
	"${CMAKE_CURRENT_BINARY_DIR}/app.rc")
ENDIF()

add_executable(icedb-particle_physical_properties ${srcs})
target_link_libraries(icedb-particle_physical_properties icedb)
addapp(icedb-particle_physical_properties Level_1)
storebin(icedb-particle_physical_properties)
