# CPack stuff
set(CPACK_PACKAGE_NAME "BetterThrow")
set(CPACK_PACKAGE_VENDOR "Ryan Honeyager")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "BetterThrow")
set(CPACK_PACKAGE_VERSION "${BETTERTHROW_VERSION}")
set(CPACK_PACKAGE_VERSION_MAJOR "${BT_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${BT_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${BT_REVISION}")
set(CPACK_DEBIAN_PACKAGE_RELEASE "1")
set(CPACK_RPM_PACKAGE_RELEASE "1")

SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Ryan Honeyager")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/doc/pkg-description.txt")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "BetterThrow")
set(CPACK_PACKAGE_CONTACT "Ryan Honeyager (ryan@honeyager.info)")
set(CPACK_WIX_UPGRADE_GUID "01698ED4-5245-428D-97E8-CF46A374E437")
set(CPACK_WIX_PRODUCT_GUID "BBF0A289-8F38-45F3-ADEB-9DCE93F1A016")
#set(CPACK_WIX_PRODUCT_ICON "${CMAKE_CURRENT_SOURCE_DIR}/share/icons\\\\favicon.ico")
set(CPACK_WIX_CMAKE_PACKAGE_REGISTRY "BetterThrow")
set(CPACK_WIX_PROPERTY_ARPURLINFOABOUT "https://rhoneyager.github.io/BetterThrow")

#if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
#	set(CPACK_SYSTEM_NAME "${LSB_DISTRIBUTION_NAME_SHORT}-${LSB_RELEASE_ID_SHORT}")
#endif()
#IF(WIN32 AND NOT UNIX)
#	set(CPACK_SYSTEM_NAME "Windows")
#endif()
#option(PACKAGE_CI_BUILD "Is this a testing (continuous integration) build?" OFF)
#set(PACKAGE_GIT_BRANCH "${GITBRANCH}" CACHE STRING
#	"What is the branch of the code used to produce this build?")
#string(TIMESTAMP PACKAGE_TIMESTAMP "%Y%m%d")
#if(PACKAGE_CI_BUILD)
#	SET(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${PACKAGE_GIT_BRANCH}-${PACKAGE_TIMESTAMP}")
#endif()
IF(WIN32 AND NOT UNIX)
	set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}\\\\LICENSE.txt")
	
	# There is a bug in NSIS that does not handle full unix paths properly. Make
	# sure there is at least one set of four (4) backslashes.
	#SET(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/share/icons\\\\favicon-96x96.png")
	#SET(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/share/icons\\\\favicon.ico")
	#SET(CPACK_NSYS_MUI_UNIICON "${CMAKE_SOURCE_DIR}/share/icons\\\\favicon.ico")

#	SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\MyExecutable.exe")
	SET(CPACK_NSIS_DISPLAY_NAME "BetterThrow")
	SET(CPACK_NSIS_HELP_LINK "https:\\\\\\\\rhoneyager.github.io/BetterThrow/")
	SET(CPACK_NSIS_URL_INFO_ABOUT "https:\\\\\\\\rhoneyager.github.io/BetterThrow/")
	SET(CPACK_NSIS_CONTACT "ryan@honeyager.info")
	SET(CPACK_NSIS_MODIFY_PATH ON)
ELSE(WIN32 AND NOT UNIX)
	set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE.txt")
	SET(CPACK_STRIP_FILES FALSE)
	SET(CPACK_SOURCE_STRIP_FILES FALSE)
ENDIF(WIN32 AND NOT UNIX)
#SET(CPACK_PACKAGE_EXECUTABLES "MyExecutable" "My Executable")

set (CPACK_COMPONENTS_ALL 
	Documentation
	Headers
	Libraries
)

set(CPACK_COMPONENT_DOCUMENTATION_DESCRIPTION 
	"Doxygen HTML docs")
set(CPACK_COMPONENT_HEADERS_DESCRIPTION 
	"Header files")
set(CPACK_COMPONENT_LIBRARIES_DESCRIPTION 
	"Compiled libraries")
#set(CPACK_COMPONENT_SOURCE_DESCRIPTION 
#	"Source code")

#set(CPACK_RPM_PACKAGE_REQUIRES 
#	"cmake >= 3.1, hdf5-devel, hdf5, git, zlib-devel, boost-devel >= 1.48, gcc-c++ >= 6"
#	)

#set(CPACK_DEBIAN_PACKAGE_DEPENDS
#	"cmake (>= 3.1), libhdf5-dev, zlib1g-dev, libboost-all-dev (>= 1.48)"
#	)
#set(CPACK_DEBIAN_PACKAGE_RECOMMENDS "g++ (>= 6), hdf5-tools, git, doxygen, graphviz")
set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
#set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "all")

# This must always be last!
include(CPack)

