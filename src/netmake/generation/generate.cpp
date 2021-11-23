#include "generate.hpp"
#include "path.hpp"
#include "settings/settings.hpp"

#include <fmt/os.h>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <filesystem>
#include <string>

using namespace netmake;
using namespace std::filesystem;

/**
 * @brief Load file from specific path.
 * 
 * @param path Path to file that will be loaded
 * @return std::string File contents
 */
std::string load_file(const path& path) {
    std::ifstream ifs(path);
    if (!ifs.good()) {
        throw std::runtime_error(fmt::format("File can't be open/doesn't exist. Path: {}", path.c_str()));
    }
    std::ostringstream oss;
    oss << ifs.rdbuf();
    if (!ifs.good()) {
        throw std::runtime_error(fmt::format("Error while reading file: {}", path.c_str()));
    }
    return oss.str();
}

/**
 * @brief Load file contents and parses it into json
 * 
 * @param path Path to the json file
 * @return json Json data that were stored in the file
 */
json load_json(const path& path) {
    json res;
    std::ifstream ifs(path);
    if (!ifs.good()) {
        throw std::runtime_error(fmt::format("Can't open/find file {}", path.c_str()));
    }
    ifs >> res;
    return res;
}


/**
 * @brief Main generation function.
 * Generates whole website based on configuration.
 * The function checks if source and destination directories are present.
 * If the source directory doesn't exists, then the function will throw std::runtime_error.
 * If the destination directory doesn't exists, then it is created.
 * The the function loads `sites.json` file from source directory, that should contain the configuration needed for website generation.
 * Either generate_complex_site ot generate_simple_site is used depending on the configuration.
 */
void netmake::generate() {
    if (!is_directory(settings::source_dir)) {
        throw std::runtime_error(fmt::format("Can't generate website. Source directory {} doesn't exists", settings::source_dir.c_str()));
    }

    if (!is_directory(settings::dest_dir)) {
        create_directories(settings::dest_dir);
    }

    json sites = load_json(get_source_path("sites.json"));
    for (auto& item: sites.items()) {
        if (item.key().ends_with('*')) {
            generate_complex_site(item.key(), sites);
        } else {
            generate_simple_site(item.key(), sites);
        }
    }
}

std::string get_meta_tag(const std::string& meta_tag_name, const json& meta_data) {
    std::string res = "";
    if (meta_data.contains(meta_tag_name)) {
        res += fmt::format("<meta name=\"{}\" content=\"{}\" />\n", meta_tag_name, meta_data[meta_tag_name]);
    }
    return res;
}

std::string generate_keyword_meta_tag(const json& meta_data, const json& site_data) {
    std::string res = "";
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
        res += fmt::format("<meta name=\"keywords\" content=\"{}\" />\n", fmt::join(keywords, ", "));
    return res;
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

/**
 * @brief Generates html file header from provided configuration.
 *
 * The function generates html file header based on provided template, that should be stored in `header.html` file.
 * Also all metadata are loaded from `metadata.json` file.
 * 
 * @param site_data Generation configuration and data
 * @param extra_title Text that will be appended to predefined title.
 * @return std::string 
 */
std::string generate_header(const json& site_data, const std::string& extra_title = "") {
    static json meta_data = {};
    static const std::string header_template = load_file(get_template_path("header.html"));
    if (meta_data == json{}) {
        std::ifstream md_file(get_source_path("metadata.json"));
        md_file >> meta_data;
    }

    std::string complete_title = fmt::format("{} - {}", site_data["title"], meta_data["main_title"]);
    if (extra_title != "") {
        complete_title = fmt::format("{} - {}", extra_title, complete_title);
    }

    std::string meta_tags = "";
    meta_tags += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" />\n";

    meta_tags += "<meta name=\"generator\" content=\"NetMake - TODO: Add url\" />\n";
    meta_tags += get_meta_tag("description", meta_data);
    meta_tags += get_meta_tag("author", meta_data);
    meta_tags += get_meta_tag("application-name", meta_data);
    meta_tags += generate_keyword_meta_tag(meta_data, site_data);
    
    std::string res = "";
    res += fmt::format(header_template, fmt::arg("title", complete_title),
                                        fmt::arg("meta_tags", meta_tags),
                                        fmt::arg("extra_styles", generate_extra_styles(site_data)));
    return res;
}

std::string generate_nav(const std::string_view& site_name, const json& sites) {
    static std::string nav_template = load_file(get_template_path("nav.html"));
    std::string nav_items = "";
    for (auto& site: sites.items()) {
        if (site.key() != "index") {
            std::string edited_link = site.key();
            if (site.key().ends_with("*")) {
                edited_link = edited_link.substr(0, edited_link.size() - 2);
            }
            std::string site_title = site.value()["title"];
            if (site.key() == site_name) {
                nav_items += fmt::format("<span class=\"nav-item nav-current\"><a href=\"{}\" >{}</a>/<span>",
                                              edited_link,
                                              site_title);
            } else {
                nav_items += fmt::format("<span class=\"nav-item\"><a href=\"{}\" >{}</a>/<span>",
                                              edited_link,
                                              site_title);
            }
        }
    }
    std::string res = "";
    res += fmt::format(nav_template, fmt::arg("nav_items", nav_items));
    return res;
}

std::string generate_body(const std::string& site_name, const json& sites) {
    // const json site_data = sites[site_name];
    std::string site_template = load_file(get_template_path(fmt::format("{}.html", site_name)));
    std::string res = "";
    res += fmt::format("{}\n{}", generate_nav(site_name, sites), site_template);
    return res;
}

/**
 * @brief Generates one simple page based on the configuration.
 * 
 * @param site_name Name of the site (used for the file name)
 * @param sites Configuration
 */
void netmake::generate_simple_site(const std::string& site_name, const json& sites) {
    auto file = fmt::output_file(get_destination_path(fmt::format("{}.html", site_name)).c_str());
    file.print("<head>\n{}\n</head>\n<body>\n{}\n</body>\n",
               generate_header(sites[site_name]),
               generate_body(site_name, sites));
}

void netmake::generate_complex_site(const std::string& site_name, const json& sites) {
    std::string file_name = site_name.substr(0, site_name.size() - 2);
    auto main_site = fmt::output_file(get_destination_path(fmt::format("{}.html", file_name)).c_str());

    std::string main_site_template = load_file(get_template_path(fmt::format("{}.html", file_name)));
    std::string nav_bar = generate_nav(site_name, sites);

    auto list = sites[site_name]["items"];
    if (list.is_string()) {
        list = load_json(get_source_path(fmt::format("{}.json", list)));
    }

    std::filesystem::create_directories(get_destination_path(file_name));
    std::string generated_list = "";
    if (list.size() > 0) {
        std::string item_template_name = sites[site_name]["list_item_template"];
        std::string item_template = load_file(get_template_path(fmt::format("{}.html", item_template_name)));
        for (auto item: list) {
            std::string url = fmt::format("{}/{}.html", file_name, item["name"]);
            generated_list += fmt::format(item_template, fmt::arg("item_title", item["title"]),
                                                         fmt::arg("item_description", item["description"]),
                                                         fmt::arg("item_url", url));
            
            auto site = fmt::output_file(get_destination_path(url).c_str());
            std::string site_template = load_file(get_template_path(fmt::format("{}.html", item["name"])));
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