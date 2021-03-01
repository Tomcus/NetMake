#ifndef NETMAKE_SETTINGS_HPP
#define NETMAKE_SETTINGS_HPP

#include <string_view>
#include <vector>
#include <filesystem>

#include "debug20/log.hpp"

namespace netmake {
    struct settings {
        /// Processes user arguments and reinitiliazes the settings values
        static void init(const std::vector<std::string>& args) noexcept;
        /// Get local copy of master settings
        static settings master() noexcept;
        /// Source file folder location
        static std::filesystem::path source_dir;
        /// Destination folder, where generated data are stored.
        static std::filesystem::path dest_dir;
        static d20::logger logger;

        /// Get path to source file defined by file_name
        [[nodiscard]] std::filesystem::path path_to_source_file(const std::string_view file_name) const noexcept;
        /// Get path to template file in source directory
        [[nodiscard]] std::filesystem::path path_to_template_file(const std::string_view file_name) const noexcept;
        /// Get path for output file
        [[nodiscard]] std::filesystem::path path_to_destination_file(const std::string_view file_name) const noexcept;
        /// Offset local copy of settings into defined subfolder.
        void offset(const std::string_view subfolder) noexcept;
    protected:
        settings(const std::filesystem::path& source_dir, const std::filesystem::path& destination_dir);
        std::filesystem::path source;
        std::filesystem::path destination;
    };
}

#endif//NETMAKE_SETTINGS_HPP