# probe various system files that may be found
function (read VAR varname)
    file (GLOB has_os_release /etc/os-release)
    if (NOT has_os_release STREQUAL "")
        read_release (${VAR} FROM /etc/os-release INTO _descr)
        string (STRIP "${_descr}" _descr)
        set (${varname} "${_descr}" PARENT_SCOPE)
    endif()

endfunction ()

function (distro_name varname)
    set(_value)
    read (PRETTY_NAME _value)
    set (${varname} "${_value}" PARENT_SCOPE)
endfunction()

function(distro_id varname)
    set(_value)
    read (ID _value)
    set (${varname} "${_value}" PARENT_SCOPE)
endfunction()

function (read_release valuename FROM filename INTO varname)
    file (STRINGS ${filename} _distrib
        REGEX "^${valuename}="
        )
    string (REGEX REPLACE
        "^${valuename}=\"?\(.*\)" "\\1" ${varname} "${_distrib}"
        )
    # remove trailing quote that got globbed by the wildcard (greedy match)
    string (REGEX REPLACE
        "\"$" "" ${varname} "${${varname}}"
        )
    set (${varname} "${${varname}}" PARENT_SCOPE)
endfunction (read_release valuename FROM filename INTO varname)

if(UNIX)
    distro_name(BDN_LINUX_DISTRIBUTION_NAME)
    distro_id(BDN_LINUX_DISTRIBUTION_ID)
    set(BDN_LINUX_DISTRIBUTION_NAME ${BDN_LINUX_DISTRIBUTION_NAME} CACHE INTERNAL "Linux distribution name")
    set(BDN_LINUX_DISTRIBUTION_ID ${BDN_LINUX_DISTRIBUTION_ID} CACHE INTERNAL "Linux distribution id")
endif()

