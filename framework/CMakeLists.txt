include(CMakePackageConfigHelpers)

##########################################################################
# Utilities

function(sourcefiles_from_folder HEADERS SOURCES FOLDER)
    file(GLOB _HEADERS ./include/bdn/${FOLDER}/*.h ./include/bdn/${FOLDER}/*.hh)
    file(GLOB _SOURCES ./src/${FOLDER}/*.cpp ./src/${FOLDER}/*.c ./src/${FOLDER}/*.mm ./src/${FOLDER}/*.m)

    set(${HEADERS} ${_HEADERS} PARENT_SCOPE)
    set(${SOURCES} ${_SOURCES} PARENT_SCOPE)
endfunction()

function(add_sources FILES HEADERS FOLDER)
    sourcefiles_from_folder(_HEADERS _SOURCES ${FOLDER})
    set(${FILES} ${${FILES}} ${_HEADERS} ${_SOURCES} PARENT_SCOPE)
    set(${HEADERS} ${${HEADERS}} ${_HEADERS} PARENT_SCOPE)
endfunction()


##########################################################################
# CPack


set(bodenConfigPackageLocation ${CMAKE_INSTALL_LIBDIR}/cmake CACHE INTERNAL "")

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/bodenConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
)

configure_file(bodenConfig.cmake
  "${CMAKE_CURRENT_BINARY_DIR}/bodenConfig.cmake"
  COPYONLY
)

install(
  FILES
    bodenConfig.cmake
    "${CMAKE_CURRENT_BINARY_DIR}/bodenConfigVersion.cmake"
  DESTINATION
    ${bodenConfigPackageLocation}
  COMPONENT
    Library
)

##########################################################################
# Boden modules

add_subdirectory(foundation)
add_subdirectory(net)
add_subdirectory(ui)
add_subdirectory(testing)
