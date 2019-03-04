
add_platform_library(mac ${CMAKE_CURRENT_LIST_DIR} MacOSX foundation)

target_link_libraries(foundation_mac PUBLIC  "-framework Cocoa -framework WebKit")
target_link_libraries(foundation_mac INTERFACE foundation_fk)
