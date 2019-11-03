

# CPack stuff
set(CPACK_PACKAGE_NAME "icedb")
set(CPACK_PACKAGE_VENDOR "Ryan Honeyager")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "icedb")
set(CPACK_PACKAGE_VERSION "${icedb_VERSION}")
set(CPACK_PACKAGE_VERSION_MAJOR "${MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${REVISION}")
set(CPACK_DEBIAN_PACKAGE_RELEASE "1")
set(CPACK_RPM_PACKAGE_RELEASE "1")

SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Ryan Honeyager")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/docs/README-package.txt")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "icedb")
set(CPACK_PACKAGE_CONTACT "Ryan Honeyager (ryan@honeyager.info)")
set(CPACK_WIX_UPGRADE_GUID "A29C49DA-8C87-4B15-AC54-D163E5AC3C29")
set(CPACK_WIX_PRODUCT_GUID "07770F44-F916-4A21-9788-CA03F71355D7")
set(CPACK_WIX_PRODUCT_ICON "${CMAKE_CURRENT_SOURCE_DIR}/share/icons\\\\favicon.ico")
set(CPACK_WIX_CMAKE_PACKAGE_REGISTRY "icedb")
set(CPACK_WIX_PROPERTY_ARPURLINFOABOUT "https://rhoneyager.github.io/icedb")
if (NOT CPACK_SYSTEM_NAME)
	set(CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_PROCESSOR}")
	if (CPACK_SYSTEM_NAME STREQUAL "x86_64")
		set(CPACK_SYSTEM_NAME "amd64")
	endif ()
endif ()
if (CPACK_SYSTEM_NAME STREQUAL "AMD64")
	set(CPACK_SYSTEM_NAME "amd64")
endif ()
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	set(CPACK_SYSTEM_NAME "${CPACK_SYSTEM_NAME}-${LSB_DISTRIBUTION_NAME_SHORT}-${LSB_RELEASE_ID_SHORT}")
endif()
IF(WIN32 AND NOT UNIX)
	set(CPACK_SYSTEM_NAME "${CPACK_SYSTEM_NAME}-Windows")
endif()
option(PACKAGE_CI_BUILD "Is this a testing (continuous integration) build?" OFF)
#set(PACKAGE_GIT_BRANCH "${GITBRANCH}" CACHE STRING "What is the branch of the code used to produce this build?")
string(TIMESTAMP PACKAGE_TIMESTAMP "%Y%m%d")
if(PACKAGE_CI_BUILD)
	SET(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_SYSTEM_NAME}-${PACKAGE_GIT_BRANCH}-${CMAKE_BUILD_TYPE}-${PACKAGE_TIMESTAMP}")
endif()
#else()
#	SET(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_SYSTEM_NAME}-${CMAKE_BUILD_TYPE}")
#endif()
IF(WIN32 AND NOT UNIX)
	# We want to find the VC2017 redistributables, package them in the installer, and 
	# invoke them on install.
	# Path looks like C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Redist\MSVC\14.15.26706
	find_path(MSVC_PF_DIR "Microsoft Visual Studio"
		HINTS "C:/Program Files" "C:/Program Files (x86)"
		)
	set(MSVC_DIR "${MSVC_PF_DIR}/Microsoft Visual Studio")
	find_path(MSVC_VC_DIR_BASE VC
		HINTS "${MSVC_DIR}"
		PATH_SUFFIXES 2017/Enterprise 2017/Professional 2017/Community 
			2015/Enterprise 2015/Professional 2015/Community
		)
	set (MSVC_VC_REDIST_MSVC_DIR "${MSVC_VC_DIR_BASE}/VC/Redist/MSVC")
	file(GLOB MSVC_VC_REDISTS LIST_DIRECTORIES TRUE "${MSVC_VC_REDIST_MSVC_DIR}/*")
	find_program(MSVC_REDIST_EXE vcredist_x64.exe
		HINTS ${MSVC_VC_REDISTS})
	mark_as_advanced(MSVC_REDIST_EXE MSVC_PF_DIR MSVC_VC_DIR_BASE)
	#install(PROGRAMS ${MSVC_REDIST_EXE} DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Libraries)
	#message("The MSVC redistributables are at ${MSVC_REDIST_EXE}")
	#list ( APPEND CPACK_NSIS_EXTRA_INSTALL_COMMANDS " ExecWait './bin/vcredist_x64.exe /quiet'")
	# TODO: Optionally met the installers at
	#  https://aka.ms/vs/16/release/VC_redist.x64.exe
	#  https://aka.ms/vs/15/release/VC_redist.x64.exe

	set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}\\\\LICENSE.txt")
	
	# There is a bug in NSIS that does not handle full unix paths properly. Make
	# sure there is at least one set of four (4) backslashes.
	SET(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/share/icons\\\\favicon-96x96.png")
	SET(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/share/icons\\\\favicon.ico")
	SET(CPACK_NSYS_MUI_UNIICON "${CMAKE_SOURCE_DIR}/share/icons\\\\favicon.ico")

#	SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\MyExecutable.exe")
	SET(CPACK_NSIS_DISPLAY_NAME "icedb")
	SET(CPACK_NSIS_HELP_LINK "https:\\\\\\\\rhoneyager.github.io/libicedb/")
	SET(CPACK_NSIS_URL_INFO_ABOUT "https:\\\\\\\\rhoneyager.github.io/libicedb/")
	SET(CPACK_NSIS_CONTACT "ryan@honeyager.info")
	SET(CPACK_NSIS_MODIFY_PATH ON)
ELSE(WIN32 AND NOT UNIX)
	set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE.txt")
	SET(CPACK_STRIP_FILES FALSE)
	SET(CPACK_SOURCE_STRIP_FILES FALSE)
ENDIF(WIN32 AND NOT UNIX)
#SET(CPACK_PACKAGE_EXECUTABLES "MyExecutable" "My Executable")

set (CPACK_COMPONENTS_ALL 
	Applications
	Documentation
	Examples
	Libraries 
	Headers
	Plugins
)

set(CPACK_COMPONENT_APPLICATIONS_DESCRIPTION 
	"All of the apps")
set(CPACK_COMPONENT_DOCUMENTATION_DESCRIPTION 
	"Doxygen HTML docs")
set(CPACK_COMPONENT_EXAMPLES_DESCRIPTION 
	"Basic example files")
set(CPACK_COMPONENT_LIBRARIES_DESCRIPTION 
	"The compiled libraries")
set(CPACK_COMPONENT_HEADERS_DESCRIPTION 
	"Headers for code development")
set(CPACK_COMPONENT_PLUGINS_DESCRIPTION 
	"All of the plugins")

set(CPACK_COMPONENT_HEADERS_DEPENDS Libraries)
set(CPACK_COMPONENT_EXAMPLES_DEPENDS Libraries Applications Headers)
set(CPACK_COMPONENT_APPLICATIONS_DEPENDS Libraries)
set(CPACK_COMPONENT_PLUGINS_DEPENDS Libraries)

set(CPACK_COMPONENT_LIBRARIES_REQUIRED 1)

set(CPACK_RPM_PACKAGE_REQUIRES 
	"cmake >= 3.1, hdf5-devel, hdf5, git, zlib-devel, boost-devel >= 1.48, gcc-c++ >= 6"
	)

set(CPACK_DEBIAN_PACKAGE_DEPENDS
	"cmake (>= 3.1), libhdf5-dev, zlib1g-dev, libboost-all-dev (>= 1.48)"
	)
if(BUILD_PLUGIN_SILO)
	set(CPACK_DEBIAN_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS}, libsilo-dev")
endif()
set(CPACK_DEBIAN_PACKAGE_RECOMMENDS "g++ (>= 6), hdf5-tools, git, doxygen, graphviz")
set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")

# This must always be last!
include(CPack)

