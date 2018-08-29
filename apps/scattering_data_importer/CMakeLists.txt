include(addapp)

# Documents the app in Windows Explorer
generaterc("icedb-scattering_data_importer" "exe"
	"icedb"
	"importer program to read and write phase functions database"
	 "${CMAKE_CURRENT_BINARY_DIR}/app.rc"
	 )
# The source files
set(srcs 
	sd-main.cpp
	README.md)
IF (WIN32 AND NOT CYGWIN)
SET(srcs ${srcs} 
	"${CMAKE_CURRENT_BINARY_DIR}/app.rc")
ENDIF()

add_executable(icedb-scattering_data_importer ${srcs})
target_link_libraries(icedb-scattering_data_importer icedb)

if (BUILD_PLUGIN_ADDA)
	add_dependencies(icedb-scattering_data_importer io_adda)
endif()
if (BUILD_PLUGIN_DDSCAT)
	add_dependencies(icedb-scattering_data_importer io_ddscat)
endif()
if (BUILD_PLUGIN_PSU)
	add_dependencies(icedb-scattering_data_importer io_psu)
endif()

addapp(icedb-scattering_data_importer Level_0)
storebin(icedb-scattering_data_importer)
