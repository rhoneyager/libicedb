# The eigen code will eventually be moved here.
include(addlib)
# Import the locally-provided version of Eigen3.

set(DIRS_eigen
	eigen/Eigen/Core
	eigen/Eigen/Dense
	eigen/Eigen/Eigen
	eigen/Eigen/Sparse
	eigen/unsupported/Eigen/CXX11/Tensor
	_build_eigen.cpp
	)
add_library(Eigen STATIC ${DIRS_eigen})
add_library(
    icedb::Eigen3 ALIAS Eigen )
add_library(
    Eigen3::Eigen3 ALIAS Eigen )

# add definitions to the library and targets that consume it
target_compile_definitions(Eigen PUBLIC
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
    target_include_directories(Eigen SYSTEM PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/eigen>
		$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${INSTALL_INCLUDE_DIR}>
    )
addlib(Eigen STATIC)
#INSTALL(DIRECTORY eigen
#	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/icedb-${icedb_VERSION}
#	COMPONENT Headers)
