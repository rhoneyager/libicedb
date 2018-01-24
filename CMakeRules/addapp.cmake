include(addlib)
#include(signing)
macro(addapp_base appname foldername)
	set_target_properties( ${appname} PROPERTIES FOLDER "Apps/${foldername}")
	include_directories(${CMAKE_CURRENT_BINARY_DIR})
	if (DEFINED ICEDB_COMMON_EXE_LIBS)
		target_link_libraries(${appname} ${ICEDB_COMMON_EXE_LIBS})
	endif()
	#delayedsigning( ${appname} )

endmacro(addapp_base appname foldername)

macro(addapp appname foldername)
	addapp_base(${appname} ${foldername})
	INSTALL(TARGETS ${appname} 
		CONFIGURATIONS Debug
		RUNTIME DESTINATION bin/bin${configappend}/Debug
		)
	INSTALL(TARGETS ${appname}
		RUNTIME DESTINATION bin/bin${configappend}/Release
		CONFIGURATIONS Release)
	INSTALL(TARGETS ${appname}
		RUNTIME DESTINATION bin/bin${configappend}/RelWithDebInfo
		CONFIGURATIONS RelWithDebInfo)
	INSTALL(TARGETS ${appname}
		RUNTIME DESTINATION bin/bin${configappend}/MinSizeRel
		CONFIGURATIONS MinSizeRel)
endmacro(addapp appname foldername)


macro(add_header_files srcs)
  if( hds )
    set_source_files_properties(${hds} PROPERTIES HEADER_FILE_ONLY ON)
    list(APPEND ${srcs} ${hds})
  endif()
endmacro(add_header_files srcs)

