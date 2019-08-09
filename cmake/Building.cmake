if(DEFINED PACKAGE_GIT_BRANCH)
	set(GITBRANCH ${PACKAGE_GIT_BRANCH})
else()

	include(GetGitRevisionDescription)
	# Versioning information
	get_git_head_revision(GITREFSPECVAR GITHASHVAR)
	if ("${GITREFSPECVAR}" STREQUAL "")
		set(GITBRANCH "GIT_BRANCH_UNKNOWN")
	else()
		string(SUBSTRING "${GITREFSPECVAR}" 11 -1 GITBRANCH )
	endif()
endif()

# This config file stores basic information about the build.
# Version number, git hash, ...
# This gets pulled into the icedb target.
configure_file (
	"${CMAKE_CURRENT_SOURCE_DIR}/lib/util/cmake-settings.h.in"
	"${CMAKE_CURRENT_BINARY_DIR}/lib/icedb-cmake-settings.h"
    )


