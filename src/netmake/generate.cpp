#include "generate.hpp"
#include "settings.hpp"

#include <fmt/os.h>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <filesystem>

using namespace netmake;

std::string load_file(const std::string& path) {
    std::ostringstream oss{};
    std::ifstream ifs(path);
    if (!ifs.good()) {
        throw std::runtime_error(fmt::format("Can't open/find file {}", path));
    }
    oss << ifs.rdbuf();
    return oss.str();
}

json load_json(const std::string& path) {
    json res;
    std::ifstream ifs(path);
    if (!ifs.good()) {
        throw std::runtime_error(fmt::format("Can't open/find file {}", path));
    }
    ifs >> res;
    return res;
}

std::string get_meta_tag(const std::string& meta_tag_name, const json& meta_data) {
    if (meta_data.contains(meta_tag_name)) {
        return fmt::format("<meta name=\"{}\" content=\"{}\" />\n", meta_tag_name, meta_data[meta_tag_name]);
    }
    return "";
}

std::string generate_keyword_meta_tag(const json& meta_data, const json& site_data) {
    std::vector<std::string> keywords;
    if (meta_data.contains("keywords")) {
        auto key_data = meta_data["keywords"];

        keywords.reserve(key_data.size());
    
        for (size_t i = 0; i < key_data.size(); ++i) {
            keywords.emplace_back(key_data[i]);
        }
    }
    if (site_data.contains("extra_keywords")) {
        auto extra_keywords = site_data["extra_keywords"];
        
        keywords.reserve(keywords.size() + extra_keywords.size());
        
        for (size_t i = 0; i < extra_keywords.size(); ++i) {
            keywords.emplace_back(extra_keywords[i]);
        }
    }
    if (keywords.size() > 0)
        return fmt::format("<meta name=\"keywords\" content=\"{}\" />\n", fmt::join(keywords, ", "));
    else
        return "";
}

std::string generate_extra_styles(const json& site_data) {
    std::string res = "";
    if (site_data.contains("extra_css")) {
        auto extra_styles = site_data["extra_css"];
        for (auto style: extra_styles) {
            res += fmt::format("<link rel=\"stylesheet\" href=\"{}\" />", style);
        }
    }
    return res;
}

std::string generate_header(const json& site_data, const std::string& extra_title = "") {
    static json meta_data = {};
    static std::string header_template = load_file(settings::source_dir + "/templates/header.html");
    if (meta_data == json{}) {
        std::ifstream md_file(settings::source_dir + "/metadata.json");
        md_file >> meta_data;
    }

    std::string complete_title = fmt::format("{} - {}", site_data["title"], meta_data["main_title"]);
    if (extra_title != "") {
        complete_title = fmt::format("{} - {}", extra_title, complete_title);
    }

    std::string meta_tags = "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" />\n";

    meta_tags += "<meta name=\"generator\" content=\"NetMake - TODO: Add url\" />\n";
    meta_tags += get_meta_tag("description", meta_data);
    meta_tags += get_meta_tag("author", meta_data);
    meta_tags += get_meta_tag("application-name", meta_data);
    meta_tags += generate_keyword_meta_tag(meta_data, site_data);
    
    return fmt::format(header_template, fmt::arg("title", complete_title),
                                        fmt::arg("meta_tags", meta_tags),
                                        fmt::arg("extra_styles", generate_extra_styles(site_data)));
}

std::string generate_nav(const std::string& site_name, const json& sites) {
    static std::string nav_template = load_file(settings::source_dir + "/templates/nav.html");
    std::string nav_items = "";
    for (auto& site: sites.items()) {
        if (site.key() != "index") {
            std::string edited_link = site.key();
            if (site.key().ends_with("*")) {
                edited_link = edited_link.substr(0, edited_link.size() - 2);
            }
            std::string site_name = site.value()["title"];
            if (site.key() == site_name) {
                nav_items += fmt::format("<span class=\"nav-item nav-current\"><a href=\"{}\" >{}</a>/<span>",
                                         edited_link,
                                         site_name);
            } else {
                nav_items += fmt::format("<span class=\"nav-item\"><a href=\"{}\" >{}</a>/<span>",
                                         edited_link,
                                         site_name);
            }
        }
    }
    return fmt::format(nav_template, fmt::arg("nav_items", nav_items));
}

std::string generate_body(const std::string& site_name, const json& sites) {
    const json site_data = sites[site_name];
    std::string site_template = load_file(fmt::format("{}/templates/{}.html", settings::source_dir, site_name));

    return fmt::format("{}\n{}", generate_nav(site_name, sites),
                       site_template);
}

void netmake::generate_simple_site(const std::string& site_name, const json& sites) {
    auto file = fmt::output_file(settings::dest_dir + "/" + site_name + ".html");
    file.print("<head>\n{}\n</head>\n<body>\n{}\n</body>\n",
               generate_header(sites[site_name]),
               generate_body(site_name, sites));
}

void netmake::generate_complex_site(const std::string& site_name, const json& sites) {
    std::string file_name = site_name.substr(0, site_name.size() - 2);
    auto main_site = fmt::output_file(settings::dest_dir + "/" + file_name + ".html");

    std::string main_site_template = load_file(fmt::format("{}/templates/{}.html", settings::source_dir, file_name));
    std::string nav_bar = generate_nav(site_name, sites);

    auto list = sites[site_name]["items"];
    if (list.is_string()) {
        list = load_json(fmt::format("{}/{}.json", settings::source_dir, list));
    }

    std::filesystem::create_directories(fmt::format("{}/{}", settings::dest_dir, file_name));
    std::string generated_list = "";
    if (list.size() > 0) {
        std::string item_template_name = sites[site_name]["list_item_template"];
        std::string item_template = load_file(fmt::format("{}/templates/{}.html", settings::source_dir, item_template_name));
        for (auto item: list) {
            std::string url = fmt::format("{}/{}.html", file_name, item["name"]);
            generated_list += fmt::format(item_template, fmt::arg("item_title", item["title"]),
                                                         fmt::arg("item_description", item["description"]),
                                                         fmt::arg("item_url", url));
            
            auto site = fmt::output_file(fmt::format("{}/{}", settings::dest_dir, url));
            std::string site_template = load_file(fmt::format("{}/templates/{}.html", settings::source_dir, item["name"]));
            site.print("<head>\n{}\n</head>\n<body>\n{}\n{}\n</body>\n",
               generate_header(sites[site_name], item["title"]),
               nav_bar,
               fmt::format(site_template, fmt::arg("generated_list", generated_list)));
        }
    }

    main_site.print("<head>\n{}\n</head>\n<body>\n{}\n{}\n</body>\n",
               generate_header(sites[site_name]),
               nav_bar,
               fmt::format(main_site_template, fmt::arg("generated_list", generated_list)));
}