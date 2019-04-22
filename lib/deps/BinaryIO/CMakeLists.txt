include(addlib)
set(DIRS_bIO
	include/BinaryIO/Buffers.hpp
	include/BinaryIO/Files.hpp
	include/BinaryIO/defs.hpp
	_bio_build.cpp
	)
add_library(BinaryIO STATIC ${DIRS_bIO})
add_library(
    BinaryIO::BinaryIO ALIAS BinaryIO )
add_library(
    icedb::BinaryIO ALIAS BinaryIO )

# add definitions to the library and targets that consume it
target_compile_definitions(BinaryIO PUBLIC
    $<$<CXX_COMPILER_ID:MSVC>:
        # remove unnecessary warnings about unchecked iterators
        _SCL_SECURE_NO_WARNINGS
		_CRT_SECURE_NO_WARNINGS
        # remove deprecation warnings about std::uncaught_exception() (from catch)
        _SILENCE_CXX17_UNCAUGHT_EXCEPTION_DEPRECATION_WARNING
    >
	)
# add include folders to the library and targets that consume it
# the SYSTEM keyword suppresses warnings for users of the library
    target_include_directories(BinaryIO SYSTEM PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${INSTALL_INCLUDE_DIR}>
    )
addlib(BinaryIO STATIC)
INSTALL(DIRECTORY include/BinaryIO
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/icedb-${icedb_VERSION}
	COMPONENT Headers)
