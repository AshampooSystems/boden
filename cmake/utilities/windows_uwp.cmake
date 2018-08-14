macro(setup_windows_uwp TARGET)
    if( BODEN_TARGET_WINUWP )	
        SET(VS_WINRT_COMPONENT TRUE)	
        
        SET(CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION "10.0.10240.0")
        
        SET(VS_WINRT_REFERENCES "Windows;Windows.UI.Core;System")
        
        target_compile_options(${TARGET} PUBLIC "/ZW")
        
        # by default the windows headers for the UWP platform define
        # a max macro. And that interferes with std::max. We set a preprocessor
        # switch to remove the max macro
        target_compile_definitions(${TARGET} PUBLIC "NOMINMAX" )
        
        
        # MSVC does not export any symbols from a DLL by default.
        # Symbols to export must be marked in the source code with
        # __declspec(dllexport).
        # All other compilers export everything by default.
        # Luckily there is a cmake feature to make VSVC behave like
        # the other compilers. So activate that.
        set_target_properties( ${TARGET} PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE )
    endif()
endmacro()
