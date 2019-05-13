
# This little macro lets you set any XCode specific property
macro (set_xcode_property TARGET XCODE_PROPERTY XCODE_VALUE)
	set_property (TARGET ${TARGET} PROPERTY XCODE_ATTRIBUTE_${XCODE_PROPERTY} ${XCODE_VALUE})
endmacro (set_xcode_property)

macro(set_xcode_appicon_asset TARGETNAME APP_ICON_NAME)
    if(BDN_PLATFORM_IOS OR BDN_PLATFORM_OSX)
        set_xcode_property(${TARGETNAME} ASSETCATALOG_COMPILER_APPICON_NAME ${APP_ICON_NAME})
    endif()
endmacro()
