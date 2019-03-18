
add_platform_library(NAME mac SOURCE_FOLDER ${CMAKE_CURRENT_LIST_DIR} COMPONENT_NAME MacOSX PARENT_LIBRARY foundation)

target_link_libraries(foundation_mac PUBLIC  "-framework Cocoa -framework WebKit")
target_link_libraries(foundation_mac INTERFACE foundation_fk)
