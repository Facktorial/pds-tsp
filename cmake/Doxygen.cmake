function(enable_doxygen)
  option(ENABLE_DOXYGEN "Enable doxygen doc builds of source" ON)


  if(ENABLE_DOXYGEN)
    # set(DOXYGEN_EXCLUDE_PATTERNS */vendor/* )

    find_package(Doxygen)
    if (DOXYGEN_FOUND)

		#IF (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		#	set(CLANG_ASSISTED_PARSING YES)
		#ENDIF()

        set(FILE_PATTERNS "*.h" "*.hpp" "*.c" "*.cc" "*cpp")
        set(RECURSIVE YES)

        set(DOXYGEN_USE_MDFILE_AS_MAINPAGE ${CMAKE_CURRENT_SOURCE_DIR}/README.md)
        set(USE_MDFILE_AS_MAINPAGE @doxy_main_page@)

        # set input and output files
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/docs)
        file(MAKE_DIRECTORY ${DOXYGEN_OUT}) 

        set(DOXYGEN_GENERATE_HTML YES)
		set(DOXYGEN_GENERATE_LATEX NO)
		set(DOXYGEN_GENERATE_XML NO)
		set(DOXYGEN_SEARCH_INCLUDES YES)
		set(DOXYGEN_ENABLE_PREPROCESSING YES)
		set(DOXYGEN_CLASS_DIAGRAMS YES)
		set(DOXYGEN_CLASS_GRAPH YES)
        set(DOXYGEN_CALLER_GRAPH YES)
        set(DOXYGEN_CALL_GRAPH YES)
		set(DOXYGEN_COLLABORATION_GRAPH YES)
		set(DOXYGEN_MAX_DOT_GRAPH_DEPTH 10)
		set(DOXYGEN_INCLUDE_GRAPH YES)
		set(DOXYGEN_INCLUDE_BY_GRAPH YES)
		set(DOXYGEN_TEMPLATE_RELATIONS YES)
		set(DOXYGEN_GRAPHICAL_HIERARCHY YES)
        set(DOXYGEN_EXTRACT_ALL YES)
		set(DOXYGEN_EXTRACT_PRIVATE YES)
		set(DOXYGEN_EXTRACT_PUBLIC YES)
		set(DOXYGEN_EXTRACT_STATIC YES)
		set(DOXYGEN_HAVE_DOT YES)
		set(DOXYGEN_GENERATE_LEGEND YES)
        set(DOXYGEN_OUTPUT_DIRECTORY ${DOXYGEN_OUT})

        # request to configure the file
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
        message("Doxygen build started")

        doxygen_add_docs(
            doxygen-docs ${CMAKE_CURRENT_SOURCE_DIR}
        )

        execute_process( COMMAND "${DOXYGEN_EXECUTABLE}" "Doxyfile.doxygen-docs")	
		install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/docs)	
		IF (LIFT_DOCS)
			execute_process(COMMAND mv ${CMAKE_CURRENT_BINARY_DIR}/docs ${CMAKE_CURRENT_SOURCE_DIR}/build_docs)
		ENDIF (LIFT_DOCS)
    else (DOXYGEN_FOUND)
        message("Doxygen need to be installed to generate the doxygen documentation")
    endif (DOXYGEN_FOUND)

  endif()
endfunction()
