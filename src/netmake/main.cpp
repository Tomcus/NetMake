#include <fmt/core.h>

#include "json.hpp"
#include "generate.hpp"
#include "settings.hpp"

#include <string>
#include <fstream>

using namespace netmake;

// void generate(const std::string& project_def_path) {
//     json projects;
//     std::ifstream ifs(project_def_path);
//     ifs >> projects;
//     for (auto& item: projects.items()) {
//         if (item.key().ends_with('*')) {
//             fmt::print("TODO: Complex generation");
//         } else {
//             generate_simple_site(item.key(), item.value());
//         }
//     }
// }

inline std::vector<std::string> parse_args(int argc, const char* args[]) {
    std::vector<std::string> res(argc);
    for (int i = 0; i < argc; ++i) {
        res.emplace_back(args[i]);
    }
    return res;
}

int main(int argc, const char* args[]) {
    std::vector<std::string> vargs = parse_args(argc, args);
    settings::init(vargs);
    fmt::print("src: {}\ndest: {}\n", settings::source_dir, settings::dest_dir);
    return 0;
}