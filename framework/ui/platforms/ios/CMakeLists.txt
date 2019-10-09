cmake_minimum_required(VERSION 3.13.5)

add_platform_library(NAME ios SOURCE_FOLDER ${CMAKE_CURRENT_LIST_DIR} COMPONENT_NAME IOS PARENT_LIBRARY ui )

target_link_options(ui_ios INTERFACE -ObjC)





