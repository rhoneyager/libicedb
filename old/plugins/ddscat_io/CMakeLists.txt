generaterc("ddscat_io" "dll"
	"libicedb"
	"Implementation of the ddscat i/o functions"
	 "${CMAKE_CURRENT_BINARY_DIR}/app.rc"
	 )
	 set (tsrcs 
			level00_ddscat_avg.h
			level00_ddscat_diel.h
			level00_ddscat_fml.h
			level00_ddscat_par.h
			level00_ddscat_sca.h
			level00_ddscat_shape.h
			level00_ddscat_shape.hpp
			plugin_ddscat.cpp
			plugin_ddscat.hpp
			raw_par_read.cpp
			raw_par_write.cpp
			raw_shape_read.cpp
			raw_shape_write.cpp
			shape.h)
IF (WIN32 AND NOT CYGWIN)
SET(tsrcs ${tsrcs} 
	"${CMAKE_CURRENT_BINARY_DIR}/app.rc")
ENDIF()


add_library(ddscat_io SHARED ${tsrcs})
#target_link_libraries(ddscat_io libicedb)
addplugin(ddscat_io io icedb)
storeplugin(ddscat_io icedb)
