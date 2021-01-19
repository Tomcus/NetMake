#include "settings.hpp"

using namespace netmake;

std::string settings::source_dir{""};
std::string settings::dest_dir{""};

void settings::init(const std::vector<std::string>& args) {
    settings::source_dir = "./src";
    settings::dest_dir = "./generated";
    for (size_t i = 0; i < args.size(); i++) {
        auto& arg = args[i];
        if (arg == "-s" || arg == "--src") {
            settings::source_dir = args[++i];
        } else if (arg == "-d" || arg == "--dest") {
            settings::dest_dir = args[++i];
        }
    }
}