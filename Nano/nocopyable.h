#pragma once

namespace Nano {
	class Noncopyable {
	public:
		Noncopyable() = default;
		virtual ~Noncopyable() = default;
		Noncopyable(const Noncopyable&) = delete;
		Noncopyable& operator=(const Noncopyable&) = delete;
	};
}
