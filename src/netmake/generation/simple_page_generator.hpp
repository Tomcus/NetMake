#ifndef NETMAKE_SIMPLE_PAGE_GENERATOR_HPP
#define NETMAKE_SIMPLE_PAGE_GENERATOR_HPP

#include <string>
#include <string_view>
#include <filesystem>
#include <unordered_map>

#include <fmt/os.h>

#include "json.hpp"

namespace netmake {
    class simple_page_generator {
    public:
        simple_page_generator(const std::string& site_name, const json& site_data);

        virtual void generate() const;
        void offset(const std::string_view& new_offset, const std::string_view& new_title);
        void set_parent_header(const std::string_view& header);

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
        static std::filesystem::path get_source_path(const std::filesystem::path& file);
        static std::filesystem::path get_template_path(const std::filesystem::path& file);

        static std::string get_template(const std::string& template_name);
        static std::string get_template(const std::string& template_name, const std::string& fallback);
        static std::unordered_map<std::string, std::string> template_store;

        template <char from, char to>
        static std::string replace_in_string(const std::string_view& str);

        std::string name;
        json data;
        std::string extra_offset{};
        std::string override_title{};
        std::string parrent_header{};
    };
}

#endif//NETMAKE_SIMPLE_PAGE_GENERATOR_HPP