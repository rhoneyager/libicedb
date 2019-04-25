set(DIRS_BT
	../include/BetterThrow/defs.hpp
	../include/BetterThrow/Error.hpp
	../include/BetterThrow/Info.hpp
	../README.md
	os.cpp
	os-posix.hpp
	os-win.hpp
	os-posix.cpp
	os-win.cpp
	)
add_library(BetterThrow STATIC ${DIRS_BT})
add_library(
    BetterThrow::BetterThrow ALIAS BetterThrow )
add_library(
    icedb::BetterThrow ALIAS BetterThrow )
# Based on os, mark either os-win.cpp or os-posix.cpp as a header file (so no compilation).
addlib(BetterThrow STATIC)
IF(WIN32 AND NOT UNIX)
	set_source_files_properties(os-posix.cpp PROPERTIES HEADER_FILE_ONLY TRUE)
else()
	set_source_files_properties(os-win.cpp PROPERTIES HEADER_FILE_ONLY TRUE)
endif()
if("${CMAKE_HOST_SYSTEM_NAME}" MATCHES "Linux")
	target_link_libraries(BetterThrow INTERFACE dl)
elseif("${CMAKE_HOST_SYSTEM_NAME}" MATCHES "FreeBSD")
elseif("${CMAKE_HOST_SYSTEM_NAME}" MATCHES "Darwin")
elseif("${CMAKE_HOST_SYSTEM_NAME}" MATCHES "Windows")
else()
	message("Unhandled system name: ${CMAKE_HOST_SYSTEM_NAME}. Build system should be expanded to handle this case.")
endif()

target_compile_definitions(BetterThrow PUBLIC
    $<$<CXX_COMPILER_ID:MSVC>:
        # remove unnecessary warnings about unchecked iterators
        _SCL_SECURE_NO_WARNINGS
		_CRT_SECURE_NO_WARNINGS
        # remove deprecation warnings about std::uncaught_exception() (from catch)
        _SILENCE_CXX17_UNCAUGHT_EXCEPTION_DEPRECATION_WARNING
		# Use unicode
		UNICODE
		_UNICODE
    >
	)
# add include folders to the library and targets that consume it
# the SYSTEM keyword suppresses warnings for users of the library
if(BT_STANDALONE_PROJECT)
    target_include_directories(BetterThrow INTERFACE
        $<BUILD_INTERFACE:
            ${CMAKE_CURRENT_SOURCE_DIR}/../include
        >
    )
else()
    target_include_directories(BetterThrow SYSTEM INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
		$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )
endif()
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../include
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/icedb-${icedb_VERSION}
	COMPONENT Headers)
