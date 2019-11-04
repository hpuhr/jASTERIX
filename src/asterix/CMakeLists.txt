
include_directories (
    "${CMAKE_CURRENT_LIST_DIR}"
    )

target_sources(jasterix
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/asterixparser.h"
        "${CMAKE_CURRENT_LIST_DIR}/datablockfindertask.h"
    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/asterixparser.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/category.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/edition.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/mapping.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/record.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/refedition.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/ref.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/spfedition.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/spf.cpp"
)

