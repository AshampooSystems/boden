# json

option(BODEN_USE_EXTERNAL_JSON "Use an external JSON library" OFF)

if(BODEN_USE_EXTERNAL_JSON)
  find_package(nlohmann_json 3.2.0 REQUIRED)
else()
  set(JSON_BuildTests OFF CACHE INTERNAL "")
  add_subdirectory(json)
endif()

target_compile_definitions(nlohmann_json INTERFACE BDN_HAS_NLOHMANN_JSON)

# Yoga

file(GLOB yogacore_SRC yoga/yoga/*.cpp)

if(BDN_NEEDS_TO_BE_SHARED_LIBRARY OR BDN_SHARED_LIB)
    add_library(yogacore SHARED ${yogacore_SRC})
elseif(BDN_NEEDS_TO_BE_STATIC_LIBRARY OR NOT BDN_SHARED_LIB)
    add_library(yogacore STATIC ${yogacore_SRC})
endif()

target_include_directories(yogacore INTERFACE ${CMAKE_CURRENT_LIST_DIR}/yoga)

if(ANDROID)
    find_library(ANDROID_LOG_LIB log)
    mark_as_advanced(ANDROID_LOG_LIB)
    target_link_libraries(yogacore PUBLIC ${ANDROID_LOG_LIB})
endif()
