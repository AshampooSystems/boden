# Writing a View module

## Intro

Boden views consist of two parts:

1. The outward facing View class
2. The inner platform specific ViewCore implementation

This guide will use the LottieView class implementation to highlight the specific steps needed to implement new Views. 

## Module structure

A View module consists of the public part ( Mostly the View class ), and a number of platform specific implementations:

```
lottieview
├── include
│── src
├── platforms
│  ├── android
│  │  ├── include
│  │  ├── java
│  │  └── src
│  ├── ios
│  │  ├── include
│  │  └── src
│  └── mac
│     ├── include
│     └── src
```

## Main CMakeLists.txt

??? note "Full CMakeLists.txt"
	```cmake
	add_sources(_BDN_LOTTIEVIEW_FILES _BDN_LOTTIEVIEW_HEADERS ./)
	GenerateTopLevelIncludeFile(_BDN_LOTTIEVIEW_COMBINED
	    ${CMAKE_CURRENT_BINARY_DIR}/include/bdn/lottieview-module.h
	    ${CMAKE_CURRENT_LIST_DIR}/include
	    ${_BDN_LOTTIEVIEW_HEADERS})
	list(APPEND _BDN_LOTTIEVIEW_FILES
	    ${_BDN_LOTTIEVIEW_COMBINED}
	    ${_BDN_LOTTIEVIEW_HEADERS})
	add_universal_library(lottieview TIDY SOURCES ${_BDN_LOTTIEVIEW_FILES})
	target_link_libraries(lottieview PUBLIC foundation ui)
	target_include_directories(lottieview
	    PUBLIC
	    $<INSTALL_INTERFACE:include>
	    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	    )
	target_include_directories(lottieview PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>)
	include(install.cmake)
	add_subdirectory(platforms)
	add_library(Boden::LottieView ALIAS lottieview)
	target_link_libraries(Boden_All INTERFACE lottieview)
	```

```cmake
add_sources(_BDN_LOTTIEVIEW_FILES _BDN_LOTTIEVIEW_HEADERS ./)
```

`add_sources` Gathers all files from the ./include and ./src folders.

```cmake
GenerateTopLevelIncludeFile(_BDN_LOTTIEVIEW_COMBINED
    ${CMAKE_CURRENT_BINARY_DIR}/include/bdn/lottieview-module.h
    ${CMAKE_CURRENT_LIST_DIR}/include
    ${_BDN_LOTTIEVIEW_HEADERS})
```

`GenerateTopLevelIncludeFile` creates a convenience header file that includes all files from the ./include folder

```cmake
list(APPEND _BDN_LOTTIEVIEW_FILES
    ${_BDN_LOTTIEVIEW_COMBINED}
    ${_BDN_LOTTIEVIEW_HEADERS})
add_universal_library(lottieview TIDY SOURCES ${_BDN_LOTTIEVIEW_FILES})
```

All files are then combined and the main library is created. The ```TIDY``` option includes the source files in clang-tidy runs.

```cmake
target_link_libraries(lottieview PUBLIC Boden::UI)
target_include_directories(lottieview
    PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )
target_include_directories(lottieview PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>)
include(install.cmake)
```

We link the library against the `Boden::UI` library. The include directories are specified here such that they work whether Boden is included in source form or as a bundled installation

Now you can write your Views as described in [Writing a new View](writing_view.md). 


