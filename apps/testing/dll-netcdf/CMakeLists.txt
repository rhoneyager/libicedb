generaterc("germany_api_test_dll-netcdf" "exe"
	"germany_api"
	"Scattering utility library"
	 "${CMAKE_CURRENT_BINARY_DIR}/app.rc"
	 )

set(srcs main.cpp)
IF (WIN32 AND NOT CYGWIN)
SET(srcs ${srcs} 
	"${CMAKE_CURRENT_BINARY_DIR}/app.rc")
ENDIF()

add_executable(germany_api_test_dll-netcdf ${srcs})
target_link_libraries(germany_api_test_dll-netcdf libicedb)
add_dependencies(germany_api_test_dll-netcdf netcdf_impl)
addapp(germany_api_test_dll-netcdf Testing)
storebin(germany_api_test_dll-netcdf)
