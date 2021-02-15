add_subdirectory(json)
add_subdirectory(fmt)


if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 10.2)
        add_subdirectory(debug20)
    endif()
endif()
