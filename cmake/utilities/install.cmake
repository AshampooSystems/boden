macro(fix_ios_package)
    if(BODEN_TARGET_IOS)
        
        # CMake has a problem with XCode's way of naming build directories.
        # It expects everything to land in folders called "Debug" or "Release".
        # In the case of iOS, it appends to the name depending on whether it 
        # builds for the simulator or the device ( -iphonesimulator, -iphoneos )
        
        # A bug in cmake also inserts code that looks for ${EFFECTIVE_PLATFORM_NAME}
        # which is only available as an environment variables during the build.
        
        # So we add this code to circumvent all these issues.
        
        install(CODE "
            set(EFFECTIVE_PLATFORM_NAME \$ENV{EFFECTIVE_PLATFORM_NAME})
            string(REPLACE \"-iphonesimulator\" \"\" CMAKE_INSTALL_CONFIG_NAME \${CMAKE_INSTALL_CONFIG_NAME})
            string(REPLACE \"-iphoneos\" \"\" CMAKE_INSTALL_CONFIG_NAME \${CMAKE_INSTALL_CONFIG_NAME})
        ")
    
    endif()
endmacro()
