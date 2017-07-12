
set (dlls_objs
	germany/dlls/dlls.h
	germany/dlls/dlls.hpp
	germany/dlls/dllsImpl.h
	germany/dlls/dllsImpl.hpp
	germany/dlls/linking.h
	germany/dlls/plugins.h
	src/dllsImpl.cpp
	src/plugins.cpp
	)
set (error_objs
	germany/error/error.h
	germany/error/error.hpp
	germany/error/error_context.h
	germany/error/errorCodes.h
	germany/error/errorCodes.hpp
	src/error.cpp
	src/error_context.cpp
	src/errorCodes.cpp
	)
set (misc_objs
	germany/misc/hash.h
	germany/misc/io.h
	germany/misc/log.h
	germany/misc/mem.h
	germany/misc/mem.hpp
	germany/misc/MurmurHash3.h
	germany/misc/os_functions.h
	germany/misc/os_functions.hpp
	germany/misc/util.h
	src/hash.cpp
	src/mem.cpp
	src/MurmurHash3.cpp
	src/os_functions.cpp
	src/util.cpp
	)
set (units_objs
	germany/units/units.h
	germany/units/units.hpp
	germany/units/unitsInterface.hpp
	src/units.cpp
	)
set (versioning_objs
	germany/versioning/versioning.hpp
	germany/versioning/versioningForwards.hpp
	germany/versioning/versioningForwards.h
	src/versioning.cpp
	)
set (base_objs
	germany/defs.h
	src/defs.cpp
	)

generaterc("germany_api" "dll"
	"germany_api"
	"Scattering utility library"
	 "${CMAKE_CURRENT_BINARY_DIR}/lib.rc"
	 )

set(srcs ${base_objs} ${dlls_objs} ${error_objs} ${misc_objs} ${units_objs}
	${versioning_objs})
IF (WIN32 AND NOT CYGWIN)
SET(srcs ${srcs} 
	"${CMAKE_CURRENT_BINARY_DIR}/lib.rc")
ENDIF()
source_group("Base" FILES ${base_objs})
source_group("DLLs" FILES ${dlls_objs})
source_group("Error" FILES ${error_objs})
source_group("Misc" FILES ${misc_objs})
source_group("Units" FILES ${units_objs})
source_group("Versioning" FILES ${versioning_objs})

add_library(germany_api SHARED ${srcs})
addlib(germany_api SHARED)
if(WIN32 AND NOT CYGWIN)
elseif(APPLE OR LINUX)
	target_link_libraries(germany_api dl)
endif()
storebin(germany_api)
#target_link_libraries(germany_api ${liblist})
#
