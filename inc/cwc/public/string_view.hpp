
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
		string_view(const ascii * ptr) noexcept : array{ptr, std::strlen(ptr) + 1} {}

		//! @brief construct string_view from reference to array (most likely a string literal)
		//! @param[in] ptr string to reference
		template<std::size_t Size>
		string_view(const ascii (&ptr)[Size]) noexcept : array{ptr, Size} {}
	
		auto begin()  const noexcept -> const ascii * { return array.begin(); }
		auto cbegin() const noexcept -> const ascii * { return array.cbegin(); }
		auto end()    const noexcept -> const ascii * { return array.end(); }
		auto cend()   const noexcept -> const ascii * { return array.cend(); }
	
		auto rbegin()  const noexcept -> std::reverse_iterator<const ascii *> { return array.rbegin(); }
		auto crbegin() const noexcept -> std::reverse_iterator<const ascii *> { return array.crbegin(); }
		auto rend()    const noexcept -> std::reverse_iterator<const ascii *> { return array.rend(); }
		auto crend()   const noexcept -> std::reverse_iterator<const ascii *> { return array.crend(); }
	
		auto operator[](std::size_t index) const noexcept -> const ascii & { return array[index]; }
		auto at(std::size_t index) const -> const ascii & { return array.at(index); }
	
		auto size() const noexcept -> std::size_t { return array.size(); }
		auto empty() const noexcept -> bool { return array.empty(); }
	
		auto c_str() const noexcept -> const ascii * { return array.data(); }

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
		auto operator< (const ascii * lhs, const string_view & rhs) noexcept -> bool { return std::strcmp(lhs, rhs.c_str()) <  0; }

		friend
		auto operator<=(const ascii * lhs, const string_view & rhs) noexcept -> bool { return std::strcmp(lhs, rhs.c_str()) <= 0; }

		friend
		auto operator>=(const ascii * lhs, const string_view & rhs) noexcept -> bool { return std::strcmp(lhs, rhs.c_str()) >= 0; }

		friend
		auto operator> (const ascii * lhs, const string_view & rhs) noexcept -> bool { return std::strcmp(lhs, rhs.c_str()) >  0; }

		friend
		auto operator==(const ascii * lhs, const string_view & rhs) noexcept -> bool { return std::strcmp(lhs, rhs.c_str()) == 0; }

		friend
		auto operator!=(const ascii * lhs, const string_view & rhs) noexcept -> bool { return std::strcmp(lhs, rhs.c_str()) != 0; }

		friend
		auto operator< (const string_view & lhs, const ascii * rhs) noexcept -> bool { return std::strcmp(lhs.c_str(), rhs) <  0; }

		friend
		auto operator<=(const string_view & lhs, const ascii * rhs) noexcept -> bool { return std::strcmp(lhs.c_str(), rhs) <= 0; }

		friend
		auto operator>=(const string_view & lhs, const ascii * rhs) noexcept -> bool { return std::strcmp(lhs.c_str(), rhs) >= 0; }

		friend
		auto operator> (const string_view & lhs, const ascii * rhs) noexcept -> bool { return std::strcmp(lhs.c_str(), rhs) >  0; }

		friend
		auto operator==(const string_view & lhs, const ascii * rhs) noexcept -> bool { return std::strcmp(lhs.c_str(), rhs) == 0; }

		friend
		auto operator!=(const string_view & lhs, const ascii * rhs) noexcept -> bool { return std::strcmp(lhs.c_str(), rhs) != 0; }
	private:
		array_ref<const ascii> array;
	};
	CWC_PACK_END

	inline
	void swap(string_view & lhs, string_view & rhs) noexcept { lhs.swap(rhs); }

	namespace literals {
		inline
		auto operator""_sv(const ascii * ptr, std::size_t) noexcept -> string_view { return{ptr}; }
	}
}