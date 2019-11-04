
include_directories (
    "${CMAKE_CURRENT_LIST_DIR}"
    )

target_sources(jasterix
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/files.h"
        "${CMAKE_CURRENT_LIST_DIR}/logger.h"
        "${CMAKE_CURRENT_LIST_DIR}/json.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/string_conv.h"
        "${CMAKE_CURRENT_LIST_DIR}/hashchecker.h"
    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/files.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/string_conv.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/hashchecker.cpp"
)

