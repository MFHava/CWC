
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once

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
		static_assert(std::is_standard_layout<Type>::value, "optional only supports standard layout types");

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

		auto operator->()       ->       Type * { return &**this; }
		auto operator->() const -> const Type * { return &**this; }

		auto operator*()       ->       Type & { assert(initialized); return reinterpret_cast<      Type &>(data); }
		auto operator*() const -> const Type & { assert(initialized); return reinterpret_cast<const Type &>(data); }

		explicit operator bool() const noexcept { return  initialized; }
		auto operator!() const noexcept -> bool { return !initialized; }

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

		friend
		auto operator==(const optional & lhs, const optional & rhs) -> bool {
			if(static_cast<bool>(lhs) != static_cast<bool>(rhs)) return false;
			if(static_cast<bool>(lhs) == false) return true;
			return *lhs == *rhs;
		}

		friend
		auto operator!=(const optional & lhs, const optional & rhs) -> bool {
			if(static_cast<bool>(lhs) != static_cast<bool>(rhs)) return true;
			if(static_cast<bool>(lhs) == false) return false;
			return *lhs != *rhs;
		}

		friend
		auto operator< (const optional & lhs, const optional & rhs) -> bool {
			if(static_cast<bool>(rhs) == false) return false;
			if(static_cast<bool>(lhs) == false) return true;
			return *lhs < *rhs;
		}

		friend
		auto operator<=(const optional & lhs, const optional & rhs) -> bool {
			if(static_cast<bool>(lhs) == false) return true;
			if(static_cast<bool>(rhs) == false) return false;
			return *lhs <= *rhs;
		}

		friend
		auto operator> (const optional & lhs, const optional & rhs) -> bool {
			if(static_cast<bool>(lhs) == false) return false;
			if(static_cast<bool>(rhs) == false) return true;
			return *lhs > *rhs;
		}

		friend
		auto operator>=(const optional & lhs, const optional & rhs) -> bool {
			if(static_cast<bool>(rhs) == false) return true;
			if(static_cast<bool>(lhs) == false) return false;
			return *lhs >= *rhs;
		}

		friend
		auto operator==(const optional & opt, internal::nullopt_t) noexcept -> bool { return !opt; }

		friend
		auto operator==(internal::nullopt_t, const optional & opt) noexcept -> bool { return !opt; }

		friend
		auto operator!=(const optional & opt, internal::nullopt_t) noexcept -> bool { return opt; }

		friend
		auto operator!=(internal::nullopt_t, const optional & opt) noexcept -> bool { return opt; }

		friend
		auto operator< (const optional & opt, internal::nullopt_t) noexcept -> bool { return false; }

		friend
		auto operator< (internal::nullopt_t, const optional & opt) noexcept -> bool { return opt; }

		friend
		auto operator<=(const optional & opt, internal::nullopt_t) noexcept -> bool { return !opt; }

		friend
		auto operator<=(internal::nullopt_t, const optional & opt) noexcept -> bool { return true; }

		friend
		auto operator> (const optional & opt, internal::nullopt_t) noexcept -> bool { return opt; }

		friend
		auto operator> (internal::nullopt_t, const optional & opt) noexcept -> bool { return false; }

		friend
		auto operator>=(const optional & opt, internal::nullopt_t) noexcept -> bool { return true; }

		friend
		auto operator>=(internal::nullopt_t, const optional & opt) noexcept -> bool { return !opt; }

		friend
		auto operator==(const optional & opt, const Type & value) -> bool { return opt ? *opt == value : false; }

		friend
		auto operator==(const Type & value, const optional & opt) -> bool { return opt ? value == *opt : false; }

		friend
		auto operator!=(const optional & opt, const Type & value) -> bool { return opt ? *opt != value : true; }

		friend
		auto operator!=(const Type & value, const optional & opt) -> bool { return opt ? value != *opt : true; }

		friend
		auto operator< (const optional & opt, const Type & value) -> bool { return opt ? *opt < value : true; }

		friend
		auto operator< (const Type & value, const optional & opt) -> bool { return opt ? value < *opt : false; }

		friend
		auto operator<=(const optional & opt, const Type & value) -> bool { return opt ? *opt <= value : true; }

		friend
		auto operator<=(const Type & value, const optional & opt) -> bool { return opt ? value <= *opt : false; }

		friend
		auto operator> (const optional & opt, const Type & value) -> bool { return opt ? *opt > value : false; }

		friend
		auto operator> (const Type & value, const optional & opt) -> bool { return opt ? value > *opt : true; }

		friend
		auto operator>=(const optional & opt, const Type & value) -> bool { return opt ? *opt >= value : false; }

		friend
		auto operator>=(const Type & value, const optional & opt) -> bool { return opt ? value >= *opt : true; }
	private:
		uint8 data[sizeof(Type)];
		boolean initialized{false};
	};
	CWC_PACK_END

	template<typename Type>
	void swap(optional<Type> & lhs, optional<Type> & rhs) noexcept { lhs.swap(rhs); }
}