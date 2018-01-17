cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0011 NEW)

if(${CMAKE_VERSION} VERSION_GREATER 3.1)
	cmake_policy(SET CMP0054 NEW)
endif()


project (boden)

SET(BODEN_USES_PTHREAD 0)
SET(BODEN_USES_GTK 0)
SET(BODEN_USES_JAVA 0)
SET(BODEN_USES_FK 0)    # Foundation Kit

if(BODEN_TARGET)
	# do nothing	
	
else()

	if( IOS )
		SET( BODEN_TARGET "ios" )

		SET(BODEN_USES_PTHREAD 1)
        SET(BODEN_USES_FK 1)    # Foundation Kit

		SET( BODEN_TARGET_FAMILY "posix")

	elseif( ANDROID )
		SET( BODEN_TARGET "android" )

		SET(BODEN_USES_PTHREAD 1)
		SET(BODEN_USES_JAVA 1)

	elseif( CMAKE_SYSTEM_NAME STREQUAL "Windows" )	
		SET( BODEN_TARGET "win32" )

	elseif( CMAKE_SYSTEM_NAME STREQUAL "WindowsStore" )	

		if(CMAKE_SYSTEM_VERSION VERSION_LESS "10.0")
			SET( BODEN_TARGET "winstore" )
		else()
			SET( BODEN_TARGET "winuwp" )			
		endif()

		SET( BODEN_TARGET_FAMILY "windows")

	elseif( CMAKE_SYSTEM_NAME STREQUAL "Linux" )	
		SET( BODEN_TARGET "linux" )

		SET(BODEN_USES_PTHREAD 1)
		SET(BODEN_USES_GTK 1)

		SET( BODEN_TARGET_FAMILY "posix")

	elseif( CMAKE_SYSTEM_NAME STREQUAL "Darwin" )	
		SET( BODEN_TARGET "mac" )

		SET(BODEN_USES_PTHREAD 1)
        SET(BODEN_USES_FK 1)    # Foundation Kit

		SET( BODEN_TARGET_FAMILY "posix")

	elseif( CMAKE_SYSTEM_NAME STREQUAL "Emscripten" )		
		SET( BODEN_TARGET "webems" )

	else()
		# assume some Unix variant
		SET( BODEN_TARGET "unix" )    

		SET(BODEN_USES_PTHREAD 1)
		
	endif()

endif()

message("Boden target is ${BODEN_TARGET}")

set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++11")
set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")

set(ANDROID_LIBS "")

if( "${BODEN_TARGET}" STREQUAL "android")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

	set(ANDROID_LIBS "")

else()

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


if( "${BODEN_TARGET}" STREQUAL "winuwp" )	
	SET(VS_WINRT_COMPONENT TRUE)	

    SET(CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION "10.0.10240.0")

    SET(VS_WINRT_REFERENCES "Windows;Windows.UI.Core;System")

    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /ZW")


    # by default the windows headers for the UWP platform define
    # a max macro. And that interferes with std::max. We set a preprocessor
    # switch to remove the max macro
    add_definitions( -DNOMINMAX )
    
endif()

add_definitions( -DUNICODE -D_UNICODE )

if( "${BODEN_TARGET}" STREQUAL "webems" )
	add_definitions( -DBDN_PLATFORM_WEBEMS )

	# the C++ standard library provided by Emscripten has a bug in codecvt.
	# In encodes the data in UTF-8, but when multi-byte sequences are generated
	# then it only counts 1 byte. Thus the end of the resulting string is cut off.
	# To work around that we use our own UTF-8 implementation. We know that the
	# locale encoding is always UTF-8 with Emscripten.
	add_definitions( -DBDN_OVERRIDE_LOCALE_ENCODING_UTF8=1 )

	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s DISABLE_EXCEPTION_CATCHING=0 -s DEMANGLE_SUPPORT=1 -s ALLOW_MEMORY_GROWTH=1")
	SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -s ASSERTIONS=2")

	# for some reason it has no effect if we set the generic CMAKE_EXE_LINKER_FLAGS
	# on linux. We must set the debug/release versions
	SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} --emrun --bind -s EXPORTED_FUNCTIONS=\"['_main', '_bdn_webems_MainDispatcher_timerEventCallback']\"")
	SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} --emrun --bind -s EXPORTED_FUNCTIONS=\"['_main', '_bdn_webems_MainDispatcher_timerEventCallback']\"")

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

file( GLOB BODEN_HEADERS ${BODEN_INCDIR}/bdn/*.h ${BODEN_INCDIR}/bdn/${BODEN_TARGET}/*.h ${BODEN_INCDIR}/bdn/${BODEN_TARGET}/*.hh ${BODEN_INCDIR}/bdn/test/*.h ${BODEN_INCDIR}/bdn/${BODEN_TARGET}/test/*.h ${BODEN_INCDIR}/bdn/${BODEN_TARGET}/test/*.hh)
if(BODEN_TARGET_FAMILY)
	file( GLOB BODEN_FAMILY_HEADERS ${BODEN_INCDIR}/bdn/${BODEN_TARGET_FAMILY}/*.h ${BODEN_INCDIR}/bdn/${BODEN_TARGET_FAMILY}/test/*.h)
	list( APPEND BODEN_HEADERS ${BODEN_FAMILY_HEADERS} )	
endif()

file( GLOB BODEN_SOURCES ${BODEN_SRCDIR}/*.cpp ${BODEN_SRCDIR}/*.h ${BODEN_SRCDIR}/test/*.cpp ${BODEN_SRCDIR}/test/*.h ${BODEN_SRCDIR}/${BODEN_TARGET}/*.cpp ${BODEN_SRCDIR}/${BODEN_TARGET}/*.h ${BODEN_SRCDIR}/${BODEN_TARGET}/*.mm ${BODEN_SRCDIR}/${BODEN_TARGET}/*.m ${BODEN_SRCDIR}/${BODEN_TARGET}/*.hh )
if(BODEN_TARGET_FAMILY)
	file( GLOB BODEN_FAMILY_SOURCES ${BODEN_SRCDIR}/${BODEN_TARGET_FAMILY}/*.cpp ${BODEN_SRCDIR}/${BODEN_TARGET_FAMILY}/*.h ${BODEN_SRCDIR}/${BODEN_TARGET_FAMILY}/*.mm ${BODEN_SRCDIR}/${BODEN_TARGET_FAMILY}/*.m)
	list( APPEND BODEN_SOURCES ${BODEN_FAMILY_SOURCES} )
endif()

if(BODEN_USES_PTHREAD)
	file( GLOB BODEN_PTHREAD_HEADERS ${BODEN_INCDIR}/bdn/pthread/*.h ${BODEN_INCDIR}/bdn/pthread/test/*.h)
	list( APPEND BODEN_HEADERS ${BODEN_PTHREAD_HEADERS} )	

	file( GLOB BODEN_PTHREAD_SOURCES ${BODEN_SRCDIR}/pthread/*.cpp)
	list( APPEND BODEN_SOURCES ${BODEN_PTHREAD_SOURCES} )	
endif()

if(BODEN_USES_FK)
    file( GLOB BODEN_FK_HEADERS ${BODEN_INCDIR}/bdn/fk/*.h ${BODEN_INCDIR}/bdn/fk/*.hh ${BODEN_INCDIR}/bdn/fk/test/*.h ${BODEN_INCDIR}/bdn/fk/test/*.hh)
    list( APPEND BODEN_HEADERS ${BODEN_FK_HEADERS} )   

    file( GLOB BODEN_FK_SOURCES ${BODEN_SRCDIR}/fk/*.cpp ${BODEN_SRCDIR}/fk/*.mm)
    list( APPEND BODEN_SOURCES ${BODEN_FK_SOURCES} )   
endif()

if(BODEN_USES_JAVA)
	file( GLOB BODEN_JAVA_HEADERS ${BODEN_INCDIR}/bdn/java/*.h ${BODEN_INCDIR}/bdn/java/test/*.h)
	list( APPEND BODEN_HEADERS ${BODEN_JAVA_HEADERS} )	

	file( GLOB BODEN_JAVA_SOURCES ${BODEN_SRCDIR}/java/*.cpp)
	list( APPEND BODEN_SOURCES ${BODEN_JAVA_SOURCES} )	
endif()

if(BODEN_USES_GTK)
	file( GLOB BODEN_GTK_HEADERS ${BODEN_INCDIR}/bdn/gtk/*.h ${BODEN_INCDIR}/bdn/gtk/test/*.h)
	list( APPEND BODEN_HEADERS ${BODEN_GTK_HEADERS} )	

	file( GLOB BODEN_GTK_SOURCES ${BODEN_SRCDIR}/gtk/*.cpp)
	list( APPEND BODEN_SOURCES ${BODEN_GTK_SOURCES} )	
endif()

source_group("${BODEN_TARGET} headers" REGULAR_EXPRESSION "\\/(${BODEN_TARGET}|pthread|gtk|fk)\\/.*(\\.h|\\.hh)")
source_group("${BODEN_TARGET} sources" REGULAR_EXPRESSION "\\/(${BODEN_TARGET}|pthread|gtk|fk)\\/.*(\\.cpp|\\.m|\\.mm)")


include_directories( ${BODEN_INCDIR} )
include_directories( ${DEPENDENCIES_INCDIR} )

if( "${BODEN_TARGET}" STREQUAL "winuwp" )   
    # static libraries cannot define Windows Runtime types that are used outside the library.
    # See https://msdn.microsoft.com/en-us/library/hh771041.aspx
    # Since we do that (we have at the very least the application object) that means that we have
    # to create a shared library.
    add_library(boden SHARED ${BODEN_HEADERS} ${BODEN_SOURCES} )

    # MSVC does not export any symbols from a DLL by default.
    # Symbols to export must be marked in the source code with
    # __declspec(dllexport).
    # All other compilers export everything by default.
    # Luckily there is a cmake feature to make VSVC behave like
    # the other compilers. So activate that.
    set_target_properties( boden PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE )
    
elseif( "${BODEN_TARGET}" STREQUAL "android" )
	add_library(boden SHARED ${BODEN_HEADERS} ${BODEN_SOURCES} )

else()
    add_library(boden STATIC ${BODEN_HEADERS} ${BODEN_SOURCES} )

endif()

set_target_properties( ${targetName} PROPERTIES COMPILE_DEFINITIONS "${_oxProps}" )
set_property(TARGET boden APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Debug>:BDN_DEBUG>)


if( BODEN_TARGET STREQUAL "android")
	target_link_libraries(boden "${ANDROID_LIBS}")
endif()




if( BODEN_TARGET STREQUAL "ios")
	# there is a bug in cmake that causes debug symbols to not be generated for ios
	# builds. So we have to set it manually. Unfortunately we can apparently only
	# set it for all builds - waiting for a fix from ios cmake maintainer.
	# XXX needs to be removed for release build
	set_xcode_property(boden GCC_GENERATE_DEBUGGING_SYMBOLS YES)

	target_compile_options(boden PUBLIC "-fobjc-arc")
endif()

if( BODEN_TARGET STREQUAL "mac")
	target_compile_options(boden PUBLIC "-fobjc-arc")
endif()


# testboden

set( TESTBODEN_SRCDIR "../testboden/src")
set( TESTBODEN_COMMON_SRCDIR "../testboden_common/src")
file( GLOB TESTBODEN_SOURCES
    ${TESTBODEN_SRCDIR}/*.cpp
    ${TESTBODEN_SRCDIR}/*.h
    ${TESTBODEN_SRCDIR}/${BODEN_TARGET}/*.cpp
    ${TESTBODEN_SRCDIR}/${BODEN_TARGET}/*.h
    ${TESTBODEN_COMMON_SRCDIR}/*.cpp
    ${TESTBODEN_COMMON_SRCDIR}/*.h
    ${TESTBODEN_COMMON_SRCDIR}/${BODEN_TARGET}/*.cpp
    ${TESTBODEN_COMMON_SRCDIR}/${BODEN_TARGET}/*.mm
    ${TESTBODEN_COMMON_SRCDIR}/${BODEN_TARGET}/*.h
    ${TESTBODEN_COMMON_SRCDIR}/${BODEN_TARGET}/*.hh
    )
include_directories( ${BODEN_INCDIR} )
include_directories( ${DEPENDENCIES_INCDIR} )

# we cannot build commandline tools on iOS and UWP. So we have to build a UI app
if((BODEN_TARGET STREQUAL "ios") OR (BODEN_TARGET STREQUAL "winuwp") )
	add_executable(testboden WIN32 MACOSX_BUNDLE ${TESTBODEN_SOURCES} )

elseif( "${BODEN_TARGET}" STREQUAL "android" )
	add_library(testboden SHARED ${TESTBODEN_SOURCES} )

else()
	add_executable(testboden ${TESTBODEN_SOURCES} )
endif()

set_property(TARGET testboden APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Debug>:BDN_DEBUG>)
set_property(TARGET testboden APPEND PROPERTY COMPILE_DEFINITIONS BDN_COMPILING_COMMANDLINE_APP)

if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")

    get_filename_component(WEBEMS_SHELL_FILE "../uishell.html" ABSOLUTE)

	# make sure that we also generate a HTML file
	set_property(TARGET testboden APPEND PROPERTY LINK_FLAGS "-o testboden.html --shell-file ${WEBEMS_SHELL_FILE}")
endif()

target_link_libraries(testboden boden)

if( BODEN_TARGET STREQUAL "win32")
    target_link_libraries(testboden Shcore)
endif()

if( BODEN_TARGET STREQUAL "android")
	target_link_libraries(testboden "${ANDROID_LIBS}")
endif()

if( BODEN_TARGET STREQUAL "mac")
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

	target_compile_options(testboden PUBLIC "-fobjc-arc")
	
endif()


if( BODEN_TARGET STREQUAL "mac")
	target_compile_options(testboden PUBLIC "-fobjc-arc")	
endif()

# testbodenui

set( TESTBODENUI_SRCDIR "../testbodenui/src")
file( GLOB TESTBODENUI_SOURCES
    ${TESTBODENUI_SRCDIR}/*.cpp
    ${TESTBODENUI_SRCDIR}/${BODEN_TARGET}/*.cpp
    ${TESTBODENUI_SRCDIR}/${BODEN_TARGET}/*.mm
    ${TESTBODENUI_SRCDIR}/*.h
    ${TESTBODENUI_SRCDIR}/${BODEN_TARGET}/*.h
    ${TESTBODENUI_SRCDIR}/${BODEN_TARGET}/*.hh
    ${TESTBODEN_COMMON_SRCDIR}/*.cpp
    ${TESTBODEN_COMMON_SRCDIR}/*.h
    ${TESTBODEN_COMMON_SRCDIR}/${BODEN_TARGET}/*.cpp
    ${TESTBODEN_COMMON_SRCDIR}/${BODEN_TARGET}/*.mm
    ${TESTBODEN_COMMON_SRCDIR}/${BODEN_TARGET}/*.h
    ${TESTBODEN_COMMON_SRCDIR}/${BODEN_TARGET}/*.hh
     )


if(BODEN_USES_GTK)
	file( GLOB TESTBODENUI_GTK_SOURCES ${TESTBODENUI_SRCDIR}/gtk/*.cpp)
	list( APPEND TESTBODENUI_SOURCES ${TESTBODENUI_GTK_SOURCES} )	
endif()

include_directories( ${BODEN_INCDIR}  )
include_directories( ${DEPENDENCIES_INCDIR} )

IF ( APPLE )
	SET(APP_TYPE MACOSX_BUNDLE)
	SET(MACOSX_BUNDLE_BUNDLE_NAME testbodenui)
	SET(MACOSX_BUNDLE_GUI_IDENTIFIER "io.boden.testbodenui")
endif()

if( "${BODEN_TARGET}" STREQUAL "android" )
	add_library(testbodenui SHARED ${TESTBODENUI_SOURCES} )

else()
    add_executable(testbodenui  WIN32 MACOSX_BUNDLE ${TESTBODENUI_SOURCES} )
    
endif()

set_property(TARGET testbodenui APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Debug>:BDN_DEBUG>)

if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    get_filename_component(WEBEMS_SHELL_FILE "../uishell.html" ABSOLUTE)

	# make sure that we also generate a HTML file
	set_property(TARGET testbodenui APPEND PROPERTY LINK_FLAGS "-o testbodenui.html --shell-file ${WEBEMS_SHELL_FILE}")
endif()

target_link_libraries(testbodenui boden)

if( BODEN_TARGET STREQUAL "win32")
    target_link_libraries(testbodenui Shcore)
endif()

if( BODEN_TARGET STREQUAL "mac")
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

	target_compile_options(testbodenui PUBLIC "-fobjc-arc")

endif()

if( BODEN_TARGET STREQUAL "mac")
	target_compile_options(testbodenui PUBLIC "-fobjc-arc")
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


if( BODEN_TARGET STREQUAL "android")
	target_link_libraries(testbodenui "${ANDROID_LIBS}")
endif()


# uidemo

set( UIDEMO_SRCDIR "../uidemo/src")
file( GLOB UIDEMO_SOURCES ${UIDEMO_SRCDIR}/*.cpp ${UIDEMO_SRCDIR}/*.h)
include_directories( ${BODEN_INCDIR} )
include_directories( ${DEPENDENCIES_INCDIR} )

IF ( APPLE )
	SET(APP_TYPE MACOSX_BUNDLE)
	SET(MACOSX_BUNDLE_BUNDLE_NAME uidemo)
	SET(MACOSX_BUNDLE_GUI_IDENTIFIER "io.boden.uidemo")
endif()

if( "${BODEN_TARGET}" STREQUAL "android" )
	add_library(uidemo SHARED ${UIDEMO_SOURCES} )
else()
	add_executable(uidemo WIN32 MACOSX_BUNDLE ${UIDEMO_SOURCES} )
endif()

if( BODEN_TARGET STREQUAL "winuwp")
    set_property(TARGET uidemo APPEND PROPERTY LINK_FLAGS  "/OPT:NOREF /OPT:NOICF /DEBUG:FASTLINK")
endif()


if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
	# make sure that we also generate a HTML file

	get_filename_component(WEBEMS_SHELL_FILE "../uishell.html" ABSOLUTE)

	set_property(TARGET uidemo APPEND PROPERTY LINK_FLAGS "-o uidemo.html --shell-file ${WEBEMS_SHELL_FILE}")

endif()

target_link_libraries(uidemo boden)

if( BODEN_TARGET STREQUAL "win32")
    target_link_libraries(uidemo Shcore)
endif()

if( BODEN_TARGET STREQUAL "mac")
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

	target_compile_options(uidemo PUBLIC "-fobjc-arc")
	
endif()

if( BODEN_TARGET STREQUAL "mac")
	target_compile_options(uidemo PUBLIC "-fobjc-arc")	
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

if( BODEN_TARGET STREQUAL "android")
	target_link_libraries(uidemo "${ANDROID_LIBS}")
endif()


