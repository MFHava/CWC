
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
	struct string_ref final {
		using value_type             = const utf8;
		using size_type              = std::size_t;
		using difference_type        = std::ptrdiff_t;
		using reference              =       value_type &;
		using const_reference        = const value_type &;
		using pointer                =       value_type *;
		using const_pointer          = const value_type *;
		using iterator               =       value_type *;
		using const_iterator         = const value_type *;
		using reverse_iterator       = std::reverse_iterator<      iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

		string_ref() =default;
		string_ref(const string_ref &) =default;
		string_ref(string_ref &&) noexcept =default;
		auto operator=(const string_ref &) -> string_ref & =default;
		auto operator=(string_ref &&) noexcept -> string_ref & =default;
		~string_ref() noexcept =default;

		//! @brief construct string_ref from c-string
		//! @param[in] ptr string to reference
		string_ref(const_pointer ptr) noexcept : first{ptr}, last{ptr + std::strlen(ptr)} {}

		auto operator[](size_type index)       noexcept ->       reference { assert(!empty()); return first[index]; }
		auto operator[](size_type index) const noexcept -> const_reference { assert(!empty()); return first[index]; }

		auto at(size_type index)       ->       reference { return validate_index(index), (*this)[index]; }
		auto at(size_type index) const -> const_reference { return validate_index(index), (*this)[index]; }

		auto size()  const noexcept -> size_type { return last - first; }
		auto empty() const noexcept -> bool { return size() == 0; }

		auto data()       noexcept ->       pointer { return first; }
		auto data() const noexcept -> const_pointer { return first; }

		auto c_str()       noexcept ->       pointer { return first; }
		auto c_str() const noexcept -> const_pointer { return first; }

		auto begin()        noexcept ->       iterator { return first; }
		auto begin()  const noexcept -> const_iterator { return first; }
		auto cbegin() const noexcept -> const_iterator { return first; }
		auto end()          noexcept ->       iterator { return last; }
		auto end()    const noexcept -> const_iterator { return last; }
		auto cend()   const noexcept -> const_iterator { return last; }

		auto rbegin()        noexcept ->       reverse_iterator { return       reverse_iterator{end()}; }
		auto rbegin()  const noexcept -> const_reverse_iterator { return const_reverse_iterator{end()}; }
		auto crbegin() const noexcept -> const_reverse_iterator { return const_reverse_iterator{cend()}; }
		auto rend()          noexcept ->       reverse_iterator { return       reverse_iterator{begin()}; }
		auto rend()    const noexcept -> const_reverse_iterator { return const_reverse_iterator{begin()}; }
		auto crend()   const noexcept -> const_reverse_iterator { return const_reverse_iterator{cbegin()}; }

		friend
		void swap(string_ref & lhs, string_ref & rhs) noexcept {
			using std::swap;
			swap(lhs.first, rhs.first);
			swap(lhs.last,  rhs.last);
		}

		friend
		auto operator==(const string_ref & lhs, const string_ref & rhs) noexcept -> bool { return std::strcmp(lhs.c_str(), rhs.c_str()) == 0; }

		friend
		auto operator!=(const string_ref & lhs, const string_ref & rhs) noexcept -> bool { return std::strcmp(lhs.c_str(), rhs.c_str()) != 0; }

		friend
		auto operator< (const string_ref & lhs, const string_ref & rhs) noexcept -> bool { return std::strcmp(lhs.c_str(), rhs.c_str()) <  0; }

		friend
		auto operator<=(const string_ref & lhs, const string_ref & rhs) noexcept -> bool { return std::strcmp(lhs.c_str(), rhs.c_str()) <= 0; }

		friend
		auto operator> (const string_ref & lhs, const string_ref & rhs) noexcept -> bool { return std::strcmp(lhs.c_str(), rhs.c_str()) >  0; }

		friend
		auto operator>=(const string_ref & lhs, const string_ref & rhs) noexcept -> bool { return std::strcmp(lhs.c_str(), rhs.c_str()) >= 0; }

		friend
		auto operator==(const_pointer lhs, const string_ref & rhs) noexcept -> bool { return std::strcmp(lhs, rhs.c_str()) == 0; }

		friend
		auto operator!=(const_pointer lhs, const string_ref & rhs) noexcept -> bool { return std::strcmp(lhs, rhs.c_str()) != 0; }

		friend
		auto operator< (const_pointer lhs, const string_ref & rhs) noexcept -> bool { return std::strcmp(lhs, rhs.c_str()) <  0; }

		friend
		auto operator<=(const_pointer lhs, const string_ref & rhs) noexcept -> bool { return std::strcmp(lhs, rhs.c_str()) <= 0; }

		friend
		auto operator> (const_pointer lhs, const string_ref & rhs) noexcept -> bool { return std::strcmp(lhs, rhs.c_str()) >  0; }

		friend
		auto operator>=(const_pointer lhs, const string_ref & rhs) noexcept -> bool { return std::strcmp(lhs, rhs.c_str()) >= 0; }

		friend
		auto operator==(const string_ref & lhs, const_pointer rhs) noexcept -> bool { return std::strcmp(lhs.c_str(), rhs) == 0; }

		friend
		auto operator!=(const string_ref & lhs, const_pointer rhs) noexcept -> bool { return std::strcmp(lhs.c_str(), rhs) != 0; }

		friend
		auto operator< (const string_ref & lhs, const_pointer rhs) noexcept -> bool { return std::strcmp(lhs.c_str(), rhs) <  0; }

		friend
		auto operator<=(const string_ref & lhs, const_pointer rhs) noexcept -> bool { return std::strcmp(lhs.c_str(), rhs) <= 0; }

		friend
		auto operator> (const string_ref & lhs, const_pointer rhs) noexcept -> bool { return std::strcmp(lhs.c_str(), rhs) >  0; }

		friend
		auto operator>=(const string_ref & lhs, const_pointer rhs) noexcept -> bool { return std::strcmp(lhs.c_str(), rhs) >= 0; }

		friend
		auto operator<<(std::basic_ostream<utf8> & os, const string_ref & self) -> std::basic_ostream<utf8> & { return os << self.c_str(); }
	private:
		void validate_index(size_type index) const { if(index >= size()) throw std::out_of_range{"index out of range"}; }

		pointer first{nullptr}, last{nullptr};
	};
	CWC_PACK_END

	//! @brief helper function to test if a string_ref is referencing only ASCII characters
	//! @param[in] str string to test
	//! @returns true iff the referenced string only contains ASCII characters
	inline
	auto is_ascii(const string_ref & str) -> bool {
		for(const auto & c : str)
			if(!is_ascii(c))
				return false;
		return true;
	}

	namespace literals {
		inline
		auto operator""_sr(const utf8 * ptr, std::size_t) noexcept -> string_ref { return {ptr}; }
	}
}