file( GLOB SOURCES
    ./src/*.cpp
    ./src/*.h
    ./src/${BODEN_TARGET}/*.cpp
    ./src/${BODEN_TARGET}/*.h
    )

add_boden_test(testboden "${SOURCES}" Yes)

group_sources_automatically(${CMAKE_CURRENT_SOURCE_DIR} ${SOURCES})

install(TARGETS testboden
    DESTINATION tests
    COMPONENT Tests)

