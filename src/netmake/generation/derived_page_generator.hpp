#ifndef NETMAKE_DERIVED_PAGE_GENERATOR_HPP
#define NETMAKE_DERIVED_PAGE_GENERATOR_HPP

#include "simple_page_generator.hpp"

namespace netmake {
    class derived_page_generator : public simple_page_generator {
    public:
        derived_page_generator(const std::string& site_name, const json& site_data,
                               const std::filesystem::path& path_to_extend, const std::string& original_title);
    protected:
        
    };
}

#endif//NETMAKE_DERIVED_PAGE_GENERATOR_HPP