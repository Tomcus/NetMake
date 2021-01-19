#ifndef NETMAKE_SETTINGS_HPP
#define NETMAKE_SETTINGS_HPP

#include <string>
#include <vector>

namespace netmake {
    class settings {
    public:
        static void init(const std::vector<std::string>& args);
        static std::string source_dir;
        static std::string dest_dir;
    };
}

#endif//NETMAKE_SETTINGS_HPP