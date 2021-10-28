
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#if defined(__GNUG__)
	#define CWC_EXPORT __attribute__((visibility("default")))

	#ifdef __x86_64__
		#define CWC_CALL
	#else
		#define CWC_CALL __attribute__((__cdecl__))
	#endif
#elif defined(_MSC_VER)
	#define CWC_EXPORT __declspec(dllexport)

	#ifdef _M_X64
		#define CWC_CALL
	#else
		#define CWC_CALL __cdecl
	#endif
#else
	#error unknown compiler
#endif

#include <memory>

namespace cwc::internal {
	template<typename T>
	constexpr
	auto addressof(T && arg) noexcept -> void * { return const_cast<void *>(reinterpret_cast<const void *>(std::addressof(std::forward<T>(arg)))); }

	[[noreturn]]
	void unreachable() noexcept; //TODO: [C++23] use stacktrace to report error with trace

	class dll final {
		class impl;
		std::unique_ptr<impl> self;

		void invoke(int op, void * args[]) const;
	public:
		dll(const char * dll, const char * class_);
		~dll() noexcept;

		template<typename... Args>
		//TODO: [C++20] requires(std::is_pointer_v<std::decay_t<Args>> &&...)
		void operator()(int op, Args &&... args) const {
			void * tmp[]{args...};
			invoke(op, tmp);
		}
	};

	enum class error_selector : std::uint32_t;

	using error_callback = const void * (CWC_CALL *)(error_selector) noexcept;

	auto store_last_error() noexcept -> error_callback;

	void rethrow_last_error(error_callback callback);
}
