add_subdirectory(settings)
add_subdirectory(generation)

add_executable(netmake main.cpp)

target_link_libraries(netmake
    generation
    settings
    debug20)
