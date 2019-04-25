set(BT_BUILD_DOCUMENTATION_DEFAULT "No")
if(BT_STANDALONE_PROJECT AND BUILD_DOCUMENTATION)
	set(BT_BUILD_DOCUMENTATION_DEFAULT "Separate")
endif()

if(NOT BT_BUILD_DOCUMENTATION)
	set(BT_BUILD_DOCUMENTATION ${BT_BUILD_DOCUMENTATION_DEFAULT} CACHE STRING 
	"Choose if BetterThrow documentation is generated. Options: No, Separate, Integrated." FORCE)
	set_property(CACHE BT_BUILD_DOCUMENTATION PROPERTY STRINGS No Separate Integrated)
endif()

if(BT_BUILD_DOCUMENTATION STREQUAL "No")
	#message(STATUS "No BetterThrow documentation will be built.")
else()
	#if (NOT BUILD_DOCUMENTATION)
		#message(STATUS "No BetterThrow documentation will be built.")
		#else()
		find_package(Doxygen)

		if (NOT DOXYGEN_FOUND)
			message(SEND_ERROR "BetterThrow documentation build requested but Doxygen is not found.")
		endif()

		if (NOT DOXYGEN_DOT_EXECUTABLE)
			set(HAVE_DOT NO)
		else()
			set(HAVE_DOT YES)
		endif()

		configure_file(doc/Doxyfile.in
			"${PROJECT_BINARY_DIR}/Doxyfile" @ONLY)

		 # This builds the html docs
		add_custom_target(BT-docs-html ${ALL_FLAG}
			${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile
			WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
			COMMENT "Generating API html documentation with Doxygen" VERBATIM
		)
		# This builds the latex docs
		#    add_custom_target(doc-latex ${ALL_FLAG}
		#        latex refman.tex
		#        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/docs/latex
		#        COMMENT "Generating API pdf documentation with Doxygen" VERBATIM
		#    )

		if (BT_BUILD_DOCUMENTATION STREQUAL "INTEGRATED")
			add_custom_target(BT-docs ALL DEPENDS BT-docs-html)
			# Provides html and pdf
			install(CODE "execute_process(COMMAND ${CMAKE_BUILD_TOOL} docs)")
			# html
			install(DIRECTORY ${CMAKE_BINARY_DIR}/docs/html/ DESTINATION ${CMAKE_INSTALL_DOCDIR}/html)
			# pdf
			#    install(DIRECTORY ${CMAKE_BINARY_DIR}/docs/latex/ DESTINATION ${CMAKE_INSTALL_DOCDIR}/latex)
		else()
			add_custom_target(BT-docs DEPENDS BT-docs-html)
		endif()

		if(BT_STANDALONE_PROJECT)
			add_custom_target(docs DEPENDS BT-docs)
			add_custom_target(doc DEPENDS BT-docs)
		endif()


		#endif()
endif()
