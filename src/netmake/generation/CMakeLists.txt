add_library(generation
    generate.hpp generate.cpp
    json.hpp
    path.hpp path.cpp)

target_include_directories(generation PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_link_libraries(generation
    settings
    nlohmann_json
    fmt)