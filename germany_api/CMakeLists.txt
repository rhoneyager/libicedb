set (base_objs
	germany/defs.h
	germany/dlls.h
	germany/dlls.hpp
	germany/dllsImpl.h
	germany/dllsImpl.hpp
	germany/error.h
	germany/error.hpp
	germany/error_context.h
	germany/errorCodes.h
	germany/errorCodes.hpp
	germany/log.h
	germany/mem.h
	germany/mem.hpp
	germany/util.h
	src/defs.cpp
	src/dllsImpl.cpp
	src/error.cpp
	src/error_context.cpp
	src/errorCodes.cpp
	src/mem.cpp
	src/util.cpp
	)

generaterc("germany_api" "dll"
	"germany_api"
	"Scattering utility library"
	 "${CMAKE_CURRENT_BINARY_DIR}/lib.rc"
	 )

set(srcs ${base_objs})
IF (WIN32 AND NOT CYGWIN)
SET(srcs ${srcs} 
	"${CMAKE_CURRENT_BINARY_DIR}/lib.rc")
ENDIF()
source_group("Base" FILES ${base_objs})


add_library(germany_api SHARED ${srcs})
addlib(germany_api SHARED)
if(WIN32 AND NOT CYGWIN)
else()
	target_link_libraries(germany_api dl)
endif()
storebin(germany_api)
#target_link_libraries(germany_api ${liblist})