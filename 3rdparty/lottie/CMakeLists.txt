option(BDN_INCLUDE_LOTTIE "Include the LottieView Module" ON)

if(BDN_INCLUDE_LOTTIE)
    if(BDN_PLATFORM_OSX)
        set(_source_dir ./lottie-ios/lottie-ios/Classes/)
        include(mac_files.cmake)
        add_library(airbnb_lottie_mac SHARED ${_source_files} mac_files.cmake install.cmake)
        enable_automatic_reference_counting(airbnb_lottie_mac PRIVATE)

        target_include_directories(airbnb_lottie_mac PRIVATE
            ./lottie-ios/lottie-ios/Classes/AnimatableLayers
            ./lottie-ios/lottie-ios/Classes/AnimatableProperties
            ./lottie-ios/lottie-ios/Classes/Extensions
            ./lottie-ios/lottie-ios/Classes/MacCompatibility
            ./lottie-ios/lottie-ios/Classes/Models
            ./lottie-ios/lottie-ios/Classes/Private
            ./lottie-ios/lottie-ios/Classes/PublicHeaders
            ./lottie-ios/lottie-ios/Classes/RenderSystem
            ./lottie-ios/lottie-ios/Classes/RenderSystem/InterpolatorNodes
            ./lottie-ios/lottie-ios/Classes/RenderSystem/RenderNodes
            ./lottie-ios/lottie-ios/Classes/RenderSystem/ManipulatorNodes
            ./lottie-ios/lottie-ios/Classes/RenderSystem/AnimatorNodes
            )

        target_include_directories(airbnb_lottie_mac PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lottie-ios/lottie-ios/Classes/PublicHeaders>
            $<INSTALL_INTERFACE:include/lottie-ios>
        )

        target_link_libraries(airbnb_lottie_mac PUBLIC "-framework Cocoa -framework QuartzCore")
        add_library(airBnB::Lottie ALIAS airbnb_lottie_mac)
        
        set_property(TARGET airbnb_lottie_mac PROPERTY FOLDER "Boden/3rd-Party/Lottie")
    elseif(BDN_PLATFORM_IOS)

        set(_source_dir ./lottie-ios/lottie-ios/Classes/)
        include(ios_files.cmake)
        add_library(airbnb_lottie_ios STATIC ${_source_files} ios_files.cmake install.cmake)
        target_link_libraries(airbnb_lottie_ios PUBLIC "-framework UIKit -framework CoreGraphics -framework QuartzCore -framework Foundation -framework AVFoundation")
        enable_automatic_reference_counting(airbnb_lottie_ios PRIVATE)
        target_include_directories(airbnb_lottie_ios PRIVATE
            ./lottie-ios/lottie-ios/Classes/AnimatableLayers
            ./lottie-ios/lottie-ios/Classes/AnimatableProperties
            ./lottie-ios/lottie-ios/Classes/Extensions
            ./lottie-ios/lottie-ios/Classes/MacCompatibility
            ./lottie-ios/lottie-ios/Classes/Models
            ./lottie-ios/lottie-ios/Classes/Private
            ./lottie-ios/lottie-ios/Classes/PublicHeaders
            ./lottie-ios/lottie-ios/Classes/RenderSystem
            ./lottie-ios/lottie-ios/Classes/RenderSystem/InterpolatorNodes
            ./lottie-ios/lottie-ios/Classes/RenderSystem/RenderNodes
            ./lottie-ios/lottie-ios/Classes/RenderSystem/ManipulatorNodes
            ./lottie-ios/lottie-ios/Classes/RenderSystem/AnimatorNodes
            )
        target_include_directories(airbnb_lottie_ios PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lottie-ios/lottie-ios/Classes/PublicHeaders>
            $<INSTALL_INTERFACE:include/lottie-ios>
        )
        add_library(airBnB::Lottie ALIAS airbnb_lottie_ios)
        
        set_property(TARGET airbnb_lottie_ios PROPERTY FOLDER "Boden/3rd-Party/Lottie")

        ios_fix_lib_build_folder(airbnb_lottie_ios)
    endif()

    include(install.cmake)
endif()
