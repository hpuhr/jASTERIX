
include_directories (
    "${CMAKE_CURRENT_LIST_DIR}"
    )

target_sources(jasterix
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/itemparserbase.h"
        "${CMAKE_CURRENT_LIST_DIR}/itemparser.h"
        "${CMAKE_CURRENT_LIST_DIR}/fixedbitsitemparser.h"
        "${CMAKE_CURRENT_LIST_DIR}/fixedbytesitemparser.h"
        "${CMAKE_CURRENT_LIST_DIR}/skipbytesitemparser.h"
        "${CMAKE_CURRENT_LIST_DIR}/compounditemparser.h"
        "${CMAKE_CURRENT_LIST_DIR}/dynamicbytesitemparser.h"
        "${CMAKE_CURRENT_LIST_DIR}/extendablebitsitemparser.h"
        "${CMAKE_CURRENT_LIST_DIR}/extendableitemparser.h"
        "${CMAKE_CURRENT_LIST_DIR}/fixedbitfielditemparser.h"
        "${CMAKE_CURRENT_LIST_DIR}/optionalitemparser.h"
        "${CMAKE_CURRENT_LIST_DIR}/repetetiveitemparser.h"
    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/itemparserbase.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/itemparser.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/fixedbitsitemparser.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/fixedbytesitemparser.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/skipbytesitemparser.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/compounditemparser.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/extendablebitsitemparser.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/extendableitemparser.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/dynamicbytesitemparser.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/fixedbitfielditemparser.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/optionalitemparser.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/repetetiveitemparser.cpp"
)

