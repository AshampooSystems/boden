
set(ComponentName Library)

install(TARGETS webview
    EXPORT webview-export
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    COMPONENT
      ${ComponentName})

install(DIRECTORY include/
    DESTINATION
        ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT
      ${ComponentName}
    FILES_MATCHING PATTERN
        "*.h")

install(EXPORT webview-export
    FILE
        webviewTargets.cmake
    NAMESPACE
        Boden::
    DESTINATION
        ${bodenConfigPackageLocation}
    COMPONENT
      ${ComponentName})






