get_filename_component(BODEN_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

if(NOT TARGET Boden::foundation)
    include("${BODEN_CMAKE_DIR}/foundationTargets.cmake")
endif()

if(NOT TARGET Boden::ui)
    include("${BODEN_CMAKE_DIR}/uiTargets.cmake")
endif()
