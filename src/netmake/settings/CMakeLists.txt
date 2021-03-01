add_library(settings
    settings.hpp settings.cpp)

target_include_directories(settings PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_link_libraries(settings PUBLIC debug20)