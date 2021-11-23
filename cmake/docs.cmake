find_package(Doxygen QUIET COMPONENTS dot)

if(Doxygen_FOUND)
    doxygen_add_docs(docs ${CMAKE_SOURCE_DIR}/src/)
endif()