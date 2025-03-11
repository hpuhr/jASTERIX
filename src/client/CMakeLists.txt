
include_directories (
    "${CMAKE_CURRENT_LIST_DIR}"
    )

add_executable ( jasterix_client "${CMAKE_CURRENT_LIST_DIR}/main.cpp")

message("Client Boost_LIBRARIES: ${Boost_LIBRARIES}")

target_link_libraries ( jasterix_client ${Boost_LIBRARIES} jasterix)


