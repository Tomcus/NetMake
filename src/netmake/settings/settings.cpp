#include "settings.hpp"

using namespace netmake;

std::filesystem::path settings::source_dir{"./src"};
std::filesystem::path settings::dest_dir{"./generated"};

void settings::init(const std::vector<std::string>& args) {
    auto file_printer = d20::get_file_printer("settings.log");
    file_printer->set_level(d20::logging_level::TRACE);
    auto console_printer = d20::get_console_printer();
    console_printer->set_level(d20::logging_level::WARN);
    logger = d20::get_logger("settings", {file_printer, console_printer});
    logger->log<d20::logging_level::INFO>("Settings logger initialization finished.");
    for (size_t i = 0; i < args.size(); i++) {
        auto& arg = args[i];
        if (arg == "-s" || arg == "--src") {
            settings::source_dir = args[++i];
        } else if (arg == "-d" || arg == "--dest") {
            settings::dest_dir = args[++i];
        }
    }
    logger->log<d20::logging_level::INFO>("Finished argument parsing.");
}

settings settings::master() noexcept {
    logger->log<d20::logging_level::INFO>("Creating copy of global settings");
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