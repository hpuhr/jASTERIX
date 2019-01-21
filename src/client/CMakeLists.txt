
include_directories (
    "${CMAKE_CURRENT_LIST_DIR}"
    )

add_executable ( jasterix_client "${CMAKE_CURRENT_LIST_DIR}/main.cpp")
target_link_libraries ( jasterix_client jasterix)

target_link_libraries ( jasterix_client
    ${Boost_LIBRARIES})
