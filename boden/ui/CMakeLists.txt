
##########################################################################
# Files

add_sources(_BDN_UI_FILES _BDN_HEADERS ./)
add_sources(_BDN_UI_FILES _BDN_HEADERS ./${BODEN_TARGET})
add_sources(_BDN_UI_FILES _BDN_HEADERS ./test)


if( BODEN_USES_FK )
    add_sources(_BDN_UI_FILES _BDN_HEADERS ./fk)
endif()

if( BODEN_USES_JAVA )
    add_sources(_BDN_UI_FILES _BDN_HEADERS ./java)
    file(GLOB_RECURSE _BDN_JAVA_FILES ./src/java/*.cpp ./java/*.java)
    list(APPEND _BDN_UI_FILES ${_BDN_JAVA_FILES})
endif()

if( BODEN_USES_GTK )
    add_sources(_BDN_UI_FILES _BDN_HEADERS ./gtk)
endif()

if(BODEN_NEEDS_TO_BE_SHARED_LIBRARY OR BODEN_SHARED_LIB)
    add_library(ui SHARED ${_BDN_UI_FILES})
elseif(BODEN_NEEDS_TO_BE_STATIC_LIBRARY OR NOT BODEN_SHARED_LIB)
    add_library(ui STATIC ${_BDN_UI_FILES})
endif()

target_link_libraries(ui PUBLIC foundation)

include(install.cmake)
