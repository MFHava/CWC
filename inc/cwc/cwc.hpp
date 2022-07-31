
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#if defined(__GNUG__)
	#define CWC_EXPORT __attribute__((visibility("default")))
#elif defined(_MSC_VER)
	#define CWC_EXPORT __declspec(dllexport)
#else
	#error unknown compiler
#endif

#include <memory>
#include <cstdint>
#include <type_traits>

namespace cwc::internal {
	using version = std::uint8_t;


	struct alignas(std::uint64_t) header final {
		const version hversion{0};
		const std::uint8_t offset{sizeof(header) - 1};
		version cversion;
		std::uint8_t reserved[5]{};

		constexpr
		header(version cversion) noexcept : cversion{cversion} {}
	};
	static_assert(sizeof(header) == sizeof(std::uint64_t));
	static_assert(alignof(header) == alignof(std::uint64_t));
	static_assert(offsetof(header, hversion) == 0);
	static_assert(offsetof(header, offset) == 1);
	static_assert(offsetof(header, cversion) == 2);


	template<typename T>
	struct access final {
		template<typename Impl>
		static
		constexpr
		auto export_() { return T::template cwc_export<Impl>(); }

		static
		auto available() noexcept -> bool {
			try { T::cwc_context(); return true; }
			catch(...) { return false; }
		}
	};


	class exception final {
		unsigned char buffer[128]; //TODO: determine size, can't fallback to heap as this operation may NEVER fail!
		struct vtable;
		const vtable * vptr;

		template<typename T>
		void store(const T & exc) noexcept;

		void catch_() noexcept;
	public:
		exception() noexcept;
		exception(const exception &) =delete;
		auto operator=(const exception &) -> exception & =delete;
		~exception() noexcept;

		template<typename Func>
		void try_(Func func) noexcept
			try { func(); }
			catch(...) { catch_(); }

		void throw_() const;
	};


	template<typename>
	struct extract_vtable;

	template<typename Class, typename T>
	struct extract_vtable<T Class::*> { using type = Class; };

	template<typename VFunc>
	using extract_vtable_t = typename extract_vtable<VFunc>::type;


	template<typename>
	struct is_nothrow;

	template<typename Class, typename... Args>
	struct is_nothrow<void(* Class::*)(Args...) noexcept> : std::true_type {};

	template<typename Class, typename... Args>
	struct is_nothrow<void(* Class::*)(exception *, Args...) noexcept> : std::false_type {};

	template<typename VFunc>
	inline
	constexpr
	bool is_nothrow_v{is_nothrow<VFunc>::value};


	class context final {
		struct native_handle;
		const std::unique_ptr<const native_handle> lib;

		const void * vptr;
	public:
		context(const char * dll, const char * class_, version ver);
		~context() noexcept;

		template<auto VFunc, typename... Args>
		void call(Args &&... args) const {
			using VFuncT = decltype(VFunc);
			static_assert(std::is_member_object_pointer_v<VFuncT>);
			const auto vtable{reinterpret_cast<const extract_vtable_t<VFuncT> *>(vptr)};
			if constexpr(is_nothrow_v<VFuncT>) (vtable->*VFunc)(std::forward<Args>(args)...);
			else {
				exception exc;
				(vtable->*VFunc)(&exc, std::forward<Args>(args)...);
				exc.throw_();
			}
		}
	};
}


//! @brief C++ with Components API
namespace cwc {
	//! @brief exception thrown when type not derived from std::exception was caught at ABI boundary
	struct unknown_exception final : std::exception {
		auto what() const noexcept -> const char * override;
	};


	//! @brief check that type is available
	//! @tparam T type to check availability for
	//! @returns true iff the component is available
	//! @note unless the component is already loaded before, this operation tries to implicitly load it
	template<typename T>
	auto available() noexcept -> bool { return internal::access<T>::available(); }
}
