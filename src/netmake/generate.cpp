#include "generate.hpp"
#include "settings.hpp"

#include <fmt/os.h>
#include <fstream>
#include <sstream>

using namespace netmake;

std::string load_file(const std::string& path) {
    std::ostringstream oss{};
    std::ifstream ifs(path);
    oss << ifs.rdbuf();
    return oss.str();
}

std::string generate_header(const json& site_data) {
    static json meta_data;
    static std::string header_template = "";
    if (meta_data.is_null()) {
        std::ifstream md_file(settings::source_dir + "/meta_data.json");
        md_file >> meta_data;
    }
    if (header_template == "") {
        header_template = load_file(settings::source_dir + "/templates/header.html");
    }
    std::string complete_title = fmt::format("{} - {}", site_data["title"], meta_data["main_title"]);
    std::string meta_tags = "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" />\n";
    auto description = meta_data["description"];
    if (!description.is_null()) {
        meta_tags += fmt::format("<meta name=\"description\" content\"{}\" /> \n", description);
    }
    auto author = meta_data["author"];
    if (!author.is_null()) {
        meta_tags += fmt::format("<meta name=\"author\" content\"{}\" /> \n", author);
    }
    return fmt::format(header_template, fmt::arg("title", complete_title),
                                        fmt::arg("meta_tags", meta_tags));
}

void netmake::generate_simple_site(const std::string& site_name, const json& site_data) {
    auto file = fmt::output_file(settings::dest_dir + "/" + site_name + ".html");
    file.print("<head>\n{}\n</head>\n<body>\n\n</body>\n", generate_header(site_data));
}

void netmake::generate_complex_site(const std::string& site_name, const json& site_data) {
}