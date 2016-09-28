
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once
#include <iterator>

namespace cwc {
	//! @brief non-owning reference to array
	//! @tparam Type type of the referenced array
	CWC_PACK_BEGIN
	template<typename Type>
	struct array_ref final {
		static_assert(!internal::is_component<Type>::value && !internal::is_interface<Type>::value, "array_ref only supports simple types");

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

		//! @brief construct array_ref from RandomAccessIterators
		//! @tparam RandomAccessIterator iterator type that fulfills the RandomAccessIterator-requirements
		//! @param[in] first begin of range
		//! @param[in] last end of range
		//! @throws invalid_argument iff the given range is not [first, last)
		template<typename RandomAccessIterator>
		array_ref(RandomAccessIterator first, RandomAccessIterator last) {
			using Trait = std::iterator_traits<RandomAccessIterator>;
			static_assert(std::is_base_of<std::random_access_iterator_tag, typename Trait::iterator_category>::value, "array_ref only supports random access iterators");
			using Input = typename std::remove_pointer<typename Trait::pointer>::type;
			static_assert(!(std::is_const<Input>::value && !std::is_const<value_type>::value), "array_ref<T> can't be constructed with array_ref<const T>");

			const auto size = std::distance(first, last);
			if(size < 0) throw std::invalid_argument{"array_ref requires [first, last)"};
			else if(size) {
				ptr = &*first;
				count = size;
			}
		}

		//! @brief construct array_ref from RandomAccessRange
		//! @tparam RandomAccessRange range type that fulfills the RandomAccessRange-requirements
		//! @param[in] range range to reference
		//! @attention as array_ref is non-owning: never pass a temporary as it will result in a dangling reference
		template<typename RandomAccessRange>
		array_ref(RandomAccessRange & range) : array_ref{std::begin(range), std::end(range)} {}//TODO: how to prevent temporaries?!

		auto data()       noexcept ->       pointer { return ptr; }
		auto data() const noexcept -> const_pointer { return ptr; }

		auto size() const noexcept -> size_type { return count; }
		auto empty() const noexcept -> bool { return count == 0; }

		auto front()       noexcept ->       reference { return *begin(); }
		auto front() const noexcept -> const_reference { return *begin(); }
		auto back()        noexcept ->       iterator  { return *--end(); }
		auto back() const  noexcept -> const_iterator  { return *--end(); }

		auto operator[](size_type index)       noexcept ->       reference { return ptr[index]; }
		auto operator[](size_type index) const noexcept -> const_reference { return ptr[index]; }

		auto at(size_type index)       ->       reference { return validate_index(index), (*this)[index]; }
		auto at(size_type index) const -> const_reference { return validate_index(index), (*this)[index]; }

		auto begin()        noexcept ->       iterator { return ptr; }
		auto begin()  const noexcept -> const_iterator { return ptr; }
		auto cbegin() const noexcept -> const_iterator { return ptr; }
		auto end()          noexcept ->       iterator { return ptr + count; }
		auto end()    const noexcept -> const_iterator { return ptr + count; }
		auto cend()   const noexcept -> const_iterator { return ptr + count; }

		auto rbegin()        noexcept ->       reverse_iterator { return begin(); }
		auto rbegin()  const noexcept -> const_reverse_iterator { return begin(); }
		auto crbegin() const noexcept -> const_reverse_iterator { return cbegin(); }
		auto rend()          noexcept ->       reverse_iterator { return end(); }
		auto rend()    const noexcept -> const_reverse_iterator { return end(); }
		auto crend()   const noexcept -> const_reverse_iterator { return cend(); }
	private:
		void validate_index(size_type index) const { if(index >= size()) throw std::out_of_range{"index out of range"}; }

		Type * ptr{nullptr};
		uint64 count{0};
	};
	CWC_PACK_END
}