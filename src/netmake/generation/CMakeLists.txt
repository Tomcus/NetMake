add_library(generation
    generate.hpp generate.cpp
    json.hpp
    generative_element.hpp generative_element.cpp
    template_generator.hpp template_generator.cpp)

target_include_directories(generation PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_link_libraries(generation
    settings
    nlohmann_json
    fmt)