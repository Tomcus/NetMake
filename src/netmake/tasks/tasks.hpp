#ifndef NETMAKE_TASKS_HPP
#define NETMAKE_TASKS_HPP

#include <memory>
#include <thread>
#include <vector>

namespace netmake {
    class basic_task {
    public:
        void do_task();
        constexpr bool is_ending() const;
        constexpr bool is_finished() const;
    protected:
        constexpr explicit basic_task(bool is_ending);
        virtual void process_task() const;
        bool ending{false};
        bool finished{false};
    };
    using task = std::shared_ptr<basic_task>;

    class task_processor {
        public:
            const std::size_t TASK_POOL_CAPACITY = 10;
            static std::vector<std::thread> pool;
            static std::vector<task> to_process;
            static void add_task(task new_task) noexcept;
            static void init(std::size_t numb_of_thread = std::thread::hardware_concurrency()) noexcept;
            static void end_and_await() noexcept;
    };
}

#endif//NETMAKE_TASKS_HPP