file( GLOB SOURCES
    ./src/*.cpp
    ./src/*.h
    ./src/${BDN_TARGET}/*.cpp
    ./src/${BDN_TARGET}/*.h
    )

add_boden_test(testbodentiming "${SOURCES}" No)

install(TARGETS testbodentiming
    DESTINATION tests
    COMPONENT Tests)
