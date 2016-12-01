
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once
#include <initializer_list>

namespace cwc {
	namespace internal {//emulate C++17-features
		template<typename Container>
		constexpr auto size(const Container & c) -> decltype(c.size()) { return c.size(); }

		template<typename Type, std::size_t Size>
		constexpr auto size(const Type(&array)[Size]) noexcept -> std::size_t { return Size; }

		template<typename Container>
		constexpr auto data(Container & c) -> decltype(c.data()) { return c.data(); }

		template<typename Container>
		constexpr auto data(const Container & c) -> decltype(c.data()) { return c.data(); }

		template<typename Type, std::size_t Size>
		constexpr auto data(Type(&array)[Size]) noexcept -> Type * { return array; }

		template<typename Type>
		constexpr auto data(std::initializer_list<Type> ilist) noexcept -> const Type * { return ilist.begin(); }
	}

	CWC_PACK_BEGIN
	//! @brief non-owning reference to array
	//! @tparam Type type of the referenced array
	template<typename Type>
	struct array_ref final {
		static_assert(!internal::is_component<Type>::value && !internal::is_interface<Type>::value, "array_ref only supports simple types");
		static_assert(std::is_standard_layout<Type>::value, "array_ref only supports standard layout types");

		using value_type             = Type;
		using size_type              = std::size_t;
		using difference_type        = std::ptrdiff_t;
		using reference              = value_type &;
		using const_reference        = const value_type &;
		using pointer                = Type *;
		using const_pointer          = typename std::add_const<Type>::type *;
		using iterator               = pointer;
		using const_iterator         = const_pointer;
		using reverse_iterator       = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

		array_ref() =default;
		array_ref(const array_ref & other) =default;
		array_ref(array_ref &&) noexcept =default;
		auto operator=(const array_ref &) -> array_ref & =default;
		auto operator=(array_ref &&) noexcept -> array_ref & =default;
		~array_ref() =default;

		//! @brief construct array_ref from two pointers
		//! @param[in] first start of the referenced array
		//! @param[in] last end of the referenced array
		//! @throws std::invalid_argument iff first > last
		//! @attention [first, last) must be valid!
		array_ref(Type * first, Type * last) : first{first}, last{last} {
			if(first > last) throw std::invalid_argument{"array_ref requires [first, last)"};
		}

		//! @brief construct array_ref from pointer and size
		//! @param[in] ptr start of the referenced array
		//! @param[in] count count of elements in the referenced array
		//! @attention [ptr, count) must be valid!
		array_ref(Type * ptr, std::size_t count) noexcept : array_ref{ptr, ptr + count} {}

		//! @brief construct array_ref from ContiguousRange
		//! @tparam ContiguousRange range type that fulfills the ContiguousRange-requirements
		//! @param[in] range range to reference
		//! @attention as array_ref is non-owning: never pass a temporary as it will result in a dangling reference
		template<typename ContiguousRange>
		array_ref(ContiguousRange & range) noexcept : array_ref{internal::data(range), internal::size(range)} {}

		auto data()       noexcept ->       pointer { return first; }
		auto data() const noexcept -> const_pointer { return first; }

		auto size() const noexcept -> size_type { return last - first; }
		auto empty() const noexcept -> bool { return size() == 0; }

		auto front()       noexcept ->       reference { return *begin(); }
		auto front() const noexcept -> const_reference { return *begin(); }
		auto back()        noexcept ->       iterator { return *--end(); }
		auto back() const  noexcept -> const_iterator { return *--end(); }

		auto operator[](size_type index)       noexcept ->       reference { return first[index]; }
		auto operator[](size_type index) const noexcept -> const_reference { return first[index]; }

		auto at(size_type index)       ->       reference { return validate_index(index), (*this)[index]; }
		auto at(size_type index) const -> const_reference { return validate_index(index), (*this)[index]; }

		auto begin()        noexcept ->       iterator { return first; }
		auto begin()  const noexcept -> const_iterator { return first; }
		auto cbegin() const noexcept -> const_iterator { return first; }
		auto end()          noexcept ->       iterator { return last; }
		auto end()    const noexcept -> const_iterator { return last; }
		auto cend()   const noexcept -> const_iterator { return last; }

		auto rbegin()        noexcept ->       reverse_iterator { return end(); }
		auto rbegin()  const noexcept -> const_reverse_iterator { return end(); }
		auto crbegin() const noexcept -> const_reverse_iterator { return cend(); }
		auto rend()          noexcept ->       reverse_iterator { return begin(); }
		auto rend()    const noexcept -> const_reverse_iterator { return begin(); }
		auto crend()   const noexcept -> const_reverse_iterator { return cbegin(); }

		void swap(array_ref & other) noexcept {
			using std::swap;
			swap(first, other.first);
			swap(last,  other.last);
		}
	private:
		void validate_index(size_type index) const { if(index >= size()) throw std::out_of_range{"index out of range"}; }

		pointer first{nullptr}, last{nullptr};
	};
	CWC_PACK_END

	template<typename Type>
	auto operator==(const array_ref<Type> & lhs, const array_ref<Type> & rhs) noexcept -> bool { return (lhs.begin() == rhs.begin()) && (lhs.end() == rhs.end()); }

	template<typename Type>
	auto operator!=(const array_ref<Type> & lhs, const array_ref<Type> & rhs) noexcept -> bool { return !(lhs == rhs); }
}

namespace std {
	template<typename Type>
	void swap(cwc::array_ref<Type> & lhs, cwc::array_ref<Type> & rhs) noexcept { lhs.swap(rhs); }
}