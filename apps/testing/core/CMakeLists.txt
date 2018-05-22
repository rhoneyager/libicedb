include(addapp)

# Documents the app in Windows Explorer
generaterc("icedb-testing" "exe"
	"icedb"
	"Program to perform unit tests on the core icedb library"
	 "${CMAKE_CURRENT_BINARY_DIR}/app.rc"
	 )
# The source files
set(srcs
	testing_main.cpp
	globals.cpp
	globals.hpp
	testing_hh_file_io.cpp
	testing_hh_objects.cpp
	testing_hh_tags.cpp
	testing_hh_types.cpp
	#testing_refract.cpp
	#testing_units.cpp
	)

IF (WIN32 AND NOT CYGWIN)
SET(srcs ${srcs} 
	"${CMAKE_CURRENT_BINARY_DIR}/app.rc")
ENDIF()

add_executable(icedb-testing-core ${srcs})
target_include_directories(icedb-testing-core SYSTEM
	PRIVATE ${HDF5_INCLUDE_DIRS} ${HDF5_INCLUDE_DIR_CPP} ${CMAKE_SOURCE_DIR}/lib/icedb/compat)
target_include_directories(icedb-testing-core SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/lib)
target_include_directories(icedb-testing-core SYSTEM PRIVATE ${Boost_INCLUDE_DIRS})
target_compile_definitions(icedb-testing-core
	PRIVATE ${HDF5_DEFINITIONS})

target_link_libraries(icedb-testing-core 
	#icedb 
	${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
	${Boost_PROGRAM_OPTIONS_LIBRARY}
	${Boost_FILESYSTEM_LIBRARY}
	${Boost_SYSTEM_LIBRARY}
	${HDF5_LIBRARIES}
	${HDF5_HL_LIBRARIES}
	)

addapp(icedb-testing-core Testing)
storebin(icedb-testing-core)
