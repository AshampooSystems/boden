macro(setup_gtk TARGET)
    if( BODEN_USES_GTK )
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
        
        target_include_directories(${TARGET} PRIVATE ${GTK3_INCLUDE_DIRS})
        target_link_libraries(${TARGET} PUBLIC ${GTK3_LIBRARIES})
        
        #link_directories(${GTK3_LIBRARY_DIRS})
        
        # Add other flags to the compiler

        #target_compile_definitions(${TARGET} PRIVATE ${GTK3_CFLAGS_OTHER})

        disable_deprecated_declarations_warning(${TARGET} PRIVATE)
    endif()
endmacro()
