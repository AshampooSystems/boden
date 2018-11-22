if(${CMAKE_VERSION} VERSION_GREATER "3.9.0")
    find_package(Doxygen)
endif()

file(GLOB PAGES "./pages/*.md")

set(DOC_SOURCES
    ${PAGES})


if(DOXYGEN_FOUND)
    add_custom_target(boden_documentation SOURCES ${DOC_SOURCES})
    
    set(DOCUMENTATION_SOURCE_FOLDERS
        ${PROJECT_SOURCE_DIR}/framework/foundation/include
        ${PROJECT_SOURCE_DIR}/framework/ui/include)

    set(DOXYGEN_EXCLUDE ${PROJECT_SOURCE_DIR}/boden/include/bdn/test.h)
    set(DOXYGEN_FILE_PATTERNS *.h *.md)
    set(DOXYGEN_EXPAND_AS_DEFINED "BDN_IMPLEMENTS")
    set(DOXYGEN_EXPAND_ONLY_PREDEF Yes)
    set(DOXYGEN_EXTRACT_LOCAL_CLASSES No)
    set(DOXYGEN_GENERATE_TREEVIEW Yes)
    set(DOXYGEN_GENERATE_XML No)
    set(DOXYGEN_HTML_DYNAMIC_SECTIONS Yes)
    set(DOXYGEN_JAVADOC_AUTOBRIEF Yes)
    set(DOXYGEN_MACRO_EXPANSION Yes)
    set(DOXYGEN_PREDEFINED BDN_IMPLEMENTS=public BDN_GENERATING_DOCS=1)
    set(DOXYGEN_SHOW_FILES No)
    set(DOXYGEN_SHOW_NAMESPACES No)
    set(DOXYGEN_SHOW_USED_FILES No)
    set(DOXYGEN_STRIP_FROM_INC_PATH ${DOCUMENTATION_SOURCE_FOLDERS})
    set(DOXYGEN_STRIP_FROM_PATH ${DOCUMENTATION_SOURCE_FOLDERS})
    
#    set(DOXYGEN_HTML_HEADER ${CMAKE_CURRENT_LIST_DIR}/theme/bootstrap/header.html)
#    set(DOXYGEN_HTML_FOOTER ${CMAKE_CURRENT_LIST_DIR}/theme/bootstrap/footer.html)
#    set(DOXYGEN_HTML_EXTRA_STYLESHEET ${CMAKE_CURRENT_LIST_DIR}/theme/bootstrap/customdoxygen.css)
#    set(DOXYGEN_HTML_EXTRA_FILES ${CMAKE_CURRENT_LIST_DIR}/theme/bootstrap/doxy-boot.js)
    
    
    doxygen_add_docs(boden_doxygen
        ${DOCUMENTATION_SOURCE_FOLDERS} ${CMAKE_CURRENT_SOURCE_DIR}/pages
        COMMENT "Generate documentation")
    
    add_dependencies(boden_documentation boden_doxygen)
    
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html/
        DESTINATION documentation
        COMPONENT documentation)

else()
    add_custom_target(boden_documentation SOURCES ${DOC_SOURCES})
    
    if(EXISTS ${CMAKE_CURRENT_BINARY_DIR}/html)
        install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html/
            DESTINATION documentation
            COMPONENT documentation)
    endif()
endif()

group_sources_automatically(${CMAKE_CURRENT_SOURCE_DIR} ${DOC_SOURCES})
