generaterc("libicedb_test_error-throw" "exe"
	"libicedb"
	"Scattering utility library"
	 "${CMAKE_CURRENT_BINARY_DIR}/app.rc"
	 )

set(srcs main.cpp)
IF (WIN32 AND NOT CYGWIN)
SET(srcs ${srcs} 
	"${CMAKE_CURRENT_BINARY_DIR}/app.rc")
ENDIF()

add_executable(libicedb_test_error-throw ${srcs})
target_link_libraries(libicedb_test_error-throw libicedb)
addapp(libicedb_test_error-throw Testing)
storebin(libicedb_test_error-throw)
