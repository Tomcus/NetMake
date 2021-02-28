#include "settings.hpp"

using namespace netmake;

std::filesystem::path settings::source_dir{"./src"};
std::filesystem::path settings::dest_dir{"./generated"};

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

settings settings::master() noexcept {
    return settings(settings::source_dir, settings::dest_dir);
}

std::filesystem::path settings::path_to_source_file(const std::string_view file_name) const noexcept {
    return source / std::filesystem::path{file_name.data()};
}

std::filesystem::path path_to_template_file(const std::string_view file_name) const noexcept {
    return source / std::filesystem::path{"templates"} / std::filesystem::path{file_name.data()};
}

std::filesystem::path path_to_destination_file(const std::string_view file_name) const noexcept {
    return destination / std::filesystem::path{file_name.data()};
}

void settings::offset(const std::string_view subfolder) noexcept {
    destination /= std::filesystem::path(subfolder.c_str());
}