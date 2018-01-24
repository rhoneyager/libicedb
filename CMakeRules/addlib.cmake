macro(addlib libname libshared )
	if ("" STREQUAL "${ARGV2}")
		set(headername ${libname})
	else()
		set(headername ${ARGV2})
	endif()
	SET_TARGET_PROPERTIES( ${libname} PROPERTIES RELEASE_POSTFIX _Release${configappend} )
	SET_TARGET_PROPERTIES( ${libname} PROPERTIES MINSIZEREL_POSTFIX _MinSizeRel${configappend} )
	SET_TARGET_PROPERTIES( ${libname} PROPERTIES RELWITHDEBINFO_POSTFIX _RelWithDebInfo${configappend} )
	SET_TARGET_PROPERTIES( ${libname} PROPERTIES DEBUG_POSTFIX _Debug${configappend} )
	set_target_properties( ${libname} PROPERTIES FOLDER "Libs")

	if (DEFINED ICEDB_COMMON_SHARED_LIBS)
		target_link_libraries(${libname} ${ICEDB_COMMON_SHARED_LIBS})
	endif()


	# This is for determining the build type (esp. used in registry code)
	target_compile_definitions(${libname} PRIVATE BUILDCONF="${CMAKE_BUILD_TYPE}")
	target_compile_definitions(${libname} PRIVATE BUILDTYPE=BUILDTYPE_$<CONFIGURATION>)
	# These two are for symbol export
	target_compile_definitions(${libname} PRIVATE EXPORTING_${headername})
	target_compile_definitions(${libname} PUBLIC SHARED_${headername}=$<STREQUAL:${libshared},SHARED>)
	INSTALL(TARGETS ${libname} 
		RUNTIME DESTINATION bin/bin${configappend}
		LIBRARY DESTINATION lib/lib${configappend}
		ARCHIVE DESTINATION lib/lib${configappend}
		COMPONENT Libraries
		)

endmacro(addlib libname headername)

macro(storebin objname)
set_target_properties( ${objname}
    PROPERTIES
    #  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    # LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/Debug"
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/Release"
    ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}/MinSizeRel"
    ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/RelWithDebInfo"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/Debug"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/Release"
    LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}/MinSizeRel"
    LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/RelWithDebInfo"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/Debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/Release"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}/MinSizeRel"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/RelWithDebInfo"
    )

	#INSTALL(TARGETS ${objname} 
		#LIBRARY DESTINATION lib/lib${configappend}
				#			ARCHIVE DESTINATION lib/lib${configappend}
		#		RUNTIME DESTINATION ${INSTALL_CMAKE_DIR}/${REL_BIN_DIR}/bin${configappend}
		#		LIBRARY DESTINATION ${INSTALL_CMAKE_DIR}/${REL_LIB_DIR}/lib${configappend}
		#		ARCHIVE DESTINATION ${INSTALL_CMAKE_DIR}/${REL_LIB_DIR}/lib${configappend}
		#COMPONENT Libraries)
endmacro(storebin objname)

macro(storeplugin objname folder)
set_target_properties( ${objname}
    PROPERTIES
    #  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    # LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/Debug/${folder}-plugins"
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/Release/${folder}-plugins"
    ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}/MinSizeRel/${folder}-plugins"
    ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/RelWithDebInfo/${folder}-plugins"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/Debug/${folder}-plugins"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/Release/${folder}-plugins"
    LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}/MinSizeRel/${folder}-plugins"
    LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/RelWithDebInfo/${folder}-plugins"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/Debug/${folder}-plugins"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/Release/${folder}-plugins"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}/MinSizeRel/${folder}-plugins"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/RelWithDebInfo/${folder}-plugins"
)

endmacro(storeplugin objname folder)

macro(addplugin appname foldername folder)
	set_target_properties( ${appname} PROPERTIES FOLDER "Plugins/${foldername}")
	INSTALL(TARGETS ${appname} 
		RUNTIME DESTINATION ${INSTALL_CMAKE_DIR}/${REL_BIN_DIR}/bin${configappend}/${folder}-plugins
		LIBRARY DESTINATION ${INSTALL_CMAKE_DIR}/${REL_LIB_DIR}/lib${configappend}/${folder}-plugins
		ARCHIVE DESTINATION ${INSTALL_CMAKE_DIR}/${REL_LIB_DIR}/lib${configappend}/${folder}-plugins
		COMPONENT Plugins)
	include_directories(${CMAKE_CURRENT_BINARY_DIR})

	storeplugin(${appname} ${folder})
endmacro(addplugin appname)
