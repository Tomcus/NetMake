#ifndef NETMAKE_TEMPLATE_GENERATOR
#define NETMAKE_TEMPLATE_GENERATOR

#include "generative_element.hpp"
#include "json.hpp"

#include <memory>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace netmake::generation {
    /// Simplified storage for text ("large" data)
    using text = std::vector<char>;
    struct template_generator_impl;
    using template_generator = std::shared_ptr<template_generator_impl>;

    /// Structure for generating parial data from predefined templates
    struct template_generator_impl : public generative_element_impl {
        /// Structure must be defined by template name and page info data structure.
        template_generator_impl(const std::string_view template_name, const json page_info);
        /// Generate method that creates data
        void generate() override;
    protected:
        /// Name of temlate to use
        std::string_view name;
        /// Page data that is used in generating page data
        json page_data;
        /// Template cache that stores templates by its names
        static std::unordered_map<std::string_view, text> template_cache;
        /// Wrapper to aquire template from cache or to load it from file
        static text get_template(const std::string_view template_name);
        /// Helper functions that converts page data json into fmt compatible argument storage
        fmt::format_arg_store parse_args() const;
    };
}

#endif//NETMAKE_TEMPLATE_GENERATOR