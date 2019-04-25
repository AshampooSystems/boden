macro(roger_add TARGET _VAR_ _RESOURCE_FILE)
    # Hack to force cmake to rerun when the resource description has changed
    configure_file(${_RESOURCE_FILE} ${CMAKE_BINARY_DIR}/resource_auto_refresh.txt)

    if(BDN_PLATFORM_IOS OR BDN_PLATFORM_OSX)
        get_target_property(BODEN_CMAKE_SOURCE_DIR boden_cmake SOURCE_DIR)

        if(BDN_PLATFORM_IOS)
            set(_platform "ios")
        else()
            set(_platform "mac")
        endif()

        set(_dest_dir ${CMAKE_CURRENT_BINARY_DIR}/precompiled-resources)
        set(_timestamp_file ${_dest_dir}/resource-timestamp)

        execute_process(COMMAND ${BODEN_CMAKE_SOURCE_DIR}/../roger/roger.py dependencies -p ${_platform} -v ${_RESOURCE_FILE} ${_dest_dir}
            OUTPUT_VARIABLE _resource_output
            ERROR_VARIABLE _resource_verbose_output
            RESULT_VARIABLE _resource_result
            ENCODING UTF8)

        if(${_resource_result} GREATER 0)
            message(FATAL_ERROR ${_resource_verbose_output})
        endif()

        foreach(_object ${_resource_output})
            list(GET _object 0 _src)
            list(GET _object 1 _dest)
            list(APPEND _input_resource_files ${_src})
            list(APPEND _output_resource_files ${_dest})

            get_filename_component(_bundle_dir_location ${_dest} DIRECTORY )
            string(REPLACE "${_dest_dir}" "Resources" _bundle_dir_location ${_bundle_dir_location})

            set_source_files_properties(${_dest} PROPERTIES MACOSX_PACKAGE_LOCATION ${_bundle_dir_location})
        endforeach()

        add_custom_command(OUTPUT ${_timestamp_file}
            COMMAND ${BODEN_CMAKE_SOURCE_DIR}/../roger/roger.py build -p ${_platform} ${_RESOURCE_FILE} ${_dest_dir}
            MAIN_DEPENDENCY ${_RESOURCE_FILE}
            DEPENDS ${_input_resource_files}
            COMMENT "Copying Resources from ${_RESOURCE_FILE} ..."
            BYPRODUCTS ${_output_resource_files}
            )

        list(APPEND ${_VAR_} ${_RESOURCE_FILE} ${_timestamp_file} ${_output_resource_files} ${_input_resource_files})
    elseif(BDN_PLATFORM_ANDROID)
        get_target_property(BODEN_CMAKE_SOURCE_DIR boden_cmake SOURCE_DIR)

        execute_process(COMMAND ${BODEN_CMAKE_SOURCE_DIR}/../roger/roger.py dependencies -p android -v ${_RESOURCE_FILE} ${CMAKE_CURRENT_BINARY_DIR}/android-resources
            OUTPUT_VARIABLE _resource_output
            ERROR_VARIABLE _resource_verbose_output
            RESULT_VARIABLE _resource_result
            ENCODING UTF8)

        if(${_resource_result} GREATER 0)
            message(FATAL_ERROR ${_resource_verbose_output})
        endif()

        foreach(_object ${_resource_output})
            list(GET _object 0 _src)
            list(GET _object 1 _dest)

            list(APPEND ${_VAR_} ${_src})
        endforeach()

        list(APPEND ${_VAR_} ${_RESOURCE_FILE})

        file(REMOVE_RECURSE ${CMAKE_CURRENT_BINARY_DIR}/android-resources)

        execute_process(COMMAND ${BODEN_CMAKE_SOURCE_DIR}/../roger/roger.py build -p android -v ${_RESOURCE_FILE} ${CMAKE_CURRENT_BINARY_DIR}/android-resources
            OUTPUT_VARIABLE _resource_copy_output
            ERROR_VARIABLE _resource_copy_verbose_output
            RESULT_VARIABLE _resource_copy_result
            ENCODING UTF8)

        if(${_resource_copy_result} GREATER 0)
            message(FATAL_ERROR ${_resource_copy_verbose_output})
        endif()

    endif()
endmacro()
