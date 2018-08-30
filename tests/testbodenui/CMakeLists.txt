file(GLOB SOURCES
    ./src/*.cpp
    ./src/*.h
    ./src/${BDN_TARGET}/*.cpp
    ./src/${BDN_TARGET}/*.h
    ./src/${BDN_TARGET}/*.mm
    ./src/${BDN_TARGET}/*.hh
    )

add_boden_test(testbodenui "${SOURCES}" No)

setup_gtk(testbodenui)

group_sources_automatically(${CMAKE_CURRENT_SOURCE_DIR} ${SOURCES})

install(TARGETS testbodenui
    DESTINATION tests
    COMPONENT Tests)
