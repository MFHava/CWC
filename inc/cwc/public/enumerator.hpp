
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once

namespace cwc {
	//! @brief generic enumerator to traverse containers
	//! @tparam Type element_type of the traversed container
	//! @tparam Byte0 byte no.  0 of UUID
	//! @tparam Byte1 byte no.  1 of UUID
	//! @tparam Byte2 byte no.  2 of UUID
	//! @tparam Byte3 byte no.  3 of UUID
	//! @tparam Byte4 byte no.  4 of UUID
	//! @tparam Byte5 byte no.  5 of UUID
	//! @tparam Byte6 byte no.  6 of UUID
	//! @tparam Byte7 byte no.  7 of UUID
	//! @tparam Byte8 byte no.  8 of UUID
	//! @tparam Byte9 byte no.  9 of UUID
	//! @tparam ByteA byte no. 10 of UUID
	//! @tparam ByteB byte no. 11 of UUID
	//! @tparam ByteC byte no. 12 of UUID
	//! @tparam ByteD byte no. 13 of UUID
	//! @tparam ByteE byte no. 14 of UUID
	//! @tparam ByteF byte no. 15 of UUID
	template<typename Type, uint8 Byte0, uint8 Byte1, uint8 Byte2, uint8 Byte3, uint8 Byte4, uint8 Byte5, uint8 Byte6, uint8 Byte7, uint8 Byte8, uint8 Byte9, uint8 ByteA, uint8 ByteB, uint8 ByteC, uint8 ByteD, uint8 ByteE, uint8 ByteF>
	class enumerator : public component {
		static_assert(std::is_standard_layout<Type>::value, "enumerator only supports standard layout types");
		virtual internal::error_code CWC_CALL cwc$enumerator$end$0(boolean * cwc_ret) const noexcept =0;
		virtual internal::error_code CWC_CALL cwc$enumerator$next$1() noexcept =0;
		virtual internal::error_code CWC_CALL cwc$enumerator$get$2(Type * cwc_ret) const noexcept =0;
	public:
		//! @brief test if enumerator reached end
		//! @returns true iff end was reached
		auto end() const -> boolean {
			boolean cwc_ret;
			internal::call(*this, &enumerator::cwc$enumerator$end$0, &cwc_ret);
			return cwc_ret;
		}
		
		//! @brief move to next position
		void next() {
			internal::call(*this, &enumerator::cwc$enumerator$next$1);
		}

		//! @brief retrieve element at current position
		//! @returns the currently pointed to element
		auto get() const -> Type {
			Type cwc_ret;
			internal::call(*this, &enumerator::cwc$enumerator$get$2, &cwc_ret);
			return cwc_ret;
		}

		template<typename CWCImplementation, typename CWCTypeList>
		class cwc_implementation : public internal::default_implementation_chaining<CWCImplementation, CWCTypeList> {
			internal::error_code CWC_CALL cwc$enumerator$end$0(boolean * cwc_ret) const noexcept final { return internal::call_and_return_error([&] { *cwc_ret = static_cast<const CWCImplementation &>(*this).end(); }); }
			internal::error_code CWC_CALL cwc$enumerator$next$1() noexcept final { return internal::call_and_return_error([&] { static_cast<CWCImplementation &>(*this).next(); }); }
			internal::error_code CWC_CALL cwc$enumerator$get$2(Type * cwc_ret) const noexcept final { return internal::call_and_return_error([&] { *cwc_ret = static_cast<const CWCImplementation &>(*this).get(); }); }
		};
		static auto cwc_uuid() -> uuid { return{Byte0, Byte1, Byte2, Byte3, Byte4, Byte5, Byte6, Byte7, Byte8, Byte9, ByteA, ByteB, ByteC, ByteD, ByteE, ByteF}; }
	};
#if 0//TODO
	template<typename Enumerator>
	class enumerator_iterator final : std::iterator<std::input_iterator_tag, decltype(std::declval<Enumerator>().get())> {
		intrusive_ptr<Enumerator> enumerator;
		value_type value;

		void next() {
			if(!enumerator) return;
			if(enumerator->end()) enumerator = nullptr;//reset to empty enumerator
			else value = enumerator->get();//retrieve copy of value
		}
	public:
		enumerator_iterator() =default;
		enumerator_iterator(intrusive_ptr<Enumerator> && enumerator) : enumerator{std::move(enumerator)} { next(); }

		enumerator_iterator(const enumerator_iterator &) =default;
		enumerator_iterator(enumerator_iterator &&) noexcept =default;

		auto operator=(const enumerator_iterator &) -> enumerator_iterator & =default;
		auto operator=(enumerator_iterator &&) noexcept -> enumerator_iterator & =default;

		~enumerator_iterator() =default;

		auto operator++() -> enumerator_iterator & { next(); return *this; }
		auto operator++(int) -> enumerator_iterator {
			auto dummy = *this;
			++(*this);
			return dummy;
		}

		auto operator*()  const noexcept -> const value_type & { assert(enumerator); return  value; }
		auto operator->() const noexcept -> const value_type * { assert(enumerator); return &value; }

		//! @note as modifying one copy of an input_iterator invalidates all others => this should be valid
		friend
		auto operator==(const enumerator_iterator & lhs, const enumerator_iterator & rhs) -> bool { return lhs.enumerator == rhs.enumerator; }
		friend
		auto operator!=(const enumerator_iterator & lhs, const enumerator_iterator & rhs) -> bool { return !(lhs == rhs); }
	};

	template<typename Enumerator>
	struct enumerator_range final {
		using iterator = enumerator_iterator<Enumerator>;

		enumerator_range() =default;
		enumerator_range(intrusive_ptr<Enumerator> && enumerator) : it{std::move(enumerator)} {}

		enumerator_range(const enumerator_range &) =delete;
		enumerator_range(enumerator_range &&) noexcept =delete;

		auto operator=(const enumerator_range &) -> enumerator_range & =delete;
		auto operator=(enumerator_range &&) noexcept -> enumerator_range & =delete;

		~enumerator_range() =default;

		auto begin() -> iterator {
			iterator tmp;
			using std::swap;
			swap(tmp, it);
			return tmp;
		}
		auto end() -> iterator { return{}; }
	private:
		enumerator_iterator<Enumerator> it;
	};

	template<typename Enumerator>
	auto make_enumerator_range(intrusive_ptr<Enumerator> && enumerator) -> enumerator_range<Enumerator> { return enumerator; }
#endif
}