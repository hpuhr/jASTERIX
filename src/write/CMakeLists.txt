
include_directories (
    "${CMAKE_CURRENT_LIST_DIR}"
    )

target_sources(jasterix
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/jsonfilewritetask.h"
        "${CMAKE_CURRENT_LIST_DIR}/jsonwriter.h"
    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/jsonwriter.cpp"
)

