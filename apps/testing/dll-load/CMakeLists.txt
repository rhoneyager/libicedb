generaterc("germany_api_test_dll-load" "exe"
	"germany_api"
	"Scattering utility library"
	 "${CMAKE_CURRENT_BINARY_DIR}/app.rc"
	 )

set(srcs main.cpp)
IF (WIN32 AND NOT CYGWIN)
SET(srcs ${srcs} 
	"${CMAKE_CURRENT_BINARY_DIR}/app.rc")
ENDIF()

add_executable(germany_api_test_dll-load ${srcs})
target_link_libraries(germany_api_test_dll-load germany_api)
#testdll
addapp(germany_api_test_dll-load Testing)
storebin(germany_api_test_dll-load)

#add_library(testdll SHARED testdll.cpp)
#storebin(testdll)

