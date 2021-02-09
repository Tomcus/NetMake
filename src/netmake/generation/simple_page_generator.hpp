#ifndef NETMAKE_SIMPLE_PAGE_GENERATOR_HPP
#define NETMAKE_SIMPLE_PAGE_GENERATOR_HPP

#include <string>
#include <filesystem>
#include <functional>

#include <fmt/os.h>

#include "json.hpp"

namespace netmake {
    class simple_page_generator {
    public:
        simple_page_generator(const std::string& site_name, const json& site_data);

        virtual void generate() const;
        void offset(const std::filesystem::path& new_offset);

        static json sites;
    protected:
        std::string generate_extra_styles() const;
        std::string generate_keyword_meta_tag(const json& meta_data) const;
        std::string get_meta_tag(const std::string& meta_tag_name, const json& meta_data) const;
        std::string generate_pages_list(const std::string& template_name) const;
        virtual std::string generate_nav() const;

        void generate_header(const fmt::ostream& file) const;
        void generate_body(const fmt::ostream& file) const;
        void generate_footer(const fmt::ostream& file) const;
        
        static json load_json(const std::filesystem::path& path_to_json);
        static std::string load_file(const std::filesystem::path& path_to_file);

        std::filesystem::path get_destination_path(const std::filesystem::path& file) const;
        std::filesystem::path get_source_path(const std::filesystem::path& file) const;
        std::filesystem::path get_template_path(const std::filesystem::path& file) const;

        std::string name;
        json data;
        std::filesystem::path extra_path{};
        std::string extra_title{};
    };
}

#endif//NETMAKE_SIMPLE_PAGE_GENERATOR_HPP