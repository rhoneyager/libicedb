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
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
		COMPONENT Applications)
endmacro(addapp appname foldername)

macro(addapp_test appname foldername)
	target_include_directories(${appname} 
		PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/../../lib/private )
	addapp_base(${appname} ${foldername})
endmacro(addapp_test appname foldername)


macro(add_header_files srcs)
  if( hds )
    set_source_files_properties(${hds} PROPERTIES HEADER_FILE_ONLY ON)
    list(APPEND ${srcs} ${hds})
  endif()
endmacro(add_header_files srcs)

