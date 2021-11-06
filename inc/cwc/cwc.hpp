
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
#include <cstdint>

namespace cwc::internal {
	enum class error_selector : std::uint32_t;

	using error_callback = const void * (CWC_CALL *)(error_selector) noexcept;

	auto catch_() noexcept -> error_callback;

	template<typename Functor>
	auto try_(Functor func) noexcept -> error_callback {
		try { func(); return nullptr; }
		catch(...) { return catch_(); }
	}

	void throw_(error_callback callback); //TODO: [C++??] precondition(callback);


	template<typename T>
	struct vtable_access final { using type = typename T::cwc_vtable; };


	template<typename VFunc>
	struct extract_vtable;

	template<typename Class, typename T>
	struct extract_vtable<T Class::*> { using type = Class; };


	class dll final { //TODO: name is not ideal as this not just a reference to the DLL but also to the respective entry-point...
		struct native_handle;
		const std::unique_ptr<const native_handle> ref;

		const void * vptr;
	public:
		dll(const char * dll, const char * class_);
		~dll() noexcept;

		template<auto VFunc, typename... Args>
		void call(Args &&... args) const {
			static_assert(std::is_member_object_pointer_v<decltype(VFunc)>); //TODO: [C++20] make requirement

			using Vtable = typename extract_vtable<decltype(VFunc)>::type;
			const auto vtable{reinterpret_cast<const Vtable *>(vptr)};
			const auto call_{[&] { return (vtable->*VFunc)(std::forward<Args>(args)...); }};
			using Result = decltype(call_());

			if constexpr(std::is_same_v<Result, void>) call_();
			else {
				static_assert(std::is_same_v<Result, error_callback>);
				if(const auto error{call_()}) throw_(error);
			}
		}
	};
}
