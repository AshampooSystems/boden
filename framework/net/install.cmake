
set(ComponentName Library)

install(TARGETS net
    EXPORT net-export
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

install(EXPORT net-export
    FILE
        netTargets.cmake
    NAMESPACE
        Boden::
    DESTINATION
        ${bodenConfigPackageLocation}
    COMPONENT
      ${ComponentName})






