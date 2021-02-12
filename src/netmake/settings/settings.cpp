#include "settings.hpp"

using namespace netmake;

std::filesystem::path settings::source_dir{"./src"};
std::filesystem::path settings::dest_dir{"./generated"};
bool settings::concat_nav{true};

void settings::init(const std::vector<std::string>& args) {
    for (size_t i = 0; i < args.size(); i++) {
        auto& arg = args[i];
        if (arg == "-s" || arg == "--src") {
            settings::source_dir = args[++i];
        } else if (arg == "-d" || arg == "--dest") {
            settings::dest_dir = args[++i];
        } else if (arg == "-c" || arg == "--concat") {
            settings::concat_nav = true;
        } else if (arg == "-C" || arg == "--unique_navigations") {
            settings::concat_nav = false;
        }

    }
}