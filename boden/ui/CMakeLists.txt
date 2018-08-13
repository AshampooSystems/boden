
##########################################################################
# Files

add_sources(_BDN_UI_FILES _BDN_HEADERS ./)
add_sources(_BDN_UI_FILES _BDN_HEADERS ./${BDN_TARGET})
add_sources(_BDN_UI_FILES _BDN_HEADERS ./test)


if( BDN_USES_FK )
    add_sources(_BDN_UI_FILES _BDN_HEADERS ./fk)
endif()

if( BDN_USES_JAVA )
    add_sources(_BDN_UI_FILES _BDN_HEADERS ./java)
    file(GLOB_RECURSE _BDN_JAVA_FILES ./src/java/*.cpp ./java/*.java)
    list(APPEND _BDN_UI_FILES ${_BDN_JAVA_FILES})
endif()

if( BDN_USES_GTK )
    add_sources(_BDN_UI_FILES _BDN_HEADERS ./gtk)
endif()

if(BDN_NEEDS_TO_BE_SHARED_LIBRARY OR BDN_SHARED_LIB)
    add_library(ui SHARED ${_BDN_UI_FILES})
elseif(BDN_NEEDS_TO_BE_STATIC_LIBRARY OR NOT BDN_SHARED_LIB)
    add_library(ui STATIC ${_BDN_UI_FILES})
endif()

target_link_libraries(ui PUBLIC foundation)

add_clangformat(ui)

include(install.cmake)
