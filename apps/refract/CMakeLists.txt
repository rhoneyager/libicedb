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
target_include_directories(icedb-refract SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/lib)
target_include_directories(icedb-refract SYSTEM PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(icedb-refract icedb ${Boost_PROGRAM_OPTIONS_LIBRARY})

addapp(icedb-refract Utility)
storebin(icedb-refract)

