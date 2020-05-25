
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once

namespace cwc {
	class component;

	CWC_PACK_BEGIN
	//! @brief a smart pointer managing objects with embedded reference counts
	//! @tparam Type type of the managed object
	template<typename Type>
	struct handle final {
		static_assert(std::is_base_of_v<component, Type>);

		handle() noexcept =default;

		handle(const handle & other) noexcept : ptr{other.ptr} { if(ptr) ptr->cwc$component$new$0(); }
		handle(handle && other) noexcept { swap(other); }

		auto operator=(handle other) -> handle & {
			swap(other);
			return *this;
		}

		~handle() noexcept { if(ptr) ptr->cwc$component$delete$1(); }

		template<typename OtherType, typename = std::enable_if_t<!std::is_const_v<Type> || (std::is_const_v<Type> && std::is_const_v<OtherType>)>>
		operator handle<OtherType>() const & {
			if(!ptr) return {};
			OtherType * result;
			default_error_context error;
			ptr->cwc$component$dynamic_cast$2(&error, &internal::interface_id_v<OtherType>, (void **)(&result));
			error.rethrow_if_necessary();
			return result;
		}

		template<typename OtherType, typename = std::enable_if_t<!std::is_const_v<Type> || (std::is_const_v<Type> && std::is_const_v<OtherType>)>>
		operator handle<OtherType>() && {
			if(!ptr) return {};
			OtherType * result;
			default_error_context error;
			ptr->cwc$component$dynamic_cast_fast$3(&error, &internal::interface_id_v<OtherType>, (void **)(&result));
			error.rethrow_if_necessary();
			ptr = nullptr;
			return result;
		}

		auto operator*() const noexcept -> Type & { return *ptr; }
		auto operator->() const noexcept -> Type * { return ptr; }

		explicit
		operator bool() const noexcept { return ptr != nullptr; }
		auto operator!() const noexcept -> bool { return ptr == nullptr; }

		void reset() noexcept { handle{}.swap(*this); }

		void swap(handle & other) noexcept {
			using std::swap;
			swap(ptr, other.ptr);
		}
		friend
		void swap(handle & lhs, handle & rhs) noexcept { lhs.swap(rhs); }

		friend
		auto operator<<(std::ostream & os, const handle & self) -> std::ostream & { return os << self.ptr;  }
	private:
		template<typename OtherType>
		friend
		struct handle;

		template<typename Implementation, typename... Args>
		friend
		auto make_handle(Args &&...) -> handle<component>;

		handle(Type * ptr) noexcept : ptr{ptr} {}

		Type * ptr{nullptr};
	};
	CWC_PACK_END

	inline
	auto operator==(const handle<component> & lhs, const handle<component> & rhs) noexcept -> bool { return std::addressof(*lhs) == std::addressof(*rhs); }

	inline
	auto operator!=(const handle<component> & lhs, const handle<component> & rhs) noexcept -> bool { return !(lhs == rhs); }

	inline
	auto operator< (const handle<component> & lhs, const handle<component> & rhs) noexcept -> bool { return std::addressof(*lhs) < std::addressof(*rhs); }

	inline
	auto operator<=(const handle<component> & lhs, const handle<component> & rhs) noexcept -> bool { return !(rhs < lhs); }

	inline
	auto operator> (const handle<component> & lhs, const handle<component> & rhs) noexcept -> bool { return rhs < lhs; }

	inline
	auto operator>=(const handle<component> & lhs, const handle<component> & rhs) noexcept -> bool { return !(lhs < rhs); }

	template<typename Implementation, typename... Args>
	auto make_handle(Args &&... args) -> handle<component> {
		using TL = typename Implementation::cwc_interfaces;
		static_assert(!TL::empty);
		static_assert(std::is_same_v<component, typename TL::template at<0>>);
		constexpr auto Index{TL::size == 1 ? 0 : 1};//component is only valid identity if there are no other interfaces
		using Interface = typename TL::template at<Index>;
		//this indirection via an Interface ensures the identity relation for components
		return handle<Interface>{new Implementation{std::forward<Args>(args)...}};
	}
}
