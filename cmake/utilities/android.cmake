# Clear android dependencies in the cache

set(BAUER_ANDROID_DEPENDENCIES "" CACHE STRING "The android dependencies" FORCE)
set(BAUER_ANDROID_MANIFEST_FILE "" CACHE STRING "The android manifest file" FORCE)
set(BAUER_ANDROID_EXTRA_JAVA_DIRECTORIES "" CACHE STRING "Additional Java Directories" FORCE)


macro(android_manifest TARGET_NAME)
    if(BDN_PLATFORM_ANDROID)
        if("${BAUER_ANDROID_MANIFEST_FILE}" STREQUAL "")
            get_target_property(BODEN_CMAKE_SOURCE_DIR boden_cmake SOURCE_DIR)
            set(BAUER_ANDROID_MANIFEST_FILE "${BODEN_CMAKE_SOURCE_DIR}/templates/App.AndroidManifest.xml.in")
        endif()

        if("${ANDROID_MODULE_NAME}" STREQUAL "")
            set(ANDROID_MODULE_NAME ${TARGET_NAME})
        endif()

        if("${ANDROID_PACKAGEID}" STREQUAL "")
            set(ANDROID_PACKAGEID "io.boden.android.${TARGET_NAME}")
        endif()

        if("${ANDROID_THEME}" STREQUAL "")
            set(ANDROID_THEME "@style/Theme.AppCompat.DayNight")
        endif()

        get_target_property(TARGET_BINARY_DIR ${TARGET_NAME} BINARY_DIR)
        configure_file("${BAUER_ANDROID_MANIFEST_FILE}" "${TARGET_BINARY_DIR}/AndroidManifest.xml")
    endif()
endmacro()

macro(android_export_jni_functions_from_folder TARGET_NAME SOURCE_FOLDER)
    if(BDN_PLATFORM_ANDROID)

        execute_process(COMMAND "grep" "-rnw" "${SOURCE_FOLDER}" "--include=*.cpp" "-e" "extern \"C\" JNIEXPORT" OUTPUT_VARIABLE JNI_CALLS)
        string(REGEX MATCHALL "Java_[^(]*" JNI_CALLS "${JNI_CALLS}")
        list(TRANSFORM JNI_CALLS PREPEND "-u")

        target_link_options(${TARGET_NAME} PRIVATE ${JNI_CALLS})

    endif()
endmacro()

macro(android_export_jni_functions TARGET_NAME)
    if(BDN_PLATFORM_ANDROID)
        get_target_property(BODEN_CMAKE_SOURCE_DIR boden_cmake SOURCE_DIR)
        set(folder ${BODEN_CMAKE_SOURCE_DIR}/..)
        android_export_jni_functions_from_folder(${TARGET_NAME} ${folder})

        get_target_property(TARGET_SOURCE_DIR ${TARGET_NAME} SOURCE_DIR)
        android_export_jni_functions_from_folder(${TARGET_NAME} ${TARGET_SOURCE_DIR})
    endif()
endmacro()
