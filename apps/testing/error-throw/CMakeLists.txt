generaterc("germany_api_test_error-throw" "exe"
	"germany_api"
	"Scattering utility library"
	 "${CMAKE_CURRENT_BINARY_DIR}/app.rc"
	 )

set(srcs main.cpp)
IF (WIN32 AND NOT CYGWIN)
SET(srcs ${srcs} 
	"${CMAKE_CURRENT_BINARY_DIR}/app.rc")
ENDIF()

add_executable(germany_api_test_error-throw ${srcs})
target_link_libraries(germany_api_test_error-throw germany_api)
storebin(germany_api_test_error-throw)
