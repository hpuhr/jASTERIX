
include_directories (
    "${CMAKE_CURRENT_LIST_DIR}"
    )

target_sources(jasterix
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/jasterix.h"
        "${CMAKE_CURRENT_LIST_DIR}/edition.h"
        "${CMAKE_CURRENT_LIST_DIR}/frameparser.h"
        "${CMAKE_CURRENT_LIST_DIR}/category.h"
        "${CMAKE_CURRENT_LIST_DIR}/mapping.h"
        "${CMAKE_CURRENT_LIST_DIR}/record.h"
        "${CMAKE_CURRENT_LIST_DIR}/refedition.h"
        "${CMAKE_CURRENT_LIST_DIR}/ref.h"
        "${CMAKE_CURRENT_LIST_DIR}/spfedition.h"
        "${CMAKE_CURRENT_LIST_DIR}/spf.h"
        "${CMAKE_CURRENT_LIST_DIR}/itemparserbase.h"
)


