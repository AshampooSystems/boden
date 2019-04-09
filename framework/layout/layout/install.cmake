
set(ComponentName Library)

install(TARGETS layout
    EXPORT layout-export
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

install(EXPORT layout-export
    FILE
        layoutTargets.cmake
    NAMESPACE
        Boden::
    DESTINATION
        ${bodenConfigPackageLocation}
    COMPONENT
      ${ComponentName})






