include(addlib)
set(DIRS_gsl
	include/gsl/gsl
	include/gsl/gsl.hpp
	_build_gsl.cpp
	)
add_library(GSL STATIC ${DIRS_gsl})
add_library(
    GSL::GSL ALIAS GSL )
add_library(
    icedb::GSL ALIAS GSL )

# add definitions to the library and targets that consume it
target_compile_definitions(GSL PUBLIC
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
    target_include_directories(GSL SYSTEM PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${INSTALL_INCLUDE_DIR}>
    )
addlib(GSL STATIC)
storebin(GSL)
INSTALL(DIRECTORY include/gsl
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/icedb-${icedb_VERSION}
	COMPONENT Headers)
