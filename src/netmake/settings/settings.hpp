#ifndef NETMAKE_SETTINGS_HPP
#define NETMAKE_SETTINGS_HPP

#include <string>
#include <vector>
#include <filesystem>

namespace netmake {
    class settings {
    public:
        static void init(const std::vector<std::string>& args);
        static std::filesystem::path source_dir;
        static std::filesystem::path dest_dir;
    };
}

#endif//NETMAKE_SETTINGS_HPP