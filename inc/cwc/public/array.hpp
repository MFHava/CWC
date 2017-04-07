
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
	//! @brief a fixed-size array
	//! @tparam Type type of the stored array
	//! @tparam Size size of the stored array
	template<typename Type, std::size_t Size>
	struct array final {
		static_assert(std::is_standard_layout<Type>::value, "array only supports standard layout types");

		using value_type             = Type;
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

		template<typename... Args>
		array(Args &&... args) : values{std::forward<Args>(args)...} {}

		array(const array &) =default;
		auto operator=(const array &) -> array & =default;
		~array() noexcept =default;

		CWC_CXX_RELAXED_CONSTEXPR
		auto operator[](size_type index)       noexcept ->       reference { assert(Size); return values[index]; }
		CWC_CXX_RELAXED_CONSTEXPR
		auto operator[](size_type index) const noexcept -> const_reference { assert(Size); return values[index]; }

		CWC_CXX_RELAXED_CONSTEXPR
		auto at(size_type index)       ->       reference { return validate_index(index), (*this)[index]; }
		CWC_CXX_RELAXED_CONSTEXPR
		auto at(size_type index) const -> const_reference { return validate_index(index), (*this)[index]; }

		CWC_CXX_RELAXED_CONSTEXPR
		auto size()  const noexcept -> size_type { return Size; }
		CWC_CXX_RELAXED_CONSTEXPR
		auto empty() const noexcept -> bool { return size() == 0; }

		CWC_CXX_RELAXED_CONSTEXPR
		auto data()       noexcept ->       pointer { return values; }
		CWC_CXX_RELAXED_CONSTEXPR
		auto data() const noexcept -> const_pointer { return values; }

		CWC_CXX_RELAXED_CONSTEXPR
		auto begin()        noexcept ->       iterator { return values; }
		CWC_CXX_RELAXED_CONSTEXPR
		auto begin()  const noexcept -> const_iterator { return values; }
		CWC_CXX_RELAXED_CONSTEXPR
		auto cbegin() const noexcept -> const_iterator { return values; }
		CWC_CXX_RELAXED_CONSTEXPR
		auto end()          noexcept ->       iterator { return values + Size; }
		CWC_CXX_RELAXED_CONSTEXPR
		auto end()    const noexcept -> const_iterator { return values + Size; }
		CWC_CXX_RELAXED_CONSTEXPR
		auto cend()   const noexcept -> const_iterator { return values + Size; }

		CWC_CXX_RELAXED_CONSTEXPR
		auto rbegin()        noexcept ->       reverse_iterator { return reverse_iterator{end()}; }
		CWC_CXX_RELAXED_CONSTEXPR
		auto rbegin()  const noexcept -> const_reverse_iterator { return const_reverse_iterator{end()}; }
		CWC_CXX_RELAXED_CONSTEXPR
		auto crbegin() const noexcept -> const_reverse_iterator { return const_reverse_iterator{cend()}; }
		CWC_CXX_RELAXED_CONSTEXPR
		auto rend()          noexcept ->       reverse_iterator { return reverse_iterator{begin()}; }
		CWC_CXX_RELAXED_CONSTEXPR
		auto rend()    const noexcept -> const_reverse_iterator { return const_reverse_iterator{begin()}; }
		CWC_CXX_RELAXED_CONSTEXPR
		auto crend()   const noexcept -> const_reverse_iterator { return const_reverse_iterator{cbegin()}; }

		void fill(const_reference value) { for(auto & v : values) v = value; }

		void swap(array & other) noexcept {
			using std::swap;
			for(size_type i{0}; i < Size; ++i)
				swap(values[i], other.values[i]);
		}

		friend
		auto operator==(const array & lhs, const array & rhs) noexcept -> bool {
			for(size_type i{0}; i < Size; ++i)
				if(lhs[i] != rhs[i])
					return false;
			return true;
		}

		friend
		auto operator!=(const array & lhs, const array & rhs) noexcept -> bool { return !(lhs == rhs); }

		friend
		auto operator< (const array & lhs, const array & rhs) noexcept -> bool {
			for(size_type i{0}; i < Size; ++i)
				if(!(lhs[i] < rhs[i]))
					return false;
			return true;
		}

		friend
		auto operator> (const array & lhs, const array & rhs) noexcept -> bool { return rhs < lhs; }

		friend
		auto operator<=(const array & lhs, const array & rhs) noexcept -> bool { return !(rhs < lhs); }

		friend
		auto operator>=(const array & lhs, const array & rhs) noexcept -> bool { return !(lhs < rhs); }
	private:
		CWC_CXX_RELAXED_CONSTEXPR
		void validate_index(size_type index) const { if(index >= size()) throw std::out_of_range{"index out of range"}; }

		value_type values[Size];
	};
	CWC_PACK_END

	template<typename Type, std::size_t Size>
	void swap(array<Type, Size> & lhs, array<Type, Size> & rhs) noexcept { lhs.swap(rhs); }

	template<typename Type, std::size_t Size, std::size_t Index>
	CWC_CXX_RELAXED_CONSTEXPR
	auto get(      array<Type, Size> & self) noexcept ->       Type & {
		static_assert(Index < Size, "index out of range");
		return self[Index];
	}

	template<typename Type, std::size_t Size, std::size_t Index>
	CWC_CXX_RELAXED_CONSTEXPR
	auto get(const array<Type, Size> & self) noexcept -> const Type & {
		static_assert(Index < Size, "index out of range");
		return self[Index];
	}
}