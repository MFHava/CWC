
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once
#include <cassert>

namespace cwc {
	namespace internal {
		struct nullopt_helper;
		using nullopt_t = int nullopt_helper::*;
	}

	//! @brief global constant used to indicate an optional with uninitialized state
	constexpr internal::nullopt_t nullopt{nullptr};

	CWC_PACK_BEGIN
	//! @brief an optional value
	//! @tparam Type type of the potentially contained object
	//! @note the design of this class is derived from C++17
	template<typename Type>
	struct optional final {
		static_assert(!internal::is_component<Type>::value && !internal::is_interface<Type>::value, "optional only supports simple types");
		static_assert(std::is_standard_layout<Type>::value, "optional only supports standard layout types");

		using value_type = Type;

		//! @brief construct an empty optional
		optional() noexcept =default;

		//! @brief construct an empty optional
		optional(internal::nullopt_t) noexcept {}

		//! @brief copy constructor
		//! @param[in] other optional to copy
		//! @note iff other contains a value, that value will be copied into this instance
		optional(const optional & other) {
			if(!other) return;
			new(data) Type{*other};
			initialized = true;
		}

		//! @brief move constructor
		//! @param[in] other optional to move
		//! @note iff other contains a value, that value will be moved into this instance
		optional(optional && other) noexcept {
			if(!other) return;
			new(data) Type{std::move(*other)};
			initialized = true;
		}

		optional(const Type & value) : initialized{true} { new(data) Type{value}; }
		optional(Type && value) noexcept : initialized{true} { new(data) Type{std::move(value)}; }

		auto operator=(internal::nullopt_t) noexcept -> optional & {
			reset();
			return *this;
		}

		auto operator=(const optional & other) -> optional & {
			if(other) *this = *other;
			else reset();
			return *this;
		}
		auto operator=(optional && other) noexcept -> optional & {
			if(other) *this = std::move(*other);
			else reset();
			return *this;
		}

		auto operator=(const Type & value) -> optional & {
			if(initialized) **this = value;
			else new(data) Type{value};
			return *this;
		}
		auto operator=(Type && value) noexcept -> optional & {
			if(initialized) **this = std::move(value);
			else new(data) Type{std::move(value)};
			return *this;
		}

		~optional() noexcept { reset(); }

		auto operator->() const -> const Type * { return &**this; }
		auto operator->()       ->       Type * { return &**this; }

		auto operator*() const -> const Type & { assert(initialized); return reinterpret_cast<const Type &>(data); }
		auto operator*()       ->       Type & { assert(initialized); return reinterpret_cast<      Type &>(data); }

		explicit operator bool() const noexcept { return initialized; }
		auto operator!() const noexcept -> bool { return !**this; }

		void swap(optional & other) noexcept {
			if(!*this && !other) return;
			else if(*this && !other) {
				other = std::move(*this);
				reset();
			} else if(!*this && other) {
				*this = std::move(other);
				other.reset();
			} else {
				assert(*this && other);
				using std::swap;
				swap(**this, *other);
			}
		}

		//! @brief releases the contained object
		void reset() noexcept {
			if(!initialized) return;
			(**this).~Type();
			initialized = false;
		}
	private:
		uint8 data[sizeof(Type)];
		boolean initialized{false};
	};
	CWC_PACK_END

	template<typename Type>
	auto operator==(const optional<Type> & lhs, const optional<Type> & rhs) -> bool {
		if(static_cast<bool>(lhs) != static_cast<bool>(rhs)) return false;
		if(static_cast<bool>(lhs) == false) return true;
		return *lhs == *rhs;
	}

	template<typename Type>
	auto operator!=(const optional<Type> & lhs, const optional<Type> & rhs) -> bool {
		if(static_cast<bool>(lhs) != static_cast<bool>(rhs)) return true;
		if(static_cast<bool>(lhs) == false) return false;
		return *lhs != *rhs;
	}

	template<typename Type>
	auto operator< (const optional<Type> & lhs, const optional<Type> & rhs) -> bool {
		if(static_cast<bool>(rhs) == false) return false;
		if(static_cast<bool>(lhs) == false) return true;
		return *lhs < *rhs;
	}

	template<typename Type>
	auto operator<=(const optional<Type> & lhs, const optional<Type> & rhs) -> bool {
		if(static_cast<bool>(lhs) == false) return true;
		if(static_cast<bool>(rhs) == false) return false;
		return *lhs <= *rhs;
	}

	template<typename Type>
	auto operator> (const optional<Type> & lhs, const optional<Type> & rhs) -> bool {
		if(static_cast<bool>(lhs) == false) return false;
		if(static_cast<bool>(rhs) == false) return true;
		return *lhs > *rhs;
	}

	template<typename Type>
	auto operator>=(const optional<Type> & lhs, const optional<Type> & rhs) -> bool {
		if(static_cast<bool>(rhs) == false) return true;
		if(static_cast<bool>(lhs) == false) return false;
		return *lhs >= *rhs;
	}

	template<typename Type>
	auto operator==(const optional<Type> & opt, internal::nullopt_t) noexcept -> bool { return !opt; }

	template<typename Type>
	auto operator==(internal::nullopt_t, const optional<Type> & opt) noexcept -> bool { return !opt; }

	template<typename Type>
	auto operator!=(const optional<Type> & opt, internal::nullopt_t) noexcept -> bool { return opt; }

	template<typename Type>
	auto operator!=(internal::nullopt_t, const optional<Type> & opt) noexcept -> bool { return opt; }

	template<typename Type>
	auto operator< (const optional<Type> & opt, internal::nullopt_t) noexcept -> bool { return false; }

	template<typename Type>
	auto operator< (internal::nullopt_t, const optional<Type> & opt) noexcept -> bool { return opt; }

	template<typename Type>
	auto operator<=(const optional<Type> & opt, internal::nullopt_t) noexcept -> bool { return !opt; }

	template<typename Type>
	auto operator<=(internal::nullopt_t, const optional<Type> & opt) noexcept -> bool { return true; }

	template<typename Type>
	auto operator> (const optional<Type> & opt, internal::nullopt_t) noexcept -> bool { return opt; }

	template<typename Type>
	auto operator> (internal::nullopt_t, const optional<Type> & opt) noexcept -> bool { return false; }

	template<typename Type>
	auto operator>=(const optional<Type> & opt, internal::nullopt_t) noexcept -> bool { return true; }

	template<typename Type>
	auto operator>=(internal::nullopt_t, const optional<Type> & opt) noexcept -> bool { return !opt; }

	template<typename Type>
	auto operator==(const optional<Type> & opt, const Type & value) -> bool { return opt ? *opt == value : false; }

	template<typename Type>
	auto operator==(const Type & value, const optional<Type> & opt) -> bool { return opt ? value == *opt : false; }

	template<typename Type>
	auto operator!=(const optional<Type> & opt, const Type & value) -> bool { return opt ? *opt != value : true; }

	template<typename Type>
	auto operator!=(const Type & value, const optional<Type> & opt) -> bool { return opt ? value != *opt : true; }

	template<typename Type>
	auto operator< (const optional<Type> & opt, const Type & value) -> bool { return opt ? *opt < value : true; }

	template<typename Type>
	auto operator< (const Type & value, const optional<Type> & opt) -> bool { return opt ? value < *opt : false; }

	template<typename Type>
	auto operator<=(const optional<Type> & opt, const Type & value) -> bool { return opt ? *opt <= value : true; }

	template<typename Type>
	auto operator<=(const Type & value, const optional<Type> & opt) -> bool { return opt ? value <= *opt : false; }

	template<typename Type>
	auto operator> (const optional<Type> & opt, const Type & value) -> bool { return opt ? *opt > value : false; }

	template<typename Type>
	auto operator> (const Type & value, const optional<Type> & opt) -> bool { return opt ? value > *opt : true; }

	template<typename Type>
	auto operator>=(const optional<Type> & opt, const Type & value) -> bool { return opt ? *opt >= value : false; }

	template<typename Type>
	auto operator>=(const Type & value, const optional<Type> & opt) -> bool { return opt ? value >= *opt : true; }

	template<typename Type>
	void swap(optional<Type> & lhs, optional<Type> & rhs) noexcept { lhs.swap(rhs); }
}