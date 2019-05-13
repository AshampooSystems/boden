file(GLOB _PYTHON ./*.py ./*.txt)

add_custom_target(boden_roger SOURCES ${_PYTHON})

group_sources_automatically(${CMAKE_CURRENT_SOURCE_DIR} ${_PYTHON})

