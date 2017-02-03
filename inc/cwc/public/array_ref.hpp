
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
		array_ref(Type * first, Type * last) : first{first}, last{last} { if(first > last) throw std::invalid_argument{"array_ref requires [first, last)"}; }

		//! @brief construct array_ref from pointer and size
		//! @param[in] ptr start of the referenced array
		//! @param[in] count count of elements in the referenced array
		//! @attention [ptr, count) must be valid!
		array_ref(Type * ptr, std::size_t count) noexcept : array_ref{ptr, ptr + count} {}

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

		auto data()       noexcept ->       Type * { return first; }
		auto data() const noexcept -> const Type * { return first; }

		auto size()  const noexcept -> std::size_t { return last - first; }
		auto empty() const noexcept -> bool { return size() == 0; }

		auto operator[](std::size_t index)       noexcept ->       Type & { return first[index]; }
		auto operator[](std::size_t index) const noexcept -> const Type & { return first[index]; }

		auto at(std::size_t index)       ->       Type & { return validate_index(index), (*this)[index]; }
		auto at(std::size_t index) const -> const Type & { return validate_index(index), (*this)[index]; }

		auto begin()        noexcept ->       Type * { return first; }
		auto begin()  const noexcept -> const Type * { return first; }
		auto cbegin() const noexcept -> const Type * { return first; }
		auto end()          noexcept ->       Type * { return last; }
		auto end()    const noexcept -> const Type * { return last; }
		auto cend()   const noexcept -> const Type * { return last; }

		auto rbegin()        noexcept -> std::reverse_iterator<      Type *> { return std::reverse_iterator<      Type *>(end()); }
		auto rbegin()  const noexcept -> std::reverse_iterator<const Type *> { return std::reverse_iterator<const Type *>(end()); }
		auto crbegin() const noexcept -> std::reverse_iterator<const Type *> { return std::reverse_iterator<const Type *>(cend()); }
		auto rend()          noexcept -> std::reverse_iterator<      Type *> { return std::reverse_iterator<      Type *>(begin()); }
		auto rend()    const noexcept -> std::reverse_iterator<const Type *> { return std::reverse_iterator<const Type *>(begin()); }
		auto crend()   const noexcept -> std::reverse_iterator<const Type *> { return std::reverse_iterator<const Type *>(cbegin()); }

		void swap(array_ref & other) noexcept {
			using std::swap;
			swap(first, other.first);
			swap(last,  other.last);
		}

		//TODO: switch to value comparison?! (=> generate comparison operators for all structs automagically?!)
		friend
		auto operator==(const array_ref & lhs, const array_ref & rhs) noexcept -> bool { return (lhs.begin() == rhs.begin()) && (lhs.end() == rhs.end()); }

		friend
		auto operator!=(const array_ref & lhs, const array_ref & rhs) noexcept -> bool { return !(lhs == rhs); }
	private:
		void validate_index(std::size_t index) const { if(index >= size()) throw std::out_of_range{"index out of range"}; }

		Type * first{nullptr}, * last{nullptr};
	};
	CWC_PACK_END

	template<typename Type>
	void swap(array_ref<Type> & lhs, array_ref<Type> & rhs) noexcept { lhs.swap(rhs); }
}