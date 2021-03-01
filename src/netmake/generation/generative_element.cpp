#include "generative_element.hpp"

#include <algorithm>
#include <stdexcept>

using namespace netmake::generation;

generation_error::generation_error(const std::string_view& msg, const source_location& loc): d20::exception(loc), err_msg(msg) { }

const char* generation_error::what() const noexcept {
    return err_msg.c_str();
}

generative_element_impl::~generative_element_impl() {
    if (data != nullptr) {
        delete [] data;
    }
}