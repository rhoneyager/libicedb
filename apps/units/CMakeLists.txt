generaterc("units-app" "exe"
	"germany_api"
	"Program to interconvert units"
	 "${CMAKE_CURRENT_BINARY_DIR}/app.rc"
	 )

set(srcs units-app.cpp)
IF (WIN32 AND NOT CYGWIN)
SET(srcs ${srcs} 
	"${CMAKE_CURRENT_BINARY_DIR}/app.rc")
ENDIF()

add_executable(units-app ${srcs})
target_link_libraries(units-app germany_api)
addapp(units-app Misc)
storebin(units-app)

