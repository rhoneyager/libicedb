include(signing)
macro(addapp appname foldername)
	set_target_properties( ${appname} PROPERTIES FOLDER "Apps/${foldername}")
	INSTALL(TARGETS ${appname} RUNTIME DESTINATION bin/bin${configappend} COMPONENT Applications)
	include_directories(${CMAKE_CURRENT_BINARY_DIR})
	if (DEFINED ICEDB_COMMON_EXE_LIBS)
		target_link_libraries(${appname} ${ICEDB_COMMON_EXE_LIBS})
	endif()
	#	storebin(${appname})
	delayedsigning( ${appname} )
endmacro(addapp appname)

macro(add_header_files srcs)
  if( hds )
    set_source_files_properties(${hds} PROPERTIES HEADER_FILE_ONLY ON)
    list(APPEND ${srcs} ${hds})
  endif()
endmacro(add_header_files srcs)

