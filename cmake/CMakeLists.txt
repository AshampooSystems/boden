cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0011 NEW)

if(${CMAKE_VERSION} VERSION_GREATER 3.0)
	cmake_policy(SET CMP0054 NEW)
endif()


project (boden)


if( IOS )
	SET( BODEN_TARGET "ios" )

elseif( ANDROID )
	SET( BODEN_TARGET "android" )

elseif( CMAKE_SYSTEM_NAME MATCHES "Windows" )	
	SET( BODEN_TARGET "windows" )

elseif( CMAKE_SYSTEM_NAME MATCHES "Linux" )	
	SET( BODEN_TARGET "linux" )

elseif( CMAKE_SYSTEM_NAME MATCHES "Darwin" )	
	SET( BODEN_TARGET "osx" )

elseif( CMAKE_SYSTEM_NAME MATCHES "Emscripten" )		
	SET( BODEN_TARGET "web" )

else()
	# assume some Unix variant
	SET( BODEN_TARGET "unix" )    
	
endif()


set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++11")
set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_SYSTEM_NAME MATCHES "Emscripten")
	SET(CMAKE_CXX_FLAGS "-std=c++11")
endif()


add_definitions( -DUNICODE -D_UNICODE )

set( BODEN_INCDIR "../boden/include")
set( BODEN_SRCDIR "../boden/src")

file( GLOB_RECURSE BODEN_HEADERS ${BODEN_INCDIR}/common/*.h ${BODEN_INCDIR}/${BODEN_TARGET}/*.h)

file( GLOB_RECURSE BODEN_SOURCES ${BODEN_SRCDIR}/common/*.cpp ${BODEN_SRCDIR}/common/*.h ${BODEN_SRCDIR}/${BODEN_TARGET}/*.cpp ${BODEN_SRCDIR}/${BODEN_TARGET}/*.h ${BODEN_SRCDIR}/${BODEN_TARGET}/*.mm ${BODEN_SRCDIR}/${BODEN_TARGET}/*.m)

include_directories( ${BODEN_INCDIR}/common ${BODEN_INCDIR}/${BODEN_TARGET} )

add_library(boden STATIC ${BODEN_HEADERS} ${BODEN_SOURCES} )

set_property(TARGET boden APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Debug>:BDN_DEBUG>)


set( UIDEMO_SRCDIR "../uidemo/src")
file( GLOB_RECURSE UIDEMO_SOURCES ${UIDEMO_SRCDIR}/*.cpp ${UIDEMO_SRCDIR}/*.h)
include_directories( ${BODEN_INCDIR}/common ${BODEN_INCDIR}/${BODEN_TARGET} )

add_executable(uidemo WIN32 MACOSX_BUNDLE ${UIDEMO_SOURCES} )

if(CMAKE_SYSTEM_NAME MATCHES "Emscripten")
	# make sure that we also generate a HTML file
	set_property(TARGET uidemo APPEND PROPERTY LINK_FLAGS "-o uidemo.html")
endif()

target_link_libraries(uidemo boden)
if(APPLE)
	find_library(COCOA_LIBRARY Cocoa)
	mark_as_advanced(COCOA_LIBRARY)
	target_link_libraries(uidemo ${COCOA_LIBRARY})
endif()



set( BODENTEST_SRCDIR "../bodentest/src")
file( GLOB_RECURSE BODENTEST_SOURCES ${BODENTEST_SRCDIR}/*.cpp ${BODENTEST_SRCDIR}/*.h)
include_directories( ${BODEN_INCDIR}/common ${BODEN_INCDIR}/${BODEN_TARGET} )


add_executable(bodentest ${BODENTEST_SOURCES} )

set_property(TARGET bodentest APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Debug>:BDN_DEBUG>)

if(CMAKE_SYSTEM_NAME MATCHES "Emscripten")
	# make sure that we also generate a HTML file
	set_property(TARGET bodentest APPEND PROPERTY LINK_FLAGS "-o bodentest.html")
endif()

target_link_libraries(bodentest boden)

if(APPLE)

	find_library(COCOA_LIBRARY Cocoa)
	mark_as_advanced(COCOA_LIBRARY)
	target_link_libraries(bodentest ${COCOA_LIBRARY})

endif()

