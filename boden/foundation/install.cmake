set(ComponentName Library)

install(TARGETS foundation
    EXPORT foundation-export
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

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/
    DESTINATION
        ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT
      ${ComponentName}
    FILES_MATCHING PATTERN
        "*.h")

install(EXPORT foundation-export
    FILE
        foundationTargets.cmake
    NAMESPACE
        Boden::
    DESTINATION
        ${bodenConfigPackageLocation}
    COMPONENT
      ${ComponentName})






