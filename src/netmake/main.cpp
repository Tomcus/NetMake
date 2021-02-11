#include <fmt/core.h>

#include "generation/generate.hpp"
#include "settings/settings.hpp"

#include <string>
#include <filesystem>
#include <chrono>
#include <fmt/core.h>

using namespace netmake;
using namespace std::chrono;

inline std::vector<std::string> convert_args(int argc, const char* args[]) {
    std::vector<std::string> res(argc);
    for (int i = 0; i < argc; ++i) {
        res.emplace_back(args[i]);
    }
    return res;
}

void copy_extra_files() {
    std::filesystem::copy(fmt::format("{}/to_copy", settings::source_dir.c_str()),
                          settings::dest_dir, std::filesystem::copy_options::recursive);
}

int main(int argc, const char* args[]) {
    try {
        std::vector<std::string> vargs = convert_args(argc, args);
        settings::init(vargs);
        auto start = std::chrono::high_resolution_clock::now();
        generate();
        auto end = std::chrono::high_resolution_clock::now();
        duration<double> diff = end - start;
        fmt::print("Generation finifhed in: {}ms\n", diff.count() * 1000);
        copy_extra_files()
    } catch (std::exception& e) {
        fmt::print(stderr, "Error occured during generation.\nError message: {}\n", e.what());
        return 1;
    }
    return 0;
}