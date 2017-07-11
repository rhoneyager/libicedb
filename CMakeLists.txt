cmake_minimum_required (VERSION 3.1)
project(germany C CXX)
set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  "${CMAKE_SOURCE_DIR}/CMakeRules"
  "${CMAKE_CURRENT_SOURCE_DIR}"
)
set(CMAKE_CXX_STANDARD 11)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)
include(addBaseProject)
#include(addBoostUniform)
include(adddocs)
include(addapp)
INCLUDE(addlib)
include(generaterc)
include(GetGitRevisionDescription)

get_git_head_revision(GITREFSPECVAR GITHASHVAR)
set(MAJOR 0)
set(MINOR 0)
set(REVISION 2)
set(germany_VERSION ${MAJOR}.${MINOR}.${REVISION})
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
addBaseProject()

configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake-settings.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake-settings.h"
    )

# Add in script-generated includes
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

find_package(Eigen3 REQUIRED)
find_package(Udunits2)
if (UDUNITS2_INCLUDE_DIR)
	include_directories(${UDUNITS2_INCLUDE_DIR})
	set(HAS_UDUNITS TRUE)
else(UDUNITS2_INCLUDE_DIR)
	OPTION(SRC_BUILD_UDUNITS "Whether to build the udunits library, provided in source" OFF)
	IF(SRC_BUILD_UDUNITS)
		SET(HAS_UDUNITS TRUE)
		ADD_SUBDIRECTORY(related/UDUNITS-2)
	else(SRC_BUILD_UDUNITS)
		SET(HAS_UDUNITS FALSE)
	endif(SRC_BUILD_UDUNITS)
endif(UDUNITS2_INCLUDE_DIR)

ADD_SUBDIRECTORY(germany_api)
ADD_SUBDIRECTORY(plugins)
ADD_SUBDIRECTORY(apps)
