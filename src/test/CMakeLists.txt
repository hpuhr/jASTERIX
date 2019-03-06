
include_directories (
    "${CMAKE_CURRENT_LIST_DIR}"
    )




add_executable ( jasterix_tests "${CMAKE_CURRENT_LIST_DIR}/main_tests.cpp")

target_sources(jasterix_tests
    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/cat048_1.15.cpp"
)

target_link_libraries ( jasterix_tests jasterix)

target_link_libraries ( jasterix_tests
    ${Boost_LIBRARIES})
