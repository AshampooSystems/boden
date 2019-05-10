
file(GLOB _PYTHON "./*.py")

add_custom_target(boden_bauer SOURCES ${_PYTHON} "../boden.py")

group_sources_automatically(${CMAKE_CURRENT_SOURCE_DIR} ${_PYTHON})

set_property(TARGET boden_bauer PROPERTY FOLDER "Boden/Bauer")
