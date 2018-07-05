macro(addBoostUniform )

# Check for Anaconda
# If we are in a conda environment, ensure that the right Boost headers are being loaded
if (DEFINED ENV{CONDA_PREFIX})
	set(Boost_INCLUDE_DIR "$ENV{CONDA_PREFIX}/include" CACHE STRING "Include dir for boost")
endif()

    #set(Boost_DEBUG ON)
if (WIN32 AND NOT CYGWIN)
    option ( AUTOLINK_BOOST
       "Automatically link Boost" ON)
        set(WINBOOST_AUTOLINK ${AUTOLINK_BOOST})
else()
    set(WINBOOST_AUTOLINK OFF)
endif()
set (BOOST_STATIC_LINK OFF)
if (WIN32 AND NOT CYGWIN)
	if(WINBOOST_AUTOLINK)
	else()
		set (BOOST_STATIC_LINK ON)
	endif()
endif()
#set(BOOST_ALL_DYN_LINK ON)
#option (BOOST_STATIC_LINK "Mostly link static boost libraries" OFF)
if (NOT BOOST_STATIC_LINK)
    set(Boost_USE_STATIC_RUNTIME OFF)
    set(Boost_USE_STATIC_LIBS OFF)
else()
	set(Boost_USE_STATIC_RUNTIME ON)
    set(Boost_USE_STATIC_LIBS ON)
endif()
#message("argv ${ARGV} ${args}")
if (NOT WINBOOST_AUTOLINK)
    add_definitions(-DBOOST_ALL_NO_LIB)
    add_definitions(-DBOOST_LOG_DYN_LINK)
else()
    if (NOT BOOST_STATIC_LINK)
        add_definitions(-DBOOST_ALL_DYN_LINK)
        add_definitions(-DBOOST_LOG_DYN_LINK)
    endif()
endif()

find_package(Boost COMPONENTS ${ARGV} ) #REQUIRED)
if (NOT WINBOOST_AUTOLINK)
	#if (Boost_LIBRARIES)
	#set(liblist ${liblist} ${Boost_LIBRARIES})
	#endif()
else()
endif()

#include_directories(BEFORE SYSTEM ${Boost_INCLUDE_DIR})
#message("boost ${Boost_LIBRARY_DIR}")
#link_directories(${Boost_LIBRARY_DIR})
#link_directories(${Boost_LIBRARY_DIR_DEBUG})
#link_directories(${Boost_LIBRARY_DIR_RELEASE})



# Also install / pack the relevant boost libraries into the install tree
# Do extention swapping
#if(WIN32)
#   set(dllext ".dll")
#   set(libext "[.]lib")
#else()
#   set(dllext ".so")
#   set(libext "[.]a")
#endif()


endmacro(addBoostUniform args)


