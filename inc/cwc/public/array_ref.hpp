
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once

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
		static_assert(std::is_standard_layout<Type>::value, "array_ref only supports standard layout types");

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
		array_ref(pointer first, pointer last) : first{first}, last{last} { if(first > last) throw std::invalid_argument{"array_ref requires [first, last)"}; }

		//! @brief construct array_ref from pointer and size
		//! @param[in] ptr start of the referenced array
		//! @param[in] count count of elements in the referenced array
		//! @attention [ptr, count) must be valid!
		array_ref(pointer ptr, size_type count) noexcept : array_ref{ptr, ptr + count} {}

		//! @brief construct array_ref from ContiguousRange
		//! @tparam ContiguousRange range type that fulfills the ContiguousRange-requirements
		//! @param[in] range range to reference
		template<typename ContiguousRange>
		array_ref(ContiguousRange & range) noexcept : array_ref{internal::data(range), internal::size(range)} {}

		template<typename ContiguousRange>
		array_ref(const ContiguousRange && range) =delete;

		template<typename OtherType>
		array_ref(const array_ref<OtherType> & other) noexcept : first{other.begin()}, last{other.end()} {}

		template<typename OtherType>
		array_ref(array_ref<OtherType> && other) noexcept : first{other.begin()}, last{other.end()} {}

		template<typename OtherType>
		auto operator=(const array_ref<OtherType> & other) noexcept -> array_ref & {
			first = other.begin();
			last = other.end();
			return *this;
		}

		template<typename OtherType>
		auto operator=(array_ref<OtherType> && other) noexcept -> array_ref & {
			first = other.begin();
			last = other.end();
			return *this;
		}

		auto operator[](size_type index)       noexcept ->       reference { assert(!empty()); return first[index]; }
		auto operator[](size_type index) const noexcept -> const_reference { assert(!empty()); return first[index]; }

		auto at(size_type index)       ->       reference { return validate_index(index), (*this)[index]; }
		auto at(size_type index) const -> const_reference { return validate_index(index), (*this)[index]; }

		auto size()  const noexcept -> size_type { return last - first; }
		auto empty() const noexcept -> bool { return size() == 0; }

		auto data()       noexcept ->       pointer { return first; }
		auto data() const noexcept -> const_pointer { return first; }

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

		void swap(array_ref & other) noexcept {
			using std::swap;
			swap(first, other.first);
			swap(last,  other.last);
		}

		friend
		auto operator==(const array_ref & lhs, const array_ref & rhs) noexcept -> bool {
			if(lhs.size() != rhs.size()) return false;
			for(size_type i{0}; i < lhs.size(); ++i)
				if(lhs[i] != rhs[i])
					return false;
			return true;
		}

		friend
		auto operator!=(const array_ref & lhs, const array_ref & rhs) noexcept -> bool { return !(lhs == rhs); }

		//TODO: add operator< <= => > ?!
	private:
		void validate_index(size_type index) const { if(index >= size()) throw std::out_of_range{"index out of range"}; }

		pointer first{nullptr}, last{nullptr};
	};
	CWC_PACK_END

	template<typename Type>
	void swap(array_ref<Type> & lhs, array_ref<Type> & rhs) noexcept { lhs.swap(rhs); }
}