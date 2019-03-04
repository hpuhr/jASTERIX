
include_directories (
    "${CMAKE_CURRENT_LIST_DIR}"
    )

target_sources(jasterix
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/jasterix.h"
    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/jasterix.cpp"
)

include("${CMAKE_CURRENT_LIST_DIR}/asterix/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/frames/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/items/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/utils/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/client/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/test/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/write/CMakeLists.txt")


