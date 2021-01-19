#ifndef NETMAKE_GENERATE_HPP
#define NETMAKE_GENERATE_HPP

#include "json.hpp"
#include <string>

namespace netmake {
    void generate_simple_site(const std::string& site_name, const json& site_data);
    void generate_complex_site(const std::string& site_name, const json& site_data);
}

#endif//NETMAKE_GENERATE_HPP