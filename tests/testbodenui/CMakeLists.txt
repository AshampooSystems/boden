file(GLOB SOURCES
    ./src/*.cpp
    ./src/*.h
    ./src/${BODEN_TARGET}/*.cpp
    ./src/${BODEN_TARGET}/*.h
    ./src/${BODEN_TARGET}/*.mm
    ./src/${BODEN_TARGET}/*.hh
    )

add_boden_test(testbodenui "${SOURCES}" No)

setup_gtk(testbodenui)

group_sources_automatically(${CMAKE_CURRENT_SOURCE_DIR} ${SOURCES})
