include(addlib)
option (BUILD_PLUGIN_DDSCAT "Build the plugin for importing DDSCAT data" ON)

if (BUILD_PLUGIN_DDSCAT)
	set(IO_DDSCAT_FILES
		ddOriData.cpp
		ddOriData.h
		ddOriDataParsers.cpp
		ddOriDataParsers.h
		ddOutput.cpp
		ddOutput.h
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
		plugin-io-ddscat.hpp
		plugin-io-ddscat.cpp
		)

	add_library(io_ddscat SHARED ${IO_DDSCAT_FILES})
	addlib(io_ddscat SHARED)
	storebin(io_ddscat)
	addplugin(io_ddscat IO icedb)

	target_link_libraries(io_ddscat icedb Boost::date_time Boost::regex)
	target_include_directories(io_ddscat SYSTEM PUBLIC ${Boost_INCLUDE_DIRS})
	target_include_directories(io_ddscat SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/lib/related/eigen)
	target_include_directories(io_ddscat SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/lib)
	target_include_directories(io_ddscat SYSTEM
		PRIVATE ${HDF5_INCLUDE_DIRS} ${HDF5_INCLUDE_DIR_CPP} ${CMAKE_SOURCE_DIR}/lib/icedb/compat)

endif()
