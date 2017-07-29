generaterc("netcdf_impl" "dll"
	"libicedb"
	"netCDF interface plugin"
	 "${CMAKE_CURRENT_BINARY_DIR}/app.rc"
	 )
set (tsrcs 
	paths.cpp
	plugin_netcdf.cpp
	plugin_netcdf.hpp
	plugin_netcdf_impl.hpp
	plugin_netcdf_interface.h
	)
IF (WIN32 AND NOT CYGWIN)
SET(tsrcs ${tsrcs} 
	"${CMAKE_CURRENT_BINARY_DIR}/app.rc")
ENDIF()


add_library(netcdf_impl SHARED ${tsrcs})
#target_link_libraries(testdll libicedb)
addplugin(netcdf_impl Filesystem icedb)
storeplugin(netcdf_impl icedb)

