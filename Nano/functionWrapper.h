#pragma once
#include <memory>
#include "nocopyable.h"

namespace Nano {
	namespace Concurrency {
		class FunctionWrapper : public Noncopyable
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
			FunctionWrapper(F&& f) :
				impl(new impl_type<F>(std::move(f)))
			{}
			void operator()() { impl->call(); }
			FunctionWrapper() = default;
			FunctionWrapper(FunctionWrapper&& other) noexcept :
				impl(std::move(other.impl))
			{}
			FunctionWrapper& operator=(FunctionWrapper&& other) noexcept
			{
				impl = std::move(other.impl);
				return *this;
			}
		};
	}
}