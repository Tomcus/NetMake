#include "generative_element.hpp"

#include <algorithm>
#include <stdexcept>
#include <execution>

using namespace netmake::generation;

constexpr bool generative_element_impl::can_generate() noexcept const {
    atomic_bool result{true};
    std::transform(std::execution::par, dependecies.begin(), dependecies.end(), [&](const auto& dep){
        result &= dep->done();
    });
    return result;
}

constexpr bool generative_element_impl::is_done() noexcept const {
    return done;
}

constexpr const char* generative_element_impl::get_result() const {
    if (data != nullptr && done) {
        return data;
    }
    throw std::runtime_error("Can't return result until generation is done.");
}

generative_element_impl::~generative_element_impl() {
    if (data != nullptr) {
        delete [] data;
    }
}