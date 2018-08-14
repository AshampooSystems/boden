file(GLOB _UIDEMO_SOURCES ./src/*.cpp)

list(APPEND UIDEMO_FILES ${_UIDEMO_SOURCES})

IF ( APPLE )
	SET(APP_TYPE MACOSX_BUNDLE)
	SET(MACOSX_BUNDLE_BUNDLE_NAME uidemo)
	SET(MACOSX_BUNDLE_GUI_IDENTIFIER "io.boden.uidemo")
endif()

add_universal_executable(uidemo No ${UIDEMO_FILES})


group_sources_automatically(${CMAKE_CURRENT_SOURCE_DIR} ${UIDEMO_FILES})

target_link_libraries(uidemo PRIVATE boden)
