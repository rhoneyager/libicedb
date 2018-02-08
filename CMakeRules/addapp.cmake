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
	if (NOT INSTALL_DIR_SUBFOLDERS)
		INSTALL(TARGETS ${appname}
			RUNTIME DESTINATION ${INSTALL_CMAKE_DIR}/${REL_BIN_DIR}
			COMPONENT Applications
			)
	else()
		INSTALL(TARGETS ${appname} 
			CONFIGURATIONS Debug
			RUNTIME DESTINATION ${INSTALL_CMAKE_DIR}/${REL_BIN_DIR}/bin${configappend}/Debug
			COMPONENT Applications
			)
		INSTALL(TARGETS ${appname}
			RUNTIME DESTINATION ${INSTALL_CMAKE_DIR}/${REL_BIN_DIR}/bin${configappend}/Release
			CONFIGURATIONS Release
			COMPONENT Applications)
		INSTALL(TARGETS ${appname}
			RUNTIME DESTINATION ${INSTALL_CMAKE_DIR}/${REL_BIN_DIR}/bin${configappend}/RelWithDebInfo
			CONFIGURATIONS RelWithDebInfo
			COMPONENT APPLICATIONS)
		INSTALL(TARGETS ${appname}
			RUNTIME DESTINATION ${INSTALL_CMAKE_DIR}/${REL_BIN_DIR}/bin${configappend}/MinSizeRel
			CONFIGURATIONS MinSizeRel
			COMPONENT Applications)
	endif()
endmacro(addapp appname foldername)


macro(add_header_files srcs)
  if( hds )
    set_source_files_properties(${hds} PROPERTIES HEADER_FILE_ONLY ON)
    list(APPEND ${srcs} ${hds})
  endif()
endmacro(add_header_files srcs)

