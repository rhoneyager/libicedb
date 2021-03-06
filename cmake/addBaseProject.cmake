macro(getMSVCappend)
    if(DEFINED MSVC)
        if (MSVC)
                set (MSVC_APPEND ${MSVC_VERSION})
        endif()
    endif()
endmacro(getMSVCappend)

# This macro sets compiler and os-specific options, and it also
# sets a few convenience variables regarding the naming of
# libraries and provides build system information as preprocessor
# defines.
# TODO: Some cleanup is needed.
macro(addBaseProject)

if(DEFINED MSVC)
    # MSVC parallel builds by default
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
	# Boost bug
	add_definitions("/D _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING")
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
	SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-attributes")
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Intel")
	SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-attributes")
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
  # using regular Clang or AppleClang
  # message("TODO: Test and fix Clang Builds")
	#SET (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++1z -stdlib=libc++")
	add_definitions("-DGSL_USE_STD_BYTE=0")
else()
	message("Unsupported compiler: ${CMAKE_CXX_COMPILER_ID}. May need to adjust compiler and linker flags.")
endif()

if("${CMAKE_HOST_SYSTEM_NAME}" MATCHES "Linux")
	set (ICEDB_COMMON_EXE_LIBS ${ICEDB_COMMON_EXE_LIBS} pthread)
	#SET (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -ldl")
	#SET (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -ldl")
	#SET (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lpthread -ldl")
elseif("${CMAKE_HOST_SYSTEM_NAME}" MATCHES "FreeBSD")
	set (ICEDB_COMMON_EXE_LIBS ${ICEDB_COMMON_EXE_LIBS} thr)
elseif("${CMAKE_HOST_SYSTEM_NAME}" MATCHES "Darwin")
	#set (ICEDB_COMMON_EXE_LIBS ${ICEDB_COMMON_EXE_LIBS} thr)
elseif("${CMAKE_HOST_SYSTEM_NAME}" MATCHES "Windows")
	#set (ICEDB_COMMON_EXE_LIBS ${ICEDB_COMMON_EXE_LIBS} thr)
elseif("${CMAKE_HOST_SYSTEM_NAME}" MATCHES "CYGWIN")
	#set .....
else()
	message("Unhandled system name: ${CMAKE_HOST_SYSTEM_NAME}. Build system should be expanded to handle this case.")
endif()

    set(configbase "")
    if (CMAKE_CL_64)
        set(configbase "${configbase}x64")
    elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "64")
        set(configbase "${configbase}x64")
    elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "86")
        set(configbase "${configbase}x86")
    else()
	set(configbase "${configbase}unknown")
    endif()

    if (MSVC)
        getMSVCappend()
        set(configbase "${configbase}_vs${MSVC_APPEND}")
        add_definitions(-DCONF="$(Configuration)_${configbase}")
        set(CONF CONF)
    elseif (MINGW)
        set(configbase "${configbase}_mingw")
        add_definitions(-DCONF="${CMAKE_BUILD_TYPE}_${configbase}")
        set(CONF "\"${CMAKE_BUILD_TYPE}_${configbase}\"")
    else()
        set(configbase "${configbase}_${CMAKE_CXX_COMPILER_ID}")
        add_definitions(-DCONF="${CMAKE_BUILD_TYPE}_${configbase}")
        set(CONF "\"${CMAKE_BUILD_TYPE}_${configbase}\"")
    endif()

    set(configappend "_${configbase}")

endmacro(addBaseProject)

