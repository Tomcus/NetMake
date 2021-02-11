#include "path.hpp"
#include "settings/settings.hpp"

using namespace std::filesystem;

path netmake::get_destination_path(const path& file) {
    return settings::dest_dir / file;
}

path netmake::get_source_path(const path& file) {
    return settings::source_dir / file;
}

path netmake::get_template_path(const path& file) {
    return settings::source_dir / path{"templates"} / file;
}