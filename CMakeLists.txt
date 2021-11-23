cmake_minimum_required(VERSION 3.1)

project(NetMake 
    VERSION 0.1
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

include(cmake/docs.cmake)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(external_libs)
add_subdirectory(src)
