
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

		template<typename... Args>
		array(Args &&... args) : values{std::forward<Args>(args)...} {}

		array(const array &) =default;
		auto operator=(const array &) -> array & =default;
		~array() noexcept =default;

		CWC_CXX_RELAXED_CONSTEXPR
		auto operator[](std::size_t index)       noexcept ->       Type & { return values[index]; }
		CWC_CXX_RELAXED_CONSTEXPR
		auto operator[](std::size_t index) const noexcept -> const Type & { return values[index]; }

		CWC_CXX_RELAXED_CONSTEXPR
		auto at(std::size_t index)       ->       Type & { return validate_index(index), values[index]; }
		CWC_CXX_RELAXED_CONSTEXPR
		auto at(std::size_t index) const -> const Type & { return validate_index(index), values[index]; }

		CWC_CXX_RELAXED_CONSTEXPR
		auto data()       noexcept ->       Type * { return values; }
		CWC_CXX_RELAXED_CONSTEXPR
		auto data() const noexcept -> const Type * { return values; }

		CWC_CXX_RELAXED_CONSTEXPR
		auto size()  const noexcept -> std::size_t { return Size; }
		CWC_CXX_RELAXED_CONSTEXPR
		auto empty() const noexcept -> bool { return size() == 0; }

		CWC_CXX_RELAXED_CONSTEXPR
		auto begin()        noexcept ->       Type * { return values; }
		CWC_CXX_RELAXED_CONSTEXPR
		auto begin()  const noexcept -> const Type * { return values; }
		CWC_CXX_RELAXED_CONSTEXPR
		auto cbegin() const noexcept -> const Type * { return values; }
		CWC_CXX_RELAXED_CONSTEXPR
		auto end()          noexcept ->       Type * { return values + Size; }
		CWC_CXX_RELAXED_CONSTEXPR
		auto end()    const noexcept -> const Type * { return values + Size; }
		CWC_CXX_RELAXED_CONSTEXPR
		auto cend()   const noexcept -> const Type * { return values + Size; }

		CWC_CXX_RELAXED_CONSTEXPR
		auto rbegin()        noexcept -> std::reverse_iterator<      Type *> { return std::reverse_iterator<      Type *>(end()); }
		CWC_CXX_RELAXED_CONSTEXPR
		auto rbegin()  const noexcept -> std::reverse_iterator<const Type *> { return std::reverse_iterator<const Type *>(end()); }
		CWC_CXX_RELAXED_CONSTEXPR
		auto crbegin() const noexcept -> std::reverse_iterator<const Type *> { return std::reverse_iterator<const Type *>(cend()); }
		CWC_CXX_RELAXED_CONSTEXPR
		auto rend()          noexcept -> std::reverse_iterator<      Type *> { return std::reverse_iterator<      Type *>(begin()); }
		CWC_CXX_RELAXED_CONSTEXPR
		auto rend()    const noexcept -> std::reverse_iterator<const Type *> { return std::reverse_iterator<const Type *>(begin()); }
		CWC_CXX_RELAXED_CONSTEXPR
		auto crend()   const noexcept -> std::reverse_iterator<const Type *> { return std::reverse_iterator<const Type *>(cbegin()); }

		void fill(const Type & value) { for(auto & v : values) v = value; }

		void swap(array & other) noexcept {
			using std::swap;
			for(std::size_t i{0}; i < Size; ++i)
				swap(values[i], other.values[i]);
		}

		friend
		auto operator==(const array & lhs, const array & rhs) noexcept -> bool {
			for(std::size_t i{0}; i < Size; ++i)
				if(lhs[i] != rhs[i])
					return false;
			return true;
		}

		friend
		auto operator!=(const array & lhs, const array & rhs) noexcept -> bool { return !(lhs == rhs); }

		friend
		auto operator< (const array & lhs, const array & rhs) noexcept -> bool {
			for(std::size_t i{0}; i < Size; ++i)
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
		void validate_index(std::size_t index) const { if(index >= size()) throw std::out_of_range{"index out of range"}; }

		Type values[Size];
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