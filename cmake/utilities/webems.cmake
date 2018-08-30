
macro(setup_web_ems)
    if( BDN_PLATFORM_WEBEMS )
        add_definitions( -DBDN_PLATFORM_WEBEMS )

        # the C++ standard library provided by Emscripten has a bug in codecvt.
        # In encodes the data in UTF-8, but when multi-byte sequences are generated
        # then it only counts 1 byte. Thus the end of the resulting string is cut off.
        # To work around that we use our own UTF-8 implementation. We know that the
        # locale encoding is always UTF-8 with Emscripten.
        add_definitions( -DBDN_OVERRIDE_LOCALE_ENCODING_UTF8=1 )

        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s DISABLE_EXCEPTION_CATCHING=0 -s DEMANGLE_SUPPORT=1 -s ALLOW_MEMORY_GROWTH=1")
        SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -s ASSERTIONS=2")


        set(CMAKE_EXECUTABLE_SUFFIX ".html")


        # for some reason it has no effect if we set the generic CMAKE_EXE_LINKER_FLAGS
        # on linux. We must set the debug/release versions
        SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} --emrun --bind -s EXPORTED_FUNCTIONS=\"['_main', '_bdn_webems_MainDispatcher_timerEventCallback', '_bdn_webems_TextFieldCore_onInput']\" -s EXTRA_EXPORTED_RUNTIME_METHODS=\"['cwrap']\"")
        SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} --emrun --bind -s EXPORTED_FUNCTIONS=\"['_main', '_bdn_webems_MainDispatcher_timerEventCallback', '_bdn_webems_TextFieldCore_onInput']\" -s EXTRA_EXPORTED_RUNTIME_METHODS=\"['cwrap']\"")

    endif()
endmacro()
