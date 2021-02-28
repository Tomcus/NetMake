#include "template_generator.hpp"
#include "settings/settings.hpp"
#include "debug20/assert.hpp"

#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <algorithm>
#include <array>
#include <cstring>

#include <fmt/format.h>

std::unordered_map<std::string_view, text> template_generator_impl::template_cache{};

/// Default buffer size for loading templates
const size_t BUFFER_SIZE = 65536;

text template_generator_impl::get_template(const std::string_view template_name) {
    // try to find buffered template
    if (template_cache.contains(template_name)) {
        // if found return imediatly
        return template_cache.at(template_name);
    } else {
        // Generate file name
        fmt::memory_buffer file_name{};
        fmt::format_to(file_name, "{}.html", template_name);
        // Generate path to the template file
        std::filesystem::path path_to_template = settings::source_dir / std::filesystem::path{"templates"}
            / std::filesystem::path{file_name.data()};
        // Check if file exists and it is a regular file.
        d20::assert(std::filesystem::is_file(path_to_template), fmt::format("Template not found on path: {}", path_to_file.c_str()));

        // Open the template file
        std::ifstream template_file{};
        template_file.open(path_to_template.c_str());

        // Check if file was successfully opened
        d20::assert(template_file.good(), fmt::format("Can't open file at: {}", path_to_file.c_str()));

        // Load file into memory
        char buffer[BUFFER_SIZE];
        text result;
        while(!template_file.eof()) {
            buffer.clear();
            template_file.read(buffer, BUFFER_SIZE);
            
            result.resize(result.capacity() + template_file.gcount());
            char* ptr = result.data();
            std::advance(ptr, result.size());
            memcpy(ptr, buffer, template_file.gcount());
        }

        // Store loaded file into cache
        template_cache.emplace(template_name, result);
        // Return cached text
        return result;
    }
}

template_generator_impl::template_generator_impl(const std::string_view template_name):
                                                 name(template_name), page_data(page_info) { }

void template_generator_impl::generate() {
    // Check if data was already generated
    d20::assert(data == nullptr, "Data already generated. Please don't generate twice.");
    text& template = get_template(name);

    // Create memory buffer where to store temporary data
    fmt::memory_buffer res;
    fmt::vformat_to(res, template.data(), parse_args());

    // Copy data to local storage
    data = new char[res.size()];
    memcpy(data, res.data(), res.size());
    done = true;
}

fmt::format_arg_store template_generator_impl::parse_args() const {
    // Define argument storage
    fmt::format_arg_store args;

    // Scan page data and generate arguments
    for (auto page_info:page_data.items()) {
        if (page_info.value().is_array()) {
            args.push_back(fmt::join(page_info.value(), ", "));
        } else if (page_info.value().is_primitive()) {
            args.push_back(fmt::arg(page_info.key()));
        } else if (!page_info.value().is_null()) {
            throw generation_error(fmt::format("Can't parse argument of type {}", page_info.value().type()));
        }
    }
    // Return created arguments
    return args;
}