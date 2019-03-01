cmake_minimum_required(VERSION 3.6)

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_POSITION_INDEPENDENT_CODE ON )

project({project_name})

set(_sources 
	./source/AppController.h
	./source/AppController.cpp
	./source/MainViewController.h
	./source/MainViewController.cpp
	./source/main.cpp
)

set(MACOSX_BUNDLE_BUNDLE_NAME {project_name})
set(MACOSX_BUNDLE_GUI_IDENTIFIER "io.mycompany.{project_name}")

set(BDN_BUILD_TESTS OFF CACHE BOOL "Don't include boden tests")
set(BDN_BUILD_EXAMPLES OFF CACHE BOOL "Don't include boden examples")

add_subdirectory({boden_relative_path} boden)

add_universal_executable({project_name} NO ${_sources})

target_link_libraries({project_name} ui yogalayout)
