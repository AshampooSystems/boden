##########################################################################
# Modules

add_subdirectory(yoga)
if(BDN_INCLUDE_LOTTIE)
    add_subdirectory(lottieview)
endif()

set_property(TARGET yoga PROPERTY FOLDER "Boden/Modules/UI")
set_property(TARGET lottieview PROPERTY FOLDER "Boden/Modules/UI")
