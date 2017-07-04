generaterc("germany_api_test_os-functions" "exe"
	"germany_api"
	"Scattering utility library"
	 "${CMAKE_CURRENT_BINARY_DIR}/app.rc"
	 )

set(srcs test-os-functions.cpp)
IF (WIN32 AND NOT CYGWIN)
SET(srcs ${srcs} 
	"${CMAKE_CURRENT_BINARY_DIR}/app.rc")
ENDIF()

add_executable(germany_api_test_os-functions ${srcs})
target_link_libraries(germany_api_test_os-functions germany_api)
storebin(germany_api_test_os-functions)