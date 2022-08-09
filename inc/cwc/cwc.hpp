
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


	template<typename T>
	class result final {
		static_assert(!std::is_reference_v<T> && !std::is_pointer_v<T>);

		unsigned char data[sizeof(T) + 1]{};
	public:
		result() noexcept =default;
		result(const result &) =delete;
		auto operator=(const result &) -> result & =delete;
		~result() noexcept { if(data[sizeof(T)]) reinterpret_cast<T *>(data)->~T(); }

		void operator=(T && val) noexcept {
			//TODO: assert(!data[sizeof(T)]);
			new(data) T{std::move(val)};
			data[sizeof(T)] = true;
		}

		auto return_() noexcept -> T {
			//TODO: assert(data[sizeof(T)]);
			return std::move(*reinterpret_cast<T *>(data));
		}
	};


	template<typename>
	struct extract_vtable;

	template<typename Class, typename T>
	struct extract_vtable<T Class::*> { using type = Class; };

	template<typename VFunc>
	using extract_vtable_t = typename extract_vtable<VFunc>::type;


	template<typename>
	struct is_result_specialization : std::false_type {};

	template<typename T>
	struct is_result_specialization<result<T>> : std::true_type {};

	template<typename T>
	inline
	constexpr
	bool is_result_specialization_v{is_result_specialization<T>::value};


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


	template<typename>
	struct is_returning;

	template<typename Class, typename... Args>
	class is_returning<void(* Class::*)(Args...) noexcept> {
		static
		constexpr
		auto compute() noexcept -> bool {
			if constexpr(sizeof...(Args) != 0) {
				const bool tmp[]{is_result_specialization_v<std::remove_pointer_t<Args>>...};
				return tmp[sizeof...(Args) - 1];
			} else return false;
		}
	public:
		static
		constexpr
		bool value{compute()};
	};

	template<typename VFunc>
	inline
	constexpr
	bool is_returning_v{is_returning<VFunc>::value};


	template<typename>
	struct extract_return_type;

	template<typename Class, typename... Args>
	class extract_return_type<void(* Class::*)(Args...) noexcept> {
		template<typename T>
		struct type_identity final { using type = T; }; //TODO: [C++20] replace with std::type_identity

		template<typename T, typename... Ts>
		static
		constexpr
		auto compute() noexcept {
			if constexpr(sizeof...(Ts) != 0) return compute<Ts...>();
			else return type_identity<std::remove_pointer_t<T>>{};
		}
	public:
		using type = typename decltype(compute<Args...>())::type;
	};

	template<typename VFunc>
	using extract_return_type_t = typename extract_return_type<VFunc>::type;


	class context final {
		struct native_handle;
		const std::unique_ptr<const native_handle> lib;

		const void * vptr;
	public:
		context(const char * dll, const char * class_, version ver);
		~context() noexcept;

		template<auto VFunc, typename... Args>
		auto call(Args &&... args) const {
			using VFuncT = decltype(VFunc);
			static_assert(std::is_member_object_pointer_v<VFuncT>);
			const auto vtable{reinterpret_cast<const extract_vtable_t<VFuncT> *>(vptr)};
			exception exc;
			if constexpr(is_returning_v<VFuncT>) {
				extract_return_type_t<VFuncT> res;
				if constexpr(is_nothrow_v<VFuncT>) (vtable->*VFunc)(std::forward<Args>(args)..., &res);
				else {
					(vtable->*VFunc)(&exc, std::forward<Args>(args)..., &res);
					exc.throw_();
				}
				return res.return_();
			} else {
				if constexpr(is_nothrow_v<VFuncT>) (vtable->*VFunc)(std::forward<Args>(args)...);
				else {
					(vtable->*VFunc)(&exc, std::forward<Args>(args)...);
					exc.throw_();
				}
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
