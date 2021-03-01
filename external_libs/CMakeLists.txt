set(JSON_BuildTests OFF CACHE BOOL "Build the unit tests when BUILD_TESTING is enabled." FORCE)
add_subdirectory(json)

add_subdirectory(fmt)

add_subdirectory(debug20)
