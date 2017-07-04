cmake_minimum_required (VERSION 3.0)
project(germany C CXX)
set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  "${CMAKE_SOURCE_DIR}/CMakeRules"
  "${CMAKE_CURRENT_SOURCE_DIR}"
)

include(addBaseProject)
#include(addBoostUniform)
include(adddocs)
include(addapp)
INCLUDE(addlib)
include(generaterc)
#include(PrecompiledHeader)
include(GetGitRevisionDescription)

get_git_head_revision(GITREFSPECVAR GITHASHVAR)
set(MAJOR 0)
set(MINOR 0)
set(REVISION 1)
set(germany_VERSION ${MAJOR}.${MINOR}.${REVISION})
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
addBaseProject()

configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake-settings.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake-settings.h"
    )

# Add in script-generated includes
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

ADD_SUBDIRECTORY(germany_api)
ADD_SUBDIRECTORY(apps)
