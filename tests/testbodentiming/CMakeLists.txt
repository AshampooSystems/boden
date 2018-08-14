file( GLOB SOURCES
    ./src/*.cpp
    ./src/*.h
    ./src/${BODEN_TARGET}/*.cpp
    ./src/${BODEN_TARGET}/*.h
    )

add_boden_test(testbodentiming "${SOURCES}" No)

