#include "generate.hpp"
#include "settings/settings.hpp"
#include "simple_page_generator.hpp"

#include <fstream>
#include <stdexcept>
#include <filesystem>
#include <string>

using namespace netmake;
using namespace std::filesystem;

// void netmake::generate() {
//     if (!is_directory(settings::source_dir)) {
//         throw std::runtime_error(fmt::format("Can't generate website. Source directory {} doesn't exists", settings::source_dir.c_str()));
//     }

//     if (!is_directory(settings::dest_dir)) {
//         create_directories(settings::dest_dir);
//     }

//     std::ifstream ifs(settings::source_dir / path{"pages.json"});
//     json site_defs;
//     ifs >> site_defs;
//     simple_page_generator::sites = site_defs;

//     for (auto& item: site_defs["pages"].items()) {
//         if (item.key().ends_with('*')) {
//             // generate_complex_site(item.key(), sites);
//         } else {
//             simple_page_generator ssg{item.key(), item.value()};
//             ssg.generate();
//         }
//     }
// }

// void netmake::generate_complex_site(const std::string& site_name, const json& sites) {
//     std::string file_name = site_name.substr(0, site_name.size() - 2);
//     auto main_site = fmt::output_file(get_destination_path(fmt::format("{}.html", file_name)).c_str());

//     std::string main_site_template = load_file(get_template_path(fmt::format("{}.html", file_name)));
//     std::string nav_bar = generate_nav(site_name, sites);

//     auto list = sites[site_name]["items"];
//     if (list.is_string()) {
//         list = load_json(get_source_path(fmt::format("{}.json", list)));
//     }

//     std::filesystem::create_directories(get_destination_path(file_name));
//     std::string generated_list = "";
//     if (list.size() > 0) {
//         std::string item_template_name = sites[site_name]["list_item_template"];
//         std::string item_template = load_file(get_template_path(fmt::format("{}.html", item_template_name)));
//         for (auto item: list) {
//             std::string url = fmt::format("{}/{}.html", file_name, item["name"]);
//             generated_list += fmt::format(item_template, fmt::arg("item_title", item["title"]),
//                                                          fmt::arg("item_description", item["description"]),
//                                                          fmt::arg("item_url", url));
            
//             auto site = fmt::output_file(get_destination_path(url).c_str());
//             std::string site_template = load_file(get_template_path(fmt::format("{}.html", item["name"])));
//             site.print("<head>\n{}\n</head>\n<body>\n{}\n{}\n</body>\n",
//                generate_header(sites[site_name], item["title"]),
//                nav_bar,
//                fmt::format(site_template, fmt::arg("generated_list", generated_list)));
//         }
//     }

//     main_site.print("<head>\n{}\n</head>\n<body>\n{}\n{}\n</body>\n",
//                generate_header(sites[site_name]),
//                nav_bar,
//                fmt::format(main_site_template, fmt::arg("generated_list", generated_list)));
// }