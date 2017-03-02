
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once

namespace cwc {
	CWC_PACK_BEGIN
	//! @brief a readonly, non-owning reference to a null-terminated string
	struct string_view final {
		string_view() =default;
		string_view(const string_view &) =default;
		string_view(string_view &&) noexcept =default;
		auto operator=(const string_view &) -> string_view & =default;
		auto operator=(string_view &&) noexcept -> string_view & =default;
		~string_view() noexcept =default;
	
		//! @brief construct string_view from c-string
		//! @param[in] ptr string to reference
		string_view(const utf8 * ptr) noexcept : array{ptr, std::strlen(ptr)} {}

		//! @brief construct string_view from reference to array (most likely a string literal)
		//! @param[in] ptr string to reference
		template<std::size_t Size>
		string_view(const utf8 (&ptr)[Size]) noexcept : array{ptr, Size - 1} {}
	
		auto begin()  const noexcept -> const utf8 * { return array.begin(); }
		auto cbegin() const noexcept -> const utf8 * { return array.cbegin(); }
		auto end()    const noexcept -> const utf8 * { return array.end(); }
		auto cend()   const noexcept -> const utf8 * { return array.cend(); }
	
		auto rbegin()  const noexcept -> std::reverse_iterator<const utf8 *> { return array.rbegin(); }
		auto crbegin() const noexcept -> std::reverse_iterator<const utf8 *> { return array.crbegin(); }
		auto rend()    const noexcept -> std::reverse_iterator<const utf8 *> { return array.rend(); }
		auto crend()   const noexcept -> std::reverse_iterator<const utf8 *> { return array.crend(); }
	
		auto operator[](std::size_t index) const noexcept -> const utf8 & { return array[index]; }
		auto at(std::size_t index) const -> const utf8 & { return array.at(index); }
	
		auto size() const noexcept -> std::size_t { return array.size(); }
		auto empty() const noexcept -> bool { return array.empty(); }
	
		auto c_str() const noexcept -> const utf8 * { return array.data(); }

		void swap(string_view & other) noexcept {
			using std::swap;
			swap(array, other.array);
		}

		friend
		auto operator<<(std::ostream & os, const string_view & self) -> std::ostream & { return os << self.c_str(); }

		friend
		auto operator< (const string_view & lhs, const string_view & rhs) noexcept -> bool { return std::strcmp(lhs.c_str(), rhs.c_str()) <  0; }

		friend
		auto operator<=(const string_view & lhs, const string_view & rhs) noexcept -> bool { return std::strcmp(lhs.c_str(), rhs.c_str()) <= 0; }

		friend
		auto operator>=(const string_view & lhs, const string_view & rhs) noexcept -> bool { return std::strcmp(lhs.c_str(), rhs.c_str()) >= 0; }

		friend
		auto operator> (const string_view & lhs, const string_view & rhs) noexcept -> bool { return std::strcmp(lhs.c_str(), rhs.c_str()) >  0; }

		friend
		auto operator==(const string_view & lhs, const string_view & rhs) noexcept -> bool { return std::strcmp(lhs.c_str(), rhs.c_str()) == 0; }

		friend
		auto operator!=(const string_view & lhs, const string_view & rhs) noexcept -> bool { return std::strcmp(lhs.c_str(), rhs.c_str()) != 0; }

		friend
		auto operator< (const utf8 * lhs, const string_view & rhs) noexcept -> bool { return std::strcmp(lhs, rhs.c_str()) <  0; }

		friend
		auto operator<=(const utf8 * lhs, const string_view & rhs) noexcept -> bool { return std::strcmp(lhs, rhs.c_str()) <= 0; }

		friend
		auto operator>=(const utf8 * lhs, const string_view & rhs) noexcept -> bool { return std::strcmp(lhs, rhs.c_str()) >= 0; }

		friend
		auto operator> (const utf8 * lhs, const string_view & rhs) noexcept -> bool { return std::strcmp(lhs, rhs.c_str()) >  0; }

		friend
		auto operator==(const utf8 * lhs, const string_view & rhs) noexcept -> bool { return std::strcmp(lhs, rhs.c_str()) == 0; }

		friend
		auto operator!=(const utf8 * lhs, const string_view & rhs) noexcept -> bool { return std::strcmp(lhs, rhs.c_str()) != 0; }

		friend
		auto operator< (const string_view & lhs, const utf8 * rhs) noexcept -> bool { return std::strcmp(lhs.c_str(), rhs) <  0; }

		friend
		auto operator<=(const string_view & lhs, const utf8 * rhs) noexcept -> bool { return std::strcmp(lhs.c_str(), rhs) <= 0; }

		friend
		auto operator>=(const string_view & lhs, const utf8 * rhs) noexcept -> bool { return std::strcmp(lhs.c_str(), rhs) >= 0; }

		friend
		auto operator> (const string_view & lhs, const utf8 * rhs) noexcept -> bool { return std::strcmp(lhs.c_str(), rhs) >  0; }

		friend
		auto operator==(const string_view & lhs, const utf8 * rhs) noexcept -> bool { return std::strcmp(lhs.c_str(), rhs) == 0; }

		friend
		auto operator!=(const string_view & lhs, const utf8 * rhs) noexcept -> bool { return std::strcmp(lhs.c_str(), rhs) != 0; }
	private:
		array_ref<const utf8> array;
	};
	CWC_PACK_END

	inline
	void swap(string_view & lhs, string_view & rhs) noexcept { lhs.swap(rhs); }

	//! @brief helper function to test if a string_view is referencing only ASCII characters
	//! @param[in] sv string to test
	//! @returns true iff the referenced string only contains ASCII characters
	inline
	auto is_ascii(const string_view & sv) -> bool {
		for(const auto & c : sv)
			if(!is_ascii(c))
				return false;
		return true;
	}

	namespace literals {
		inline
		auto operator""_sv(const utf8 * ptr, std::size_t) noexcept -> string_view { return {ptr}; }
	}
}