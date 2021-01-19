#include <fmt/core.h>

#include "json.hpp"
#include "generate.hpp"
#include "settings.hpp"

#include <string>
#include <fstream>

using namespace netmake;

void generate() {
    json sites;
    std::ifstream ifs(settings::source_dir + "/sites.json");
    ifs >> sites;
    for (auto& item: sites.items()) {
        if (item.key().ends_with('*')) {
            generate_complex_site(item.key(), item.value());
        } else {
            generate_simple_site(item.key(), item.value());
        }
    }
}

inline std::vector<std::string> parse_args(int argc, const char* args[]) {
    std::vector<std::string> res(argc);
    for (int i = 0; i < argc; ++i) {
        res.emplace_back(args[i]);
    }
    return res;
}

int main(int argc, const char* args[]) {
    try {
        std::vector<std::string> vargs = parse_args(argc, args);
        settings::init(vargs);
        generate();
    } catch (std::exception& e) {
        fmt::print(stderr, "Error occured during generation.\nError message: {}\n", e.what());
        return 1;
    }
    return 0;
}