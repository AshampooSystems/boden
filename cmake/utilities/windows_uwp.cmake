macro(setup_windows_uwp TARGET)
    if( BDN_PLATFORM_WINUWP )	
        SET(VS_WINRT_COMPONENT TRUE)	
        
        SET(CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION "10.0.10240.0")
        
        SET(VS_WINRT_REFERENCES "Windows;Windows.UI.Core;System")
        
        target_compile_options(${TARGET} PUBLIC "/ZW")
        
        # by default the windows headers for the UWP platform define
        # a max macro. And that interferes with std::max. We set a preprocessor
        # switch to remove the max macro
        target_compile_definitions(${TARGET} PUBLIC "NOMINMAX" )
    endif()
endmacro()
