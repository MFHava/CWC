
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
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
	struct intrusive_ptr final {
		static_assert(std::is_base_of_v<component, Type>, "intrusive_ptr only supports components");

		intrusive_ptr() noexcept =default;
		intrusive_ptr(std::nullptr_t) noexcept {}

		explicit intrusive_ptr(Type * ptr) noexcept : ptr{ptr} {}

		intrusive_ptr(const intrusive_ptr & other) noexcept : ptr{other.ptr} { if(ptr) ptr->cwc$component$new$0(); }
		intrusive_ptr(intrusive_ptr && other) noexcept { swap(other); }

		auto operator=(const intrusive_ptr & other) -> intrusive_ptr & {
			intrusive_ptr tmp{other};
			swap(tmp);
			return *this;
		}
		auto operator=(intrusive_ptr && other) noexcept -> intrusive_ptr & { swap(other); return *this; }

		~intrusive_ptr() noexcept { if(ptr) ptr->cwc$component$delete$1(); }

		auto get() const noexcept -> Type * { return ptr; }

		auto operator*() const noexcept -> Type & { assert(ptr); return *ptr; }
		auto operator->() const noexcept -> Type * { assert(ptr); return ptr; }

		explicit operator bool() const noexcept { return ptr != nullptr; }
		auto operator!() const noexcept -> bool { return ptr == nullptr; }

		template<typename OtherType>
		operator intrusive_ptr<OtherType>() const & {
			static_assert(!std::is_const_v<Type> || (std::is_const_v<Type> && std::is_const_v<OtherType>), "constness violation detected");
			if(!ptr) return intrusive_ptr<OtherType>{};
			using TargetType = typename std::remove_const_t<OtherType>;
			TargetType * tmp;
			const uuid & id{internal::interface_id<TargetType>::get()};
			internal::validate(ptr->cwc$component$dynamic_cast$2(&id, reinterpret_cast<void **>(&tmp)));
			return intrusive_ptr<OtherType>{tmp};
		}

		template<typename OtherType>
		operator intrusive_ptr<OtherType>() && {
			static_assert(!std::is_const_v<Type> || (std::is_const_v<Type> && std::is_const_v<OtherType>), "constness violation detected");
			if(!ptr) return intrusive_ptr<OtherType>{};
			using TargetType = typename std::remove_const_t<OtherType>;
			TargetType * tmp;
			const uuid & id{internal::interface_id<TargetType>::get()};
			internal::validate(ptr->cwc$component$dynamic_cast_fast$3(&id, reinterpret_cast<void **>(&tmp)));
			ptr = nullptr;
			return intrusive_ptr<OtherType>{tmp};
		}

		void reset() noexcept { intrusive_ptr{}.swap(*this); }

		void swap(intrusive_ptr & other) noexcept {
			using std::swap;
			swap(ptr, other.ptr);
		}
		friend
		void swap(intrusive_ptr & lhs, intrusive_ptr & rhs) noexcept { lhs.swap(rhs); }

		friend
		auto operator<<(std::basic_ostream<utf8> & os, const intrusive_ptr & self) -> std::basic_ostream<utf8> & { return os << self.get();  }
	private:
		Type * ptr{nullptr};
	};
	CWC_PACK_END

	inline
	auto operator==(const intrusive_ptr<component> & lhs, const intrusive_ptr<component> & rhs) noexcept -> bool { return lhs.get() == rhs.get(); }

	inline
	auto operator!=(const intrusive_ptr<component> & lhs, const intrusive_ptr<component> & rhs) noexcept -> bool { return !(lhs == rhs); }

	inline
	auto operator< (const intrusive_ptr<component> & lhs, const intrusive_ptr<component> & rhs) noexcept -> bool { return lhs.get() < rhs.get(); }

	inline
	auto operator<=(const intrusive_ptr<component> & lhs, const intrusive_ptr<component> & rhs) noexcept -> bool { return !(rhs < lhs); }

	inline
	auto operator> (const intrusive_ptr<component> & lhs, const intrusive_ptr<component> & rhs) noexcept -> bool { return rhs < lhs; }

	inline
	auto operator>=(const intrusive_ptr<component> & lhs, const intrusive_ptr<component> & rhs) noexcept -> bool { return !(lhs < rhs); }

	template<typename Implementation, typename... Args>
	auto make_intrusive(Args &&... args) -> intrusive_ptr<component> {
		using Interface = internal::TL::at_t<typename Implementation::cwc_interfaces, 1>;//does not work for classes that implement no additional interfaces...
		static_assert(!std::is_same_v<Interface, component>);
		//this indirection via an Interface ensures the identity relation for components
		return intrusive_ptr<Interface>{new Implementation{std::forward<Args>(args)...}};
	}
}
