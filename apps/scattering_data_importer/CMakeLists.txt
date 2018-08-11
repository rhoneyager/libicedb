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
target_include_directories(icedb-scattering_data_importer SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/lib)
target_include_directories(icedb-scattering_data_importer SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/lib/icedb/compat)
target_include_directories(icedb-scattering_data_importer SYSTEM PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(icedb-scattering_data_importer SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/lib/related/eigen)

# TODO: remove the HDF5 dependency here.
target_include_directories(icedb-scattering_data_importer SYSTEM
	PRIVATE ${HDF5_INCLUDE_DIRS} ${HDF5_INCLUDE_DIR_CPP}
	)
target_compile_definitions(icedb-scattering_data_importer
	PRIVATE ${HDF5_DEFINITIONS})

target_link_libraries(icedb-scattering_data_importer icedb io_ddscat ${Boost_REGEX_LIBRARY} ${Boost_PROGRAM_OPTIONS_LIBRARY} ${Boost_DATE_TIME_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})

addapp(icedb-scattering_data_importer Level_0)
storebin(icedb-scattering_data_importer)
