
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
		static_assert(std::is_base_of<component, Type>::value, "intrusive_ptr only supports components");

		intrusive_ptr() noexcept =default;
		intrusive_ptr(std::nullptr_t) noexcept {}

		explicit intrusive_ptr(Type * ptr) noexcept : ptr{ptr} {}

		intrusive_ptr(const intrusive_ptr & other) noexcept : ptr{other.ptr} { ptr->cwc$component$new$0(); }
		intrusive_ptr(intrusive_ptr && other) noexcept { swap(other); }

		auto operator=(const intrusive_ptr & other) -> intrusive_ptr & {
			intrusive_ptr tmp{other};
			swap(tmp);
			return *this;
		}
		auto operator=(intrusive_ptr && other) noexcept -> intrusive_ptr & { swap(other); return *this; }

		~intrusive_ptr() noexcept { if(ptr) ptr->cwc$component$delete$1(); }

		auto operator->() const noexcept -> Type * { assert(ptr); return  ptr; }

		explicit operator bool() const noexcept { return ptr != nullptr; }
		auto operator!() const noexcept -> bool { return ptr == nullptr; }

		template<typename OtherType>
		operator intrusive_ptr<OtherType>() const {
			static_assert(!std::is_const<Type>::value || (std::is_const<Type>::value && std::is_const<OtherType>::value), "constness violation detected");
			if(!ptr) return intrusive_ptr<OtherType>{};
			typename std::remove_const<OtherType>::type * tmp;
			internal::call(*ptr, &component::cwc$component$cast$2, OtherType::cwc_uuid(), reinterpret_cast<void **>(&tmp));
			return intrusive_ptr<OtherType>{tmp};
		}

		void reset() noexcept {
			if(!ptr) return;
			intrusive_ptr tmp;
			swap(tmp);
		}

		void swap(intrusive_ptr & other) noexcept {
			using std::swap;
			swap(ptr, other.ptr);
		}

		template<typename OtherType>
		friend
		auto operator==(const intrusive_ptr & lhs, const intrusive_ptr<OtherType> & rhs) noexcept ->  bool { return lhs.ptr == rhs.ptr; }

		template<typename OtherType>
		friend
		auto operator!=(const intrusive_ptr & lhs, const intrusive_ptr<OtherType> & rhs) noexcept ->  bool { return !(lhs == rhs); }

		template<typename OtherType>
		friend
		auto operator<(const intrusive_ptr & lhs, const intrusive_ptr<OtherType> & rhs) noexcept ->  bool { lhs.ptr < rhs.ptr; }

		template<typename OtherType>
		friend
		auto operator>(const intrusive_ptr & lhs, const intrusive_ptr<OtherType> & rhs) noexcept ->  bool { return rhs < lhs; }

		template<typename OtherType>
		friend
		auto operator<=(const intrusive_ptr & lhs, const intrusive_ptr<OtherType> & rhs) noexcept ->  bool { return !(rhs < lhs); }

		template<typename OtherType>
		friend
		auto operator>=(const intrusive_ptr & lhs, const intrusive_ptr<OtherType> & rhs) noexcept ->  bool { return !(lhs < rhs); }
	private:
		Type * ptr{nullptr};
	};
	CWC_PACK_END

	template<typename Type, typename... Args>
	auto make_intrusive(Args &&... args) -> intrusive_ptr<Type> { return intrusive_ptr<Type>{new Type{std::forward<Args>(args)...}}; }

	template<typename Type>
	void swap(intrusive_ptr<Type> & lhs, intrusive_ptr<Type> & rhs) noexcept { lhs.swap(rhs); }
}