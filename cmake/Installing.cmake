
# Fix for the libexec install directory location on Debian and Ubuntu - they do not use /usr/libexec.
# Placing under /usr/lib per the Filesystem Hierarchy Standard (http://www.pathname.com/fhs/).
# Note: Debian and Ubuntu use FHS 2.3, and Fedora uses 3.0 (which re-adds libexec).
#message("${CMAKE_INSTALL_LIBDIR} ${CMAKE_INSTALL_LIBEXECDIR} ${CMAKE_INSTALL_BINDIR}")
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	find_program(LSB_RELEASE_EXEC lsb_release)
	if(NOT LSB_RELEASE_EXEC)
		message(FATAL_ERROR "lsb_release not found! On Debian and Ubuntu-based systems, you need to install the lsb-release package. On RHEL, Fedora and CentOS-based systems, you need to install the redhat-lsb-core package.")
	endif()
	mark_as_advanced(LSB_RELEASE_EXEC)
	execute_process(COMMAND ${LSB_RELEASE_EXEC} -is
		OUTPUT_VARIABLE LSB_DISTRIBUTION_NAME_SHORT
		OUTPUT_STRIP_TRAILING_WHITESPACE
		)
	execute_process(COMMAND ${LSB_RELEASE_EXEC} -rs
		OUTPUT_VARIABLE LSB_RELEASE_ID_SHORT
		OUTPUT_STRIP_TRAILING_WHITESPACE
		)
	if("${LSB_DISTRIBUTION_NAME_SHORT}" STREQUAL "Ubuntu" OR "${LSB_DISTRIBUTION_NAME_SHORT}" STREQUAL "Debian")
		set(CMAKE_INSTALL_LIBEXECDIR ${CMAKE_INSTALL_LIBDIR})
		set(CMAKE_INSTALL_FULL_LIBEXECDIR ${CMAKE_INSTALL_FULL_LIBDIR})
	endif()
endif()


list(APPEND ICEDB_TARGETS icedb icedb_base GSL HH BinaryIO BetterThrow Eigen miniball quickhull )

export(TARGETS ${ICEDB_TARGETS} 
		FILE "${PROJECT_BINARY_DIR}/icedbTargets.cmake")
export(PACKAGE icedb)


# CMake find_package script creator stuff # TODO: Put this in a separate script
set(pkgname "icedb")
set(pkglibs icedb)
set(pkgversion ${icedb_VERSION})
# ... for the build tree
set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/lib")
configure_file(cmake/Pkg_Config.cmake.in
	"${PROJECT_BINARY_DIR}/icedbConfig.cmake" @ONLY)
# ... and for the install tree
set(CONF_INCLUDE_DIRS "\${icedb_CMAKE_DIR}/${REL_INCLUDE_DIR}") # Before WiX
configure_file(cmake/Pkg_Config.cmake.in
	"${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/icedbConfig.cmake" @ONLY)
# ... for both
configure_file(cmake/Pkg_ConfigVersion.cmake.in
	"${PROJECT_BINARY_DIR}/icedbConfigVersion.cmake" @ONLY)

INSTALL(FILES "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/icedbConfig.cmake"
	"${PROJECT_BINARY_DIR}/icedbConfigVersion.cmake"
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/icedb COMPONENT Libraries)

INSTALL(EXPORT icedbTargets
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/icedb COMPONENT Libraries)

INSTALL(FILES LICENSE.txt README.md
	DESTINATION ${CMAKE_INSTALL_DOCDIR}
	)
INSTALL(DIRECTORY share/icedb/examples
	DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icedb
	COMPONENT Examples
	)

