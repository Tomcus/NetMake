add_executable(netmake main.cpp
               settings.hpp settings.cpp
               generate.hpp generate.cpp)

set_property(TARGET netmake PROPERTY CXX_STANDARD 20)

target_link_libraries(netmake
    fmt
    nlohmann_json)