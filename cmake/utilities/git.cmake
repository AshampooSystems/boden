macro(get_git_short_revision GIT_COMMIT_HASH)
    execute_process(
      COMMAND git log -1 --format=%h
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      OUTPUT_VARIABLE ${GIT_COMMIT_HASH}
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endmacro()
