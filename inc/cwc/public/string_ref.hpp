
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
		string_ref() =default;
		string_ref(const string_ref &) =default;
		string_ref(string_ref &&) noexcept =default;
		auto operator=(const string_ref &) -> string_ref & =default;
		auto operator=(string_ref &&) noexcept -> string_ref & =default;
		~string_ref() noexcept =default;
	
		//! @brief construct string_ref from c-string
		//! @param[in] ptr string to reference
		string_ref(const utf8 * ptr) noexcept : array{ptr, std::strlen(ptr)} {}

		//! @brief construct string_ref from reference to array (most likely a string literal)
		//! @param[in] ptr string to reference
		template<std::size_t Size>
		string_ref(const utf8 (&ptr)[Size]) noexcept : array{ptr, Size - 1} {}//TODO: check that ptr is null terminated!
	
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

		void swap(string_ref & other) noexcept {
			using std::swap;
			swap(array, other.array);
		}

		friend
		auto operator<<(std::ostream & os, const string_ref & self) -> std::ostream & { return os << self.c_str(); }

		friend
		auto operator< (const string_ref & lhs, const string_ref & rhs) noexcept -> bool { return std::strcmp(lhs.c_str(), rhs.c_str()) <  0; }

		friend
		auto operator<=(const string_ref & lhs, const string_ref & rhs) noexcept -> bool { return std::strcmp(lhs.c_str(), rhs.c_str()) <= 0; }

		friend
		auto operator>=(const string_ref & lhs, const string_ref & rhs) noexcept -> bool { return std::strcmp(lhs.c_str(), rhs.c_str()) >= 0; }

		friend
		auto operator> (const string_ref & lhs, const string_ref & rhs) noexcept -> bool { return std::strcmp(lhs.c_str(), rhs.c_str()) >  0; }

		friend
		auto operator==(const string_ref & lhs, const string_ref & rhs) noexcept -> bool { return std::strcmp(lhs.c_str(), rhs.c_str()) == 0; }

		friend
		auto operator!=(const string_ref & lhs, const string_ref & rhs) noexcept -> bool { return std::strcmp(lhs.c_str(), rhs.c_str()) != 0; }

		friend
		auto operator< (const utf8 * lhs, const string_ref & rhs) noexcept -> bool { return std::strcmp(lhs, rhs.c_str()) <  0; }

		friend
		auto operator<=(const utf8 * lhs, const string_ref & rhs) noexcept -> bool { return std::strcmp(lhs, rhs.c_str()) <= 0; }

		friend
		auto operator>=(const utf8 * lhs, const string_ref & rhs) noexcept -> bool { return std::strcmp(lhs, rhs.c_str()) >= 0; }

		friend
		auto operator> (const utf8 * lhs, const string_ref & rhs) noexcept -> bool { return std::strcmp(lhs, rhs.c_str()) >  0; }

		friend
		auto operator==(const utf8 * lhs, const string_ref & rhs) noexcept -> bool { return std::strcmp(lhs, rhs.c_str()) == 0; }

		friend
		auto operator!=(const utf8 * lhs, const string_ref & rhs) noexcept -> bool { return std::strcmp(lhs, rhs.c_str()) != 0; }

		friend
		auto operator< (const string_ref & lhs, const utf8 * rhs) noexcept -> bool { return std::strcmp(lhs.c_str(), rhs) <  0; }

		friend
		auto operator<=(const string_ref & lhs, const utf8 * rhs) noexcept -> bool { return std::strcmp(lhs.c_str(), rhs) <= 0; }

		friend
		auto operator>=(const string_ref & lhs, const utf8 * rhs) noexcept -> bool { return std::strcmp(lhs.c_str(), rhs) >= 0; }

		friend
		auto operator> (const string_ref & lhs, const utf8 * rhs) noexcept -> bool { return std::strcmp(lhs.c_str(), rhs) >  0; }

		friend
		auto operator==(const string_ref & lhs, const utf8 * rhs) noexcept -> bool { return std::strcmp(lhs.c_str(), rhs) == 0; }

		friend
		auto operator!=(const string_ref & lhs, const utf8 * rhs) noexcept -> bool { return std::strcmp(lhs.c_str(), rhs) != 0; }
	private:
		array_ref<const utf8> array;
	};
	CWC_PACK_END

	inline
	void swap(string_ref & lhs, string_ref & rhs) noexcept { lhs.swap(rhs); }

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