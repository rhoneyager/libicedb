
set (dlls_objs
	icedb/dlls/dlls.h
	icedb/dlls/dlls.hpp
	icedb/dlls/dllsImpl.h
	icedb/dlls/dllsImpl.hpp
	icedb/dlls/dllsInterfaceImpl.hpp
	icedb/dlls/dllsInterface.hpp
	icedb/dlls/linking.h
	icedb/dlls/plugins.h
	src/dllsImpl.cpp
	src/plugins.cpp
	)
set (fs_objs
	icedb/fs/fs.h
	icedb/fs/fs_backend.hpp
	icedb/fs/fs_dll_impl.hpp
	src/fs.cpp)
set (error_objs
	icedb/error/error.h
	icedb/error/error.hpp
	icedb/error/errorInterface.hpp
	icedb/error/errorInterfaceImpl.hpp
	icedb/error/error_context.h
	icedb/error/error_contextInterface.hpp
	icedb/error/error_contextInterfaceImpl.hpp
	icedb/error/errorCodes.h
	icedb/error/errorCodes.hpp
	src/error.cpp
	src/error_context.cpp
	src/errorCodes.cpp
	)
set (misc_objs
	icedb/misc/hash.h
	icedb/misc/io.h
	icedb/misc/log.h
	icedb/misc/mem.h
	icedb/misc/mem.hpp
	icedb/misc/MurmurHash3.h
	icedb/misc/os_functions.h
	icedb/misc/os_functions.hpp
	icedb/misc/util.h
	icedb/misc/utilInterface.h
	icedb/misc/utilInterfaceImpl.hpp
	src/hash.cpp
	src/mem.cpp
	src/MurmurHash3.cpp
	src/os_functions.cpp
	src/util.cpp
	)
set (units_objs
	icedb/units/units.h
	icedb/units/units.hpp
	icedb/units/unitsInterface.hpp
	icedb/units/unitsInterfaceImpl.hpp
	src/units.cpp
	)
set (versioning_objs
	icedb/versioning/versioning.hpp
	icedb/versioning/versioningForwards.hpp
	icedb/versioning/versioningForwards.h
	src/versioning.cpp
	)
set (base_objs
	icedb/defs.h
	src/defs.cpp
	)
set (level_0_objs
	icedb/level_0/shape.h
	icedb/level_0/shape.hpp
	icedb/level_0/shapeInterface.h
	icedb/level_0/shapeInterfaceImpl.hpp
	docs/attributes.md
	docs/level_0.md
	docs/shapes.md
	src/shapes.cpp
	)
set (level_1_objs
	icedb/level_1/candidate_shapeAlg.h
	src/candidate_shapeAlg.cpp
	)
set (data_objs
	icedb/data/attrs.h
	icedb/data/attrs.hpp
	icedb/data/tables.h
	src/attrs.cpp
	src/tables.cpp
	)

generaterc("libicedb" "dll"
	"libicedb"
	"Scattering utility library"
	 "${CMAKE_CURRENT_BINARY_DIR}/lib.rc"
	 )

set(srcs ${base_objs} ${data_objs} ${dlls_objs} ${error_objs} ${fs_objs}
	${misc_objs} ${units_objs}
	${versioning_objs} ${level_0_objs} ${level_1_objs})
IF (WIN32 AND NOT CYGWIN)
SET(srcs ${srcs} 
	"${CMAKE_CURRENT_BINARY_DIR}/lib.rc")
ENDIF()
source_group("Base" FILES ${base_objs})
source_group("Data Concepts" FILES ${data_objs})
source_group("DLLs" FILES ${dlls_objs})
source_group("Error" FILES ${error_objs})
source_group("Filesystem" FILES ${fs_objs})
source_group("Misc" FILES ${misc_objs})
source_group("Units" FILES ${units_objs})
source_group("Versioning" FILES ${versioning_objs})
source_group("Level 0" FILES ${level_0_objs})
source_group("Level 1" FILES ${level_1_objs})

add_library(libicedb SHARED ${srcs})
addlib(libicedb SHARED)
if(WIN32 AND NOT CYGWIN)
elseif(APPLE OR LINUX)
	target_link_libraries(libicedb dl)
endif()
storebin(libicedb)
#target_link_libraries(libicedb ${liblist})
#
