#ifndef NETMAKE_PATH_HPP
#define NETMAKE_PATH_HPP

#include <filesystem>

namespace netmake {
    std::filesystem::path get_destination_path(const std::filesystem::path& file);
    std::filesystem::path get_source_path(const std::filesystem::path& file);
    std::filesystem::path get_template_path(const std::filesystem::path& file);
}

#endif//NETMAKE_PATH_HPP