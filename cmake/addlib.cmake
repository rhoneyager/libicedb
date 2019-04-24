macro(addlib) #libname libshared)
	set(libname "${ARGV0}")
	if ("" STREQUAL "${ARGV1}")
		set(isshared "STATIC")
	else()
		set(isshared "${ARGV1}")
	endif()
	if ("" STREQUAL "${ARGV2}")
		set(libshared ${libname})
	else()
		set(libshared ${ARGV2})
	endif()
	#message("addlib name: ${libname}, shared name: ${libshared}, is shared: ${isshared}")

	SET_TARGET_PROPERTIES( ${libname} PROPERTIES RELEASE_POSTFIX _Release${configappend} )
	SET_TARGET_PROPERTIES( ${libname} PROPERTIES MINSIZEREL_POSTFIX _MinSizeRel${configappend} )
	SET_TARGET_PROPERTIES( ${libname} PROPERTIES RELWITHDEBINFO_POSTFIX _RelWithDebInfo${configappend} )
	SET_TARGET_PROPERTIES( ${libname} PROPERTIES DEBUG_POSTFIX _Debug${configappend} )

	set_target_properties( ${libname} PROPERTIES FOLDER "Libs")

	# These are for symbol export
	target_compile_definitions(${libname} PRIVATE ${libshared}_EXPORTING)
	target_compile_definitions(${libname} PUBLIC ${libshared}_SHARED=$<STREQUAL:${isshared},SHARED>)
	target_compile_definitions(${libname} PRIVATE BUILDCONF="${CMAKE_BUILD_TYPE}")
	# This is for determining the build type (esp. used in registry code)
	target_compile_definitions(${libname} PRIVATE BUILDTYPE=BUILDTYPE_$<CONFIGURATION>)

	INSTALL(TARGETS ${libname}
		EXPORT icedbTargets
		#RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} # See NSIS below
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
		ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
		COMPONENT Libraries)
	# NSIS bug. Have to do this twice.
	if (WIN32 AND NOT CYGWIN)
		if("SHARED" STREQUAL "${libshared}")
			INSTALL(TARGETS ${libname}
				#EXPORT icedbTargets
				RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
				COMPONENT Libraries)
		endif()
	endif()
endmacro(addlib )


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
endmacro(storebin objname)

macro(storeplugin objname folder)
	set_target_properties( ${objname}
		PROPERTIES
		#  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
		# LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
		ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/Debug/${folder}/plugins"
		ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/Release/${folder}/plugins"
		ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}/MinSizeRel/${folder}/plugins"
		ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/RelWithDebInfo/${folder}/plugins"
		LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/Debug/${folder}/plugins"
		LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/Release/${folder}/plugins"
		LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}/MinSizeRel/${folder}/plugins"
		LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/RelWithDebInfo/${folder}/plugins"
		RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/Debug/${folder}/plugins"
		RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/Release/${folder}/plugins"
		RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}/MinSizeRel/${folder}/plugins"
		RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/RelWithDebInfo/${folder}/plugins"
	)
endmacro(storeplugin objname folder)

macro(addplugin appname foldername folder)
	SET_TARGET_PROPERTIES( ${appname} PROPERTIES RELEASE_POSTFIX _Release${configappend} )
	SET_TARGET_PROPERTIES( ${appname} PROPERTIES MINSIZEREL_POSTFIX _MinSizeRel${configappend} )
	SET_TARGET_PROPERTIES( ${appname} PROPERTIES RELWITHDEBINFO_POSTFIX _RelWithDebInfo${configappend} )
	SET_TARGET_PROPERTIES( ${appname} PROPERTIES DEBUG_POSTFIX _Debug${configappend} )

	target_compile_definitions(${appname} PRIVATE ${appname}_EXPORTING)
	target_compile_definitions(${appname} PUBLIC ${appname}_SHARED=1)
	#message("addplugin name: ${appname}, shared name: ${appname}")

	# This is for determining the build type (esp. used in registry code)
	target_compile_definitions(${appname} PRIVATE BUILDCONF="${CMAKE_BUILD_TYPE}")
	target_compile_definitions(${appname} PRIVATE BUILDTYPE=BUILDTYPE_$<CONFIGURATION>)
	set_target_properties( ${appname} PROPERTIES FOLDER "Plugins/${foldername}")
	INSTALL(TARGETS ${appname} 
		#RUNTIME DESTINATION ${CMAKE_INSTALL_LIBEXECDIR}/icedb/plugins
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBEXECDIR}/icedb/plugins
		ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBEXECDIR}/icedb/plugins
		COMPONENT Plugins)
	# There is a bug in the NSIS generator that causes it to miss dlls.
	# Fix is here:
	if (WIN32 AND NOT CYGWIN)
		INSTALL(TARGETS ${appname}
			RUNTIME DESTINATION ${CMAKE_INSTALL_LIBEXECDIR}/icedb/plugins
			COMPONENT Plugins)
	endif()
	storeplugin(${appname} ${folder})
endmacro(addplugin appname)
