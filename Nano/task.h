#pragma once
#include <memory>

namespace Nano {
	namespace Concurrency {
        class Task
        {
            struct impl_base {
                virtual void call() = 0;
                virtual ~impl_base() {}
            };
            std::unique_ptr<impl_base> impl;
            template<typename F>
            struct impl_type : impl_base
            {
                F f;
                impl_type(F&& f_) : f(std::move(f_)) {}
                void call() { f(); }
            };
        public:
            template<typename F>
            Task(F&& f) :
                impl(new impl_type<F>(std::move(f)))
            {}
            void operator()() { impl->call(); }
            Task() = default;
            Task(Task&& other) noexcept :
                impl(std::move(other.impl))
            {}
            Task& operator=(Task&& other) noexcept
            {
                impl = std::move(other.impl);
                return *this;
            }
            Task(const Task&) = delete;
            Task(Task&) = delete;
            Task& operator=(const Task&) = delete;
        };
	}
}