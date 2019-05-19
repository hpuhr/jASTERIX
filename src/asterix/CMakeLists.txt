
include_directories (
    "${CMAKE_CURRENT_LIST_DIR}"
    )

target_sources(jasterix
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/asterixparser.h"
        "${CMAKE_CURRENT_LIST_DIR}/datablockfindertask.h"
#        "${CMAKE_CURRENT_LIST_DIR}/category.h"
#        "${CMAKE_CURRENT_LIST_DIR}/edition.h"
#        "${CMAKE_CURRENT_LIST_DIR}/mapping.h"
#        "${CMAKE_CURRENT_LIST_DIR}/record.h"
    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/asterixparser.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/category.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/edition.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/mapping.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/record.cpp"
)

