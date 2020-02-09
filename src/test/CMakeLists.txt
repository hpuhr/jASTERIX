
include_directories (
    "${CMAKE_CURRENT_LIST_DIR}"
    )


add_executable ( test_jasterix "${CMAKE_CURRENT_LIST_DIR}/test_jasterix.cpp" )

target_sources(test_jasterix
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/catch.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/test_jasterix.h"
#    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/test_cat001_1.1.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test_cat002_1.0.cpp"
#        "${CMAKE_CURRENT_LIST_DIR}/cat019_1.3.cpp"
#        "${CMAKE_CURRENT_LIST_DIR}/cat020_1.5.cpp"
#        "${CMAKE_CURRENT_LIST_DIR}/cat021_2.1.cpp"
#        "${CMAKE_CURRENT_LIST_DIR}/cat021_2.1.cpp"
#        "${CMAKE_CURRENT_LIST_DIR}/cat034_1.26.cpp"
#        "${CMAKE_CURRENT_LIST_DIR}/cat048_1.15.cpp"
#        "${CMAKE_CURRENT_LIST_DIR}/cat062_1.12.cpp"
#        "${CMAKE_CURRENT_LIST_DIR}/cat062_1.16.cpp"
#        "${CMAKE_CURRENT_LIST_DIR}/cat063_1.0.cpp"
#        "${CMAKE_CURRENT_LIST_DIR}/cat065_1.3.cpp"
)

target_link_libraries ( test_jasterix jasterix )

enable_testing()
set ( DEFINITION_PATH "${CMAKE_CURRENT_LIST_DIR}/../../definitions/" )
set ( DATA_PATH "${CMAKE_CURRENT_LIST_DIR}/" )

add_test(NAME TestjASTERIX COMMAND test_jasterix --definition_path ${DEFINITION_PATH} --data_path ${DATA_PATH})
