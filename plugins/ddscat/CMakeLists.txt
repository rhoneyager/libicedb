include(addlib)
option (BUILD_PLUGIN_DDSCAT "Build the plugin for importing DDSCAT data" ON)

if (BUILD_PLUGIN_DDSCAT)
	set(IO_DDSCAT_FILES
		defs.hpp
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
		plugin-shapes.cpp
		)

	add_library(io_ddscat SHARED ${IO_DDSCAT_FILES})
	addplugin(io_ddscat IO icedb)

	if(NOT EIGEN3_FOUND)
		message(SEND_ERROR "The DDSCAT plugin needs Eigen3, but it was not found. An easy way to get it is to run \"git submodule update --init\" in the root of the repository.")
	endif()

	target_link_libraries(io_ddscat icedb Boost::date_time Boost::regex Boost::serialization Eigen3)
	target_compile_definitions(io_ddscat PRIVATE _SILENCE_FPOS_SEEKPOS_DEPRECATION_WARNING=1)

endif()
