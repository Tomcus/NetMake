#include "simple_page_generator.hpp"
#include "settings/settings.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <fmt/core.h>

namespace netmake {
    simple_page_generator::simple_page_generator(const std::string& site_name, const json& site_data):name{site_name}, data{site_data} { }

    void simple_page_generator::generate() const {
        auto file = fmt::output_file(get_destination_path(fmt::format("{}.html", name)).c_str());
        file.print("<head>\n");
        generate_header(file);
        file.print("</head>\n<body>\n");
        generate_body(file);
        generate_footer(file);
    }

    void simple_page_generator::offset(const std::filesystem::path& new_offset) {
        extra_path = new_offset;
    }

    std::string simple_page_generator::generate_extra_styles() const {
        std::string res = "";
        if (data.contains("extra_css")) {
            auto extra_styles = data["extra_css"];
            for (auto style: extra_styles) {
                res += fmt::format("<link rel=\"stylesheet\" href=\"{}\" />", style);
            }
        }
        return res;
    }
    std::string simple_page_generator::generate_keyword_meta_tag(const json& meta_data) const {
        std::string res = "";
        std::vector<std::string> keywords;
        if (meta_data.contains("keywords")) {
            auto key_data = meta_data["keywords"];

            keywords.reserve(key_data.size());

            for (size_t i = 0; i < key_data.size(); ++i) {
                keywords.emplace_back(key_data[i]);
            }
        }
        if (data.contains("extra_keywords")) {
            auto extra_keywords = data["extra_keywords"];

            keywords.reserve(keywords.size() + extra_keywords.size());

            for (size_t i = 0; i < extra_keywords.size(); ++i) {
                keywords.emplace_back(extra_keywords[i]);
            }
        }
        if (keywords.size() > 0)
            res += fmt::format("<meta name=\"keywords\" content=\"{}\" />\n", fmt::join(keywords, ", "));
        return res;
    }
    std::string simple_page_generator::get_meta_tag(const std::string& meta_tag_name, const json& meta_data) const {
        std::string res = "";
        if (meta_data.contains(meta_tag_name)) {
            res += fmt::format("<meta name=\"{}\" content=\"{}\" />\n", meta_tag_name, meta_data[meta_tag_name]);
        }
        return res;
    }
    std::string simple_page_generator::generate_pages_list(const std::string& template_name) const {
        std::string listings = "";
        std::string listing_template = load_file(get_template_path(fmt::format("{}.html", template_name)));
        for (auto& site: sites["pages"].items()) {
            if (site.key() != "index") {
                std::string url = site.key();
                if (site.key().ends_with("*")) {
                    url = url.substr(0, url.size() - 2);
                }
                std::string site_title = site.value()["title"];
                std::string description = "";
                if (site.value().contains("description")) {
                    description = site.value()["description"];
                }
                listings += fmt::format(listing_template,
                    fmt::arg(fmt::format("{}_url", template_name).c_str(), (extra_path / std::filesystem::path{url}).c_str()),
                    fmt::arg(fmt::format("{}_title", template_name).c_str(), site_title),
                    fmt::arg(fmt::format("{}_description", template_name).c_str(), description));
            }
        }
        return listings;
    }

    json simple_page_generator::sites{};

    void simple_page_generator::generate_header(const fmt::ostream& file) const {
        static json meta_data = {};
        static std::string header_template = load_file(get_template_path("header.html"));
        if (meta_data == json{}) {
            meta_data = load_json(get_source_path("metadata.json"));
        }

        std::string complete_title = fmt::format("{} - {}", data["title"], meta_data["main_title"]);
        if (extra_title != "") {
            complete_title = fmt::format("{} - {}", extra_title, complete_title);
        }

        std::string meta_tags = "";
        meta_tags += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" />\n";

        meta_tags += "<meta name=\"generator\" content=\"NetMake - TODO: Add url\" />\n";
        meta_tags += get_meta_tag("description", meta_data);
        meta_tags += get_meta_tag("author", meta_data);
        meta_tags += get_meta_tag("application-name", meta_data);
        meta_tags += generate_keyword_meta_tag(meta_data);

        file.print(header_template, fmt::arg("title", complete_title),
                                    fmt::arg("meta_tags", meta_tags),
                                    fmt::arg("extra_styles", generate_extra_styles()));
    }
    std::string simple_page_generator::generate_nav() const {
        static std::string nav_template = load_file(get_template_path("nav.html"));
        fmt::dynamic_format_arg_store<fmt::format_context> store{};
        for (auto listing:data["listings_templates"]) {
            store.push_back(fmt::arg(fmt::format("generated_{}s", listing).c_str(), generate_pages_list(listing)));
        }
        return fmt::vformat(nav_template, store);
    }
    void simple_page_generator::generate_body(const fmt::ostream& file) const {
        // const json site_data = sites["pages"][site_name];
        std::string content = load_file(get_template_path(fmt::format("{}.html", name)));
        file.print("{}\n{}", generate_nav(), content);
    }
    void simple_page_generator::generate_footer(const fmt::ostream& file) const {
        static std::string footer = load_file(get_template_path("format.html"));
        file.print(footer);
    }
    
    json simple_page_generator::load_json(const std::filesystem::path& path_to_json) {
        json res;
        std::ifstream ifs(path_to_json);
        if (!ifs.good()) {
            throw std::runtime_error(fmt::format("Can't open/find file {}", path_to_json.c_str()));
        }
        ifs >> res;
        return res;
    }
    std::string simple_page_generator::load_file(const std::filesystem::path& path_to_file) {
        std::ifstream ifs(path_to_file);
        if (!ifs.good()) {
            throw std::runtime_error(fmt::format("File can't be open/doesn't exist. Path: {}", path_to_file.c_str()));
        }
        std::ostringstream oss;
        oss << ifs.rdbuf();
        if (!ifs.good()) {
            throw std::runtime_error(fmt::format("Error while reading file: {}", path_to_file.c_str()));
        }
        return oss.str();
    }

    std::filesystem::path simple_page_generator::get_destination_path(const std::filesystem::path& file) const {
        return settings::dest_dir / extra_path / file;
    }
    std::filesystem::path simple_page_generator::get_source_path(const std::filesystem::path& file) const {
        return settings::source_dir / file;
    }
    std::filesystem::path simple_page_generator::get_template_path(const std::filesystem::path& file) const {
        return settings::source_dir / std::filesystem::path{"templates"} / file;
    }
}