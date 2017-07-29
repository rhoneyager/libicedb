generaterc("germany_api_test_fs" "exe"
	"germany_api"
	"Scattering utility library"
	 "${CMAKE_CURRENT_BINARY_DIR}/app.rc"
	 )

set(srcs fs-main.cpp)
IF (WIN32 AND NOT CYGWIN)
SET(srcs ${srcs} 
	"${CMAKE_CURRENT_BINARY_DIR}/app.rc")
ENDIF()

add_executable(germany_api_test_fs ${srcs})
target_link_libraries(germany_api_test_fs libicedb)
IF (WIN32 AND NOT CYGWIN)
add_dependencies(germany_api_test_fs fs-win)
else()
add_dependencies(germany_api_test_fs fs-posix)
ENDIF()
add_dependencies(germany_api_test_fs netcdf_impl)
addapp(germany_api_test_fs Testing)
storebin(germany_api_test_fs)

#add_library(testdll SHARED testdll.cpp)
#storebin(testdll)

