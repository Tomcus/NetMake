#include "generate.hpp"
#include "path.hpp"
#include "settings/settings.hpp"

#include <fmt/os.h>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <filesystem>
#include <memory>
#include <cstring>

using namespace netmake;
using namespace std::filesystem;

class text_obj {
    char * ptr{nullptr};
    size_t curr_size{0};
    size_t curr_capacity{0};
    const size_t INITIAL_CAPACITY{128};
protected:
    void resize() {
        const auto old_data = ptr;

        if (curr_capacity == 0) {
            curr_capacity = INITIAL_CAPACITY;
        } else {
            curr_capacity *= 2;
        }

        ptr = new char[curr_capacity + 1];

        if (old_data != nullptr) {
            memcpy(ptr, old_data, curr_size);
        }
    }

    void append_raw(const char* data, size_t size) {
        const auto new_size = curr_size + size;
        
        if (new_size > curr_capacity) {
            resize();
        }

        if (size > 0) {
            memcpy(ptr + curr_size, data, size);
        }
        curr_size += size;
        if (ptr != nullptr) {
            ptr[curr_size] = 0;
        }
    }
public:
    const char* data() const {
        if (ptr == nullptr) 
            return "";
        return ptr;
    }

    size_t size() const {
        return curr_size;
    }

    void append(const std::string& str) {
        append_raw(str.data(), str.size());
    }

    void append(const std::shared_ptr<text_obj>& text) {
        append_raw(text->data(), text->size());
    }
};

using text = std::shared_ptr<text_obj>;
text create_text() { return std::make_shared<text_obj>(); }

text load_file(const path& path) {
    text res = create_text();
    std::ostringstream oss{};
    std::ifstream ifs(path);
    if (!ifs.good()) {
        throw std::runtime_error(fmt::format("Can't open/find file {}", path.c_str()));
    }
    oss << ifs.rdbuf();
    res->append(oss.str());
    return res;
}

json load_json(const path& path) {
    json res;
    std::ifstream ifs(path);
    if (!ifs.good()) {
        throw std::runtime_error(fmt::format("Can't open/find file {}", path.c_str()));
    }
    ifs >> res;
    return res;
}

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

text get_meta_tag(const std::string& meta_tag_name, const json& meta_data) {
    text res = std::make_shared<text_obj>();
    if (meta_data.contains(meta_tag_name)) {
        res->append(fmt::format("<meta name=\"{}\" content=\"{}\" />\n", meta_tag_name, meta_data[meta_tag_name]));
    }
    return res;
}

text generate_keyword_meta_tag(const json& meta_data, const json& site_data) {
    text res = create_text();
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
        res->append(fmt::format("<meta name=\"keywords\" content=\"{}\" />\n", fmt::join(keywords, ", ")));
    return res;
}

text generate_extra_styles(const json& site_data) {
    text res = create_text();
    if (site_data.contains("extra_css")) {
        auto extra_styles = site_data["extra_css"];
        for (auto style: extra_styles) {
            res->append(fmt::format("<link rel=\"stylesheet\" href=\"{}\" />", style));
        }
    }
    return res;
}

text generate_header(const json& site_data, const std::string& extra_title = "") {
    static json meta_data = {};
    static text header_template = load_file(get_template_path("header.html"));
    if (meta_data == json{}) {
        std::ifstream md_file(get_source_path("metadata.json"));
        md_file >> meta_data;
    }

    std::string complete_title = fmt::format("{} - {}", site_data["title"], meta_data["main_title"]);
    if (extra_title != "") {
        complete_title = fmt::format("{} - {}", extra_title, complete_title);
    }

    text meta_tags = create_text();
    meta_tags->append("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" />\n");

    meta_tags->append("<meta name=\"generator\" content=\"NetMake - TODO: Add url\" />\n");
    meta_tags->append(get_meta_tag("description", meta_data));
    meta_tags->append(get_meta_tag("author", meta_data));
    meta_tags->append(get_meta_tag("application-name", meta_data));
    meta_tags->append(generate_keyword_meta_tag(meta_data, site_data));
    
    text res = create_text();
    res->append(fmt::format(header_template->data(), fmt::arg("title", complete_title),
                                                     fmt::arg("meta_tags", meta_tags->data()),
                                                     fmt::arg("extra_styles", generate_extra_styles(site_data)->data())));
    return res;
}

text generate_nav(const std::string_view& site_name, const json& sites) {
    static text nav_template = load_file(get_template_path("nav.html"));
    text nav_items = create_text();
    for (auto& site: sites.items()) {
        if (site.key() != "index") {
            std::string edited_link = site.key();
            if (site.key().ends_with("*")) {
                edited_link = edited_link.substr(0, edited_link.size() - 2);
            }
            std::string site_title = site.value()["title"];
            if (site.key() == site_name) {
                nav_items->append(fmt::format("<span class=\"nav-item nav-current\"><a href=\"{}\" >{}</a>/<span>",
                                              edited_link,
                                              site_title));
            } else {
                nav_items->append(fmt::format("<span class=\"nav-item\"><a href=\"{}\" >{}</a>/<span>",
                                              edited_link,
                                              site_title));
            }
        }
    }
    text res = create_text();
    res->append(fmt::format(nav_template->data(), fmt::arg("nav_items", nav_items->data())));
    return res;
}

text generate_body(const std::string& site_name, const json& sites) {
    // const json site_data = sites[site_name];
    text site_template = load_file(get_template_path(fmt::format("{}.html", site_name)));
    text res = create_text();
    res->append(fmt::format("{}\n{}", generate_nav(site_name, sites)->data(), site_template->data()));
    return res;
}

void netmake::generate_simple_site(const std::string& site_name, const json& sites) {
    auto file = fmt::output_file(get_destination_path(fmt::format("{}.html", site_name)).c_str());
    file.print("<head>\n{}\n</head>\n<body>\n{}\n</body>\n",
               generate_header(sites[site_name])->data(),
               generate_body(site_name, sites)->data());
}

void netmake::generate_complex_site(const std::string& site_name, const json& sites) {
    std::string file_name = site_name.substr(0, site_name.size() - 2);
    auto main_site = fmt::output_file(get_destination_path(fmt::format("{}.html", file_name)).c_str());

    text main_site_template = load_file(get_template_path(fmt::format("{}.html", file_name)));
    text nav_bar = generate_nav(site_name, sites);

    auto list = sites[site_name]["items"];
    if (list.is_string()) {
        list = load_json(get_source_path(fmt::format("{}.json", list)));
    }

    std::filesystem::create_directories(get_destination_path(file_name));
    text generated_list = create_text();
    if (list.size() > 0) {
        std::string item_template_name = sites[site_name]["list_item_template"];
        text item_template = load_file(get_template_path(fmt::format("{}.html", item_template_name)));
        for (auto item: list) {
            std::string url = fmt::format("{}/{}.html", file_name, item["name"]);
            generated_list->append(fmt::format(item_template->data(), fmt::arg("item_title", item["title"]),
                                                                      fmt::arg("item_description", item["description"]),
                                                                      fmt::arg("item_url", url)));
            
            auto site = fmt::output_file(get_destination_path(url).c_str());
            text site_template = load_file(get_template_path(fmt::format("{}.html", item["name"])));
            site.print("<head>\n{}\n</head>\n<body>\n{}\n{}\n</body>\n",
               generate_header(sites[site_name], item["title"])->data(),
               nav_bar->data(),
               fmt::format(site_template->data(), fmt::arg("generated_list", generated_list->data())));
        }
    }

    main_site.print("<head>\n{}\n</head>\n<body>\n{}\n{}\n</body>\n",
               generate_header(sites[site_name])->data(),
               nav_bar->data(),
               fmt::format(main_site_template->data(), fmt::arg("generated_list", generated_list->data())));
}