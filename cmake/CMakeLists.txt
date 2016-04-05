cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0011 NEW)

if(${CMAKE_VERSION} VERSION_GREATER 3.0)
	cmake_policy(SET CMP0054 NEW)
endif()


project (boden)


if(BODEN_TARGET)
	# do nothing	

else()

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

endif()

message("Boden target is ${BODEN_TARGET}")

set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++11")
set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_SYSTEM_NAME MATCHES "Emscripten")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -s DISABLE_EXCEPTION_CATCHING=0")
endif()

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
endif()

if( "${BODEN_TARGET}" MATCHES "dotnet")
	string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	string(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")

	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /clr:pure /EHa")

	SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /ASSEMBLYDEBUG")

endif()



add_definitions( -DUNICODE -D_UNICODE )

if(CMAKE_SYSTEM_NAME MATCHES "Emscripten")
	add_definitions( -DBDN_TARGET_WEB )

	# the C++ standard library provided by Emscripten has a bug in codecvt.
	# In encodes the data in UTF-8, but when multi-byte sequences are generated
	# then it only counts 1 byte. Thus the end of the resulting string is cut off.
	# To work around that we use our own UTF-8 implementation. We know that the
	# locale encoding is always UTF-8 with Emscripten.
	add_definitions( -DBDN_OVERRIDE_LOCALE_ENCODING_UTF8 )
endif()


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

if( ${BODEN_TARGET} EQUAL "dotnet")
	set_target_properties(TARGET uidemo APPEND PROPERTY VS_DOTNET_REFERENCES "System;System.Core;System.Data;System.Drawing;System.Xml;WindowsBase")
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

if( ${BODEN_TARGET} EQUAL "dotnet")
	set_target_properties(TARGET uidemo APPEND PROPERTY VS_DOTNET_REFERENCES "System;System.Core;System.Data;System.Drawing;System.Xml;WindowsBase")
endif()


