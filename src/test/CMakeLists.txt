
include_directories (
    "${CMAKE_CURRENT_LIST_DIR}"
    )


add_executable ( test_categories "${CMAKE_CURRENT_LIST_DIR}/test_categories.cpp" )
add_executable ( test_ram "${CMAKE_CURRENT_LIST_DIR}/test_ram.cpp" )

target_sources( test_categories
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/catch.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/test_jasterix.h"
    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/test_cat001_1.1.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test_cat002_1.0.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test_cat019_1.3.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test_cat020_1.5.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test_cat021_2.1.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test_cat034_1.26.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test_cat048_1.15.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test_cat062_1.12.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test_cat062_1.16.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test_cat063_1.0.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test_cat065_1.3.cpp"
)

target_sources( test_ram
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/catch.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/test_jasterix.h"
    PRIVATE
        #"${CMAKE_CURRENT_LIST_DIR}/test_cat001_1.1.cpp"
        #"${CMAKE_CURRENT_LIST_DIR}/test_cat002_1.0.cpp"
        #"${CMAKE_CURRENT_LIST_DIR}/test_cat019_1.3.cpp"
        #"${CMAKE_CURRENT_LIST_DIR}/test_cat020_1.5.cpp"
        #"${CMAKE_CURRENT_LIST_DIR}/test_cat021_2.1.cpp"
        #"${CMAKE_CURRENT_LIST_DIR}/test_cat034_1.26.cpp"
        #"${CMAKE_CURRENT_LIST_DIR}/test_cat048_1.15.cpp"
        #"${CMAKE_CURRENT_LIST_DIR}/test_cat062_1.12.cpp"
        #"${CMAKE_CURRENT_LIST_DIR}/test_cat062_1.16.cpp"
        #"${CMAKE_CURRENT_LIST_DIR}/test_cat063_1.0.cpp"
        #"${CMAKE_CURRENT_LIST_DIR}/test_cat065_1.3.cpp"
)

target_link_libraries ( test_categories jasterix )
target_link_libraries ( test_ram jasterix )

enable_testing()
set ( DEFINITION_PATH "${CMAKE_CURRENT_LIST_DIR}/../../definitions/" )
set ( DATA_PATH "${CMAKE_CURRENT_LIST_DIR}/" )

set ( RAM_TEST_FILENAME "/home/sk/data/test/20190506.ff")

add_test(NAME TestCategories COMMAND test_categories --definition_path ${DEFINITION_PATH} --data_path ${DATA_PATH})
add_test(NAME TestRAM COMMAND test_ram --definition_path ${DEFINITION_PATH} --filename ${RAM_TEST_FILENAME})
