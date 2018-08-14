
macro(setup_ios TARGET)
    if( BODEN_TARGET_IOS )
        # there is a bug in cmake that causes debug symbols to not be generated for ios
        # builds. So we have to set it manually. Unfortunately we can apparently only
        # set it for all builds - waiting for a fix from ios cmake maintainer.
        # XXX needs to be removed for release build
        set_xcode_property(${TARGET} GCC_GENERATE_DEBUGGING_SYMBOLS YES)
        
        set_xcode_property(${TARGET} CODE_SIGN_IDENTITY "")
        set_xcode_property(${TARGET} CODE_SIGNING_REQUIRED "NO")
        
        #set_xcode_property(boden CODE_SIGN_IDENTITY "iPhone Developer")
        #set_xcode_property(boden CODE_SIGN_STYLE "Automatic")
        #set_xcode_property(boden PROVISIONING_PROFILE "Automatic")
        #set_xcode_property(boden DEVELOPMENT_TEAM "X")
    endif()
endmacro()
