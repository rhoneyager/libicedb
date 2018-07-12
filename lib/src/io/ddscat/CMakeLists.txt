include(addlib)
set(IO_DDSCAT_FILES
	ddOriData.cpp
	ddOriData.h
	ddOriDataParsers.cpp
	ddOriDataParsers.h
	ddOutput.cpp
	ddOutput.h
	ddOutput_query.cpp
	ddpar.cpp
	ddpar.h
	ddpar-keys.cpp
	ddpar-parsers.cpp
	ddScattMatrix.cpp
	ddScattMatrix.h
	ddVersions.cpp
	ddVersions.h
	dielTabFile.cpp
	dielTabFile.h
	parids.h
	rotations.cpp
	rotations.h
	shapefile.cpp
	shapefile.h
	shapefile_parsers.cpp
	phaseFunc.cpp
	phaseFunc.h
	)

add_library(icedb_io_ddscat STATIC ${IO_DDSCAT_FILES})
addlib(icedb_io_ddscat STATIC)
storebin(icedb_io_ddscat)

target_include_directories(icedb_io_ddscat SYSTEM PUBLIC ${Boost_INCLUDE_DIRS})
target_include_directories(icedb_io_ddscat SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/lib/related/eigen)
target_include_directories(icedb_io_ddscat SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/lib)
