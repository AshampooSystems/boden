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

		SET( BODEN_TARGET_FAMILY "posix")

	elseif( ANDROID )
		SET( BODEN_TARGET "android" )

	elseif( CMAKE_SYSTEM_NAME STREQUAL "Windows" )	
		SET( BODEN_TARGET "windows-classic" )

	elseif( CMAKE_SYSTEM_NAME STREQUAL "WindowsStore" )	

		if(CMAKE_SYSTEM_VERSION STREQUAL "10.0")
			SET( BODEN_TARGET "windows-universal" )
		else()
			SET( BODEN_TARGET "windows-store" )
		endif()

		SET( BODEN_TARGET_FAMILY "windows")

	elseif( CMAKE_SYSTEM_NAME STREQUAL "Linux" )	
		SET( BODEN_TARGET "linux" )

		SET( BODEN_TARGET_FAMILY "posix")

	elseif( CMAKE_SYSTEM_NAME STREQUAL "Darwin" )	
		SET( BODEN_TARGET "osx" )

		SET( BODEN_TARGET_FAMILY "posix")

	elseif( CMAKE_SYSTEM_NAME STREQUAL "Emscripten" )		
		SET( BODEN_TARGET "web" )

	else()
		# assume some Unix variant
		SET( BODEN_TARGET "unix" )    
		
	endif()

endif()

message("Boden target is ${BODEN_TARGET}")

set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++11")
set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")

if(CMAKE_COMPILER_IS_GNUCXX
	OR CMAKE_SYSTEM_NAME STREQUAL "Emscripten"
	OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" # clang on non-mac system
  )

	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

	if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		# use libc++ instead of libstdc++. It is far less buggy.
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
	endif()
endif()



if( "${BODEN_TARGET}" STREQUAL "linux")
	if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		# turn of this warning: 'register' storage class specifier is deprecated
		# It occurs in the GTK headers and we do not want to see it everywhere.

		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-register")
	endif()
endif()


if("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
endif()

if( "${BODEN_TARGET}" STREQUAL "dotnet")
	string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	string(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")

	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /clr:pure /EHa")

	SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /ASSEMBLYDEBUG")

endif()


if( CMAKE_SYSTEM_NAME STREQUAL "WindowsStore" )	
	SET(VS_WINRT_COMPONENT "TRUE")
endif()


add_definitions( -DUNICODE -D_UNICODE )

if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
	add_definitions( -DBDN_PLATFORM_WEB )

	# the C++ standard library provided by Emscripten has a bug in codecvt.
	# In encodes the data in UTF-8, but when multi-byte sequences are generated
	# then it only counts 1 byte. Thus the end of the resulting string is cut off.
	# To work around that we use our own UTF-8 implementation. We know that the
	# locale encoding is always UTF-8 with Emscripten.
	add_definitions( -DBDN_OVERRIDE_LOCALE_ENCODING_UTF8 )

	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s DISABLE_EXCEPTION_CATCHING=0")

	
	SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --emrun")

endif()


set( WEBDRIVERXX_INCDIR "../3rdparty/webdriverxx/include")
set( CURL_INCDIR "../3rdparty/curl/include")

set( DEPENDENCIES_INCDIR ${WEBDRIVERXX_INCDIR} ${CURL_INCDIR} )


if( BODEN_TARGET STREQUAL "linux")
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
	
	include_directories(${GTK3_INCLUDE_DIRS})
	link_directories(${GTK3_LIBRARY_DIRS})
	
	# Add other flags to the compiler
	add_definitions(${GTK3_CFLAGS_OTHER})

endif()



# boden

set( BODEN_INCDIR "../boden/include")
set( BODEN_SRCDIR "../boden/src")

file( GLOB_RECURSE BODEN_HEADERS ${BODEN_INCDIR}/common/*.h ${BODEN_INCDIR}/${BODEN_TARGET}/*.h ${BODEN_INCDIR}/${BODEN_TARGET}/*.hh)
if(BODEN_TARGET_FAMILY)
	file( GLOB_RECURSE BODEN_FAMILY_HEADERS ${BODEN_INCDIR}/${BODEN_TARGET_FAMILY}/*.h)
	list( APPEND BODEN_HEADERS ${BODEN_FAMILY_HEADERS} )
endif()

file( GLOB_RECURSE BODEN_SOURCES ${BODEN_SRCDIR}/common/*.cpp ${BODEN_SRCDIR}/common/*.h ${BODEN_SRCDIR}/${BODEN_TARGET}/*.cpp ${BODEN_SRCDIR}/${BODEN_TARGET}/*.h ${BODEN_SRCDIR}/${BODEN_TARGET}/*.mm ${BODEN_SRCDIR}/${BODEN_TARGET}/*.m)
if(BODEN_TARGET_FAMILY)
	file( GLOB_RECURSE BODEN_FAMILY_SOURCES ${BODEN_SRCDIR}/${BODEN_TARGET_FAMILY}/*.cpp ${BODEN_SRCDIR}/${BODEN_TARGET_FAMILY}/*.h ${BODEN_SRCDIR}/${BODEN_TARGET_FAMILY}/*.mm ${BODEN_SRCDIR}/${BODEN_TARGET_FAMILY}/*.m)
	list( APPEND BODEN_SOURCES ${BODEN_FAMILY_SOURCES} )
endif()

include_directories( ${BODEN_INCDIR}/common ${BODEN_INCDIR}/${BODEN_TARGET} )
if(BODEN_TARGET_FAMILY)
	include_directories( ${BODEN_INCDIR}/${BODEN_TARGET_FAMILY} )
endif()
include_directories( ${DEPENDENCIES_INCDIR} )


add_library(boden STATIC ${BODEN_HEADERS} ${BODEN_SOURCES} )

set_property(TARGET boden APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Debug>:BDN_DEBUG>)

if( BODEN_TARGET STREQUAL "ios")
	# there is a bug in cmake that causes debug symbols to not be generated for ios
	# builds. So we have to set it manually. Unfortunately we can apparently only
	# set it for all builds - waiting for a fix from ios cmake maintainer.
	# XXX needs to be removed for release build
	set_xcode_property(boden GCC_GENERATE_DEBUGGING_SYMBOLS YES)
endif()



# testboden

set( TESTBODEN_SRCDIR "../testboden/src")
file( GLOB_RECURSE TESTBODEN_SOURCES ${TESTBODEN_SRCDIR}/*.cpp ${TESTBODEN_SRCDIR}/*.h)
include_directories( ${BODEN_INCDIR}/common ${BODEN_INCDIR}/${BODEN_TARGET} )
if(BODEN_TARGET_FAMILY)
	include_directories( ${BODEN_INCDIR}/${BODEN_TARGET_FAMILY} )
endif()
include_directories( ${DEPENDENCIES_INCDIR} )

# we cannot build commandline tools on iOS. So we have to build a UI app
if(BODEN_TARGET STREQUAL "ios")
	add_executable(testboden WIN32 MACOSX_BUNDLE ${TESTBODEN_SOURCES} )
else()
	add_executable(testboden ${TESTBODEN_SOURCES} )
endif()

set_property(TARGET testboden APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Debug>:BDN_DEBUG>)

if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
	# make sure that we also generate a HTML file
	set_property(TARGET testboden APPEND PROPERTY LINK_FLAGS "-o testboden.html")
endif()

target_link_libraries(testboden boden)


if( BODEN_TARGET STREQUAL "osx")
	find_library(COCOA_LIBRARY Cocoa)
	mark_as_advanced(COCOA_LIBRARY)
	target_link_libraries(testboden ${COCOA_LIBRARY})
endif()

if( BODEN_TARGET STREQUAL "ios")
	# there is a bug in cmake that causes debug symbols to not be generated for ios
	# builds. So we have to set it manually. Unfortunately we can apparently only
	# set it for all builds - waiting for a fix from ios cmake maintainer.
	# XXX needs to be removed for release build
	set_xcode_property(testboden GCC_GENERATE_DEBUGGING_SYMBOLS YES)
endif()

if( BODEN_TARGET STREQUAL "dotnet")
	set_target_properties(TARGET testboden APPEND PROPERTY VS_DOTNET_REFERENCES "System;System.Core;System.Data;System.Drawing;System.Xml;WindowsBase")
endif()


if( BODEN_TARGET STREQUAL "linux")
	target_link_libraries(testboden ${GTK3_LIBRARIES} pthread)
endif()

if( BODEN_TARGET STREQUAL "ios")
	find_library(FOUNDATION_LIBRARY Foundation)
	mark_as_advanced(FOUNDATION_LIBRARY)

	find_library(UIKIT_LIBRARY UIKit)
	mark_as_advanced(UIKIT_LIBRARY)	

	target_link_libraries(testboden ${FOUNDATION_LIBRARY} ${UIKIT_LIBRARY} )
	
endif()


# testbodenui

set( TESTBODENUI_SRCDIR "../testbodenui/src")
file( GLOB_RECURSE TESTBODENUI_SOURCES ${TESTBODENUI_SRCDIR}/common/*.cpp  ${TESTBODENUI_SRCDIR}/${BODEN_TARGET}/*.cpp ${TESTBODENUI_SRCDIR}/common/*.h ${TESTBODENUI_SRCDIR}/${BODEN_TARGET}/*.h )
include_directories( ${BODEN_INCDIR}/common ${BODEN_INCDIR}/${BODEN_TARGET} )
if(BODEN_TARGET_FAMILY)
	include_directories( ${BODEN_INCDIR}/${BODEN_TARGET_FAMILY} )
endif()
include_directories( ${DEPENDENCIES_INCDIR} )

IF ( APPLE )
	SET(APP_TYPE MACOSX_BUNDLE)
	SET(MACOSX_BUNDLE_BUNDLE_NAME testbodenui)
	SET(MACOSX_BUNDLE_GUI_IDENTIFIER "io.boden.testbodenui")
endif()


# we cannot build commandline tools on iOS. So we have to build a UI app
if(BODEN_TARGET STREQUAL "ios")
    add_executable(testbodenui WIN32 MACOSX_BUNDLE ${TESTBODENUI_SOURCES} )
else()
    add_executable(testbodenui ${TESTBODENUI_SOURCES} )
endif()

set_property(TARGET testbodenui APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Debug>:BDN_DEBUG>)

if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
	# make sure that we also generate a HTML file
	set_property(TARGET testbodenui APPEND PROPERTY LINK_FLAGS "-o testbodenui.html")
endif()

target_link_libraries(testbodenui boden)

if( BODEN_TARGET STREQUAL "osx")
	find_library(COCOA_LIBRARY Cocoa)
	mark_as_advanced(COCOA_LIBRARY)
	target_link_libraries(testbodenui ${COCOA_LIBRARY})
endif()

if( BODEN_TARGET STREQUAL "ios")
	find_library(FOUNDATION_LIBRARY Foundation)
	mark_as_advanced(FOUNDATION_LIBRARY)

	find_library(UIKIT_LIBRARY UIKit)
	mark_as_advanced(UIKIT_LIBRARY)	

	target_link_libraries(testbodenui ${FOUNDATION_LIBRARY} ${UIKIT_LIBRARY} )
endif()

if( BODEN_TARGET STREQUAL "dotnet")
	set_target_properties(TARGET testbodenui APPEND PROPERTY VS_DOTNET_REFERENCES "System;System.Core;System.Data;System.Drawing;System.Xml;WindowsBase")
endif()

if( BODEN_TARGET STREQUAL "linux")
	target_link_libraries(testbodenui ${GTK3_LIBRARIES} pthread)
endif()

if( BODEN_TARGET STREQUAL "ios")
	# there is a bug in cmake that causes debug symbols to not be generated for ios
	# builds. So we have to set it manually. Unfortunately we can apparently only
	# set it for all builds - waiting for a fix from ios cmake maintainer.
	# XXX needs to be removed for release build
	set_xcode_property(testbodenui GCC_GENERATE_DEBUGGING_SYMBOLS YES)
endif()


# uidemo

set( UIDEMO_SRCDIR "../uidemo/src")
file( GLOB_RECURSE UIDEMO_SOURCES ${UIDEMO_SRCDIR}/*.cpp ${UIDEMO_SRCDIR}/*.h)
include_directories( ${BODEN_INCDIR}/common ${BODEN_INCDIR}/${BODEN_TARGET} )
if(BODEN_TARGET_FAMILY)
	include_directories( ${BODEN_INCDIR}/${BODEN_TARGET_FAMILY} )
endif()
include_directories( ${DEPENDENCIES_INCDIR} )

IF ( APPLE )
	SET(APP_TYPE MACOSX_BUNDLE)
	SET(MACOSX_BUNDLE_BUNDLE_NAME uidemo)
	SET(MACOSX_BUNDLE_GUI_IDENTIFIER "io.boden.uidemo")
endif()


add_executable(uidemo WIN32 MACOSX_BUNDLE ${UIDEMO_SOURCES} )

if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
	# make sure that we also generate a HTML file
	set_property(TARGET uidemo APPEND PROPERTY LINK_FLAGS "-o uidemo.html")
endif()

target_link_libraries(uidemo boden)

if( BODEN_TARGET STREQUAL "osx")
	find_library(COCOA_LIBRARY Cocoa)
	mark_as_advanced(COCOA_LIBRARY)
	target_link_libraries(uidemo ${COCOA_LIBRARY})
endif()

if( BODEN_TARGET STREQUAL "ios")
	find_library(FOUNDATION_LIBRARY Foundation)
	mark_as_advanced(FOUNDATION_LIBRARY)

	find_library(UIKIT_LIBRARY UIKit)
	mark_as_advanced(UIKIT_LIBRARY)	

	target_link_libraries(uidemo ${FOUNDATION_LIBRARY} ${UIKIT_LIBRARY} )
	
endif()

if( BODEN_TARGET STREQUAL "dotnet")
	set_target_properties(TARGET uidemo APPEND PROPERTY VS_DOTNET_REFERENCES "System;System.Core;System.Data;System.Drawing;System.Xml;WindowsBase")
endif()

if( BODEN_TARGET STREQUAL "linux")
	target_link_libraries(uidemo ${GTK3_LIBRARIES} pthread)
endif()

if( BODEN_TARGET STREQUAL "ios")
	# there is a bug in cmake that causes debug symbols to not be generated for ios
	# builds. So we have to set it manually. Unfortunately we can apparently only
	# set it for all builds - waiting for a fix from ios cmake maintainer.
	# XXX needs to be removed for release build
	set_xcode_property(uidemo GCC_GENERATE_DEBUGGING_SYMBOLS YES)
endif()





