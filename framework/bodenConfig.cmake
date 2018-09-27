get_filename_component(BDN_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

if(NOT TARGET Boden::foundation)
    include("${BDN_CMAKE_DIR}/foundationTargets.cmake")
endif()

if(NOT TARGET Boden::ui)
    include("${BDN_CMAKE_DIR}/uiTargets.cmake")
endif()
