
include_directories (
    "${CMAKE_CURRENT_LIST_DIR}"
    )




add_executable ( jasterix_tests "${CMAKE_CURRENT_LIST_DIR}/main_tests.cpp")

target_sources(jasterix_tests
    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/cat001_1.1.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/cat002_1.0.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/cat019_1.3.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/cat020_1.5.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/cat021_2.1.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/cat021_2.1.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/cat034_1.26.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/cat048_1.15.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/cat062_1.12.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/cat062_1.16.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/cat063_1.0.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/cat065_1.3.cpp"
)

target_link_libraries ( jasterix_tests jasterix)

target_link_libraries ( jasterix_tests
    ${Boost_LIBRARIES})
