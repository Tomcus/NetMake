#include <fmt/core.h>

#include "generation/generate.hpp"
#include "settings/settings.hpp"

#include <string>
#include <chrono>

using namespace netmake;
using namespace std::chrono;

inline std::vector<std::string> convert_args(int argc, const char* args[]) {
    std::vector<std::string> res(argc);
    for (int i = 0; i < argc; ++i) {
        res.emplace_back(args[i]);
    }
    return res;
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
    } catch (std::exception& e) {
        fmt::print(stderr, "Error occured during generation.\nError message: {}\n", e.what());
        return 1;
    }
    return 0;
}