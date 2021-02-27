#ifndef NETMAKE_GENERATIVE_ELEMENT
#define NETMAKE_GENERATIVE_ELEMENT

#include <vector>
#include <memory>
#include <atomic>

namespace netmake::generation {

    struct generative_element_impl;
    using generative_element = std::shared_ptr<generative_element_impl>;

    /// Generic template for generating data
    struct generative_element_impl {
        virtual ~generative_element_impl();

        /// Checks if can start generating
        constexpr bool can_generate() noexcept const;
        /// Check if generation is done
        constexpr bool is_done() noexcept const;
        /// Retrieve generation result
        constexpr const char* get_result() const;

        /// Virtual generation function must be implemented by children structures
        virtual void generate() = 0;
    protected:
        /// List of dependecies (aka. generators)
        std::vector<generative_element> dependecies{};
        /// Data storage
        char* data{nullptr};
        /// Atomic boolean to store state of generation.
        std::atomic_bool done{false};
    };
}

#endif//NETMAKE_GENERATIVE_ELEMENT