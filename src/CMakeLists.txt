
include_directories (
    "${CMAKE_CURRENT_LIST_DIR}"
    )

target_sources(jasterix
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/jasterix.h"
    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/jasterix.cpp"
)

include("${CMAKE_CURRENT_LIST_DIR}/parse/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/utils/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/client/CMakeLists.txt")


