cmake_minimum_required(VERSION 3.0)
cmake_policy(SET CMP0011 NEW)
cmake_policy(SET CMP0054 NEW)

project (boden)

if( IOS )
	SET( BODEN_TARGET "ios" )

elseif( ANDROID )
	SET( BODEN_TARGET "android" )

elseif( CMAKE_SYSTEM_NAME MATCHES "Windows" )	
	SET( BODEN_TARGET "windows" )

elseif( CMAKE_SYSTEM_NAME MATCHES "Darwin" )	
	SET( BODEN_TARGET "osx" )

elseif( CMAKE_SYSTEM_NAME MATCHES "Emscripten" )		
	SET( BODEN_TARGET "web" )

else()
	# assume some Unix variant
	SET( BODEN_TARGET "unix" )    
	
endif()


if(CMAKE_COMPILER_IS_GNUCXX OR (CMAKE_CXX_COMPILER_ID MATCHES "Apple"))
	SET(CMAKE_CXX_FLAGS "-std=c++0x")
endif()

set( BODEN_INCDIR "../boden/include")
set( BODEN_SRCDIR "../boden/src")

file( GLOB_RECURSE BODEN_HEADERS ${BODEN_INCDIR}/common/*.h ${BODEN_INCDIR}/${BODEN_TARGET}/*.h)
file( GLOB_RECURSE BODEN_SOURCES ${BODEN_SRCDIR}/common/*.cpp ${BODEN_SRCDIR}/common/*.h ${BODEN_SRCDIR}/${BODEN_TARGET}/*.cpp ${BODEN_SRCDIR}/${BODEN_TARGET}/*.h ${BODEN_SRCDIR}/${BODEN_TARGET}/*.mm ${BODEN_SRCDIR}/${BODEN_TARGET}/*.m)

include_directories( ${BODEN_INCDIR}/common ${BODEN_INCDIR}/${BODEN_TARGET} )

add_library(boden STATIC ${BODEN_HEADERS} ${BODEN_SOURCES} )


set( UIDEMO_SRCDIR "../uidemo/src")

file( GLOB_RECURSE UIDEMO_SOURCES ${UIDEMO_SRCDIR}/*.cpp ${UIDEMO_SRCDIR}/*.h)

include_directories( ${BODEN_INCDIR}/common ${BODEN_INCDIR}/${BODEN_TARGET} )

add_executable(uidemo ${UIDEMO_SOURCES} )

target_link_libraries(uidemo boden)

if(APPLE)

	find_library(COCOA_LIBRARY Cocoa)
	mark_as_advanced(COCOA_LIBRARY)
	target_link_libraries(uidemo ${COCOA_LIBRARY})

endif()

