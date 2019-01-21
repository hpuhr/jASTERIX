set(ATSDB_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
message("  Path: ${ATSDB_PATH}")

cmake_minimum_required(VERSION 3.1)
set ( CMAKE_BUILD_TYPE Debug )
set(CMAKE_CXX_FLAGS_MYREL "-O3")
#set ( CMAKE_BUILD_TYPE Release )

project( jASTERIX )
SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "0")
SET(CPACK_PACKAGE_VERSION_PATCH "1")

message("  System: ${CMAKE_SYSTEM}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

message("  Platform: Linux")
add_definitions ( -Wall -std=c++11 -Wno-int-in-bool-context)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

find_package ( Boost REQUIRED COMPONENTS regex system program_options filesystem iostreams)
message("  Boost_INCLUDE_DIR: ${Boost_INCLUDE_DIR}")
message("  Boost_LIBRARY_DIR: ${Boost_LIBRARY_DIR}")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH};${ATSDB_PATH}/cmake_modules)
message("  CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}") 

find_package ( LOG4CPP REQUIRED )
message("  LOG4CPP_INCLUDE_DIR: ${LOG4CPP_INCLUDE_DIR}")
message("  LOG4CPP_LIBRARY: ${LOG4CPP_LIBRARIES}") 

find_package(TBB REQUIRED)

add_library(jasterix "")

message("Install path: ${CMAKE_INSTALL_PREFIX}")

include(src/CMakeLists.txt)

configure_file(src/utils/global.h.in include/global.h)

include_directories (
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${PROJECT_BINARY_DIR}/include
    ${Boost_INCLUDE_DIRS}
    )

link_directories(
    ${Boost_LIBRARY_DIR}
    )

target_link_libraries ( jasterix
    ${Boost_LIBRARIES}
    ${LOG4CPP_LIBRARIES}
    ${MySQLpp_LIBRARY}
    ${TBB_LIBRARIES})

message("Installing using prefix: ${CMAKE_INSTALL_PREFIX}")
install(DIRECTORY "specifications" DESTINATION jasterix)

install (TARGETS jasterix DESTINATION lib
    PUBLIC_HEADER DESTINATION include/jasterix)

install (TARGETS jasterix_client DESTINATION bin)

# build a CPack driven installer package
include (InstallRequiredSystemLibraries)
set (CPACK_RESOURCE_FILE_LICENSE
     "${CMAKE_CURRENT_SOURCE_DIR}/gpl.txt")
include (CPack)


# add a target to generate API documentation with Doxygen
#find_package(Doxygen)
#if(DOXYGEN_FOUND)
#    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in @ONLY)
#    add_custom_target(doc
#        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile
#        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc
#        COMMENT "Generating API documentation with Doxygen" VERBATIM
#        )
#endif(DOXYGEN_FOUND)

#file(GLOB DOT ".*")
#file(GLOB TILD "*~")
#file(GLOB TEMP "*.cmake")





