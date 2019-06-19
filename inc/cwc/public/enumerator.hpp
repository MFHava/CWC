
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
		static_assert(internal::is_abi_compatible_v<Type>);
		virtual
		auto CWC_CALL cwc$enumerator$end$0(boolean * cwc_ret) const noexcept -> const internal::error * =0;
		virtual
		auto CWC_CALL cwc$enumerator$next$1() noexcept -> const internal::error * =0;
		virtual
		auto CWC_CALL cwc$enumerator$get$2(Type * cwc_ret) const noexcept -> const internal::error * =0;
	public:
		struct cwc_iterator final {
			using iterator_category = std::input_iterator_tag;
			using value_type        = Type;
			using difference_type   = std::ptrdiff_t;
			using pointer           = const value_type *;
			using reference         = const value_type &;

			cwc_iterator() =default;
			cwc_iterator(intrusive_ptr<enumerator> && enum_) : enum_{std::move(enum_)} { if(this->enum_) next(); }

			cwc_iterator(const cwc_iterator &) =default;
			cwc_iterator(cwc_iterator &&) noexcept =default;

			auto operator=(const cwc_iterator &) -> cwc_iterator & =default;
			auto operator=(cwc_iterator &&) noexcept -> cwc_iterator & =default;

			~cwc_iterator() =default;

			auto operator++() -> cwc_iterator & { next(); return *this; }
			auto operator++(int) -> cwc_iterator {
				auto dummy = *this;
				++(*this);
				return dummy;
			}

			auto operator*()  const noexcept -> reference { assert(enum_); return  value; }
			auto operator->() const noexcept -> pointer   { assert(enum_); return &value; }

			//! @note as modifying one copy of an input_iterator invalidates all others => this should be valid
			friend
			auto operator==(const cwc_iterator & lhs, const cwc_iterator & rhs) -> bool { return lhs.enum_ == rhs.enum_; }
			friend
			auto operator!=(const cwc_iterator & lhs, const cwc_iterator & rhs) -> bool { return !(lhs == rhs); }

		private:
			intrusive_ptr<enumerator> enum_;
			value_type value;

			void next() {
				assert(enum_);
				if(enum_->end()) enum_.reset();
				else {
					value = enum_->get();//retrieve copy of value
					enum_->next();
				}
			}
		};

		//! @brief test if enumerator reached end
		//! @returns true iff end was reached
		auto end() const -> boolean {
			boolean cwc_ret;
			internal::call(*this, &enumerator::cwc$enumerator$end$0, cwc_ret);
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
			internal::call(*this, &enumerator::cwc$enumerator$get$2, cwc_ret);
			return cwc_ret;
		}
	};

	namespace internal {
		template<typename Type, uint8 Byte0, uint8 Byte1, uint8 Byte2, uint8 Byte3, uint8 Byte4, uint8 Byte5, uint8 Byte6, uint8 Byte7, uint8 Byte8, uint8 Byte9, uint8 ByteA, uint8 ByteB, uint8 ByteC, uint8 ByteD, uint8 ByteE, uint8 ByteF>
		struct interface_id<enumerator<Type, Byte0, Byte1, Byte2, Byte3, Byte4, Byte5, Byte6, Byte7, Byte8, Byte9, ByteA, ByteB, ByteC, ByteD, ByteE, ByteF>> final : uuid_constant<Byte0, Byte1, Byte2, Byte3, Byte4, Byte5, Byte6, Byte7, Byte8, Byte9, ByteA, ByteB, ByteC, ByteD, ByteE, ByteF> {};

		template<typename Type, uint8 Byte0, uint8 Byte1, uint8 Byte2, uint8 Byte3, uint8 Byte4, uint8 Byte5, uint8 Byte6, uint8 Byte7, uint8 Byte8, uint8 Byte9, uint8 ByteA, uint8 ByteB, uint8 ByteC, uint8 ByteD, uint8 ByteE, uint8 ByteF, typename Implementation, typename TypeList>
		class vtable_implementation<enumerator<Type, Byte0, Byte1, Byte2, Byte3, Byte4, Byte5, Byte6, Byte7, Byte8, Byte9, ByteA, ByteB, ByteC, ByteD, ByteE, ByteF>, Implementation, TypeList> : public default_implementation_chaining<Implementation, TypeList> {
			auto CWC_CALL cwc$enumerator$end$0(boolean * cwc_ret) const noexcept -> const error * final { return call_and_return_error([&] { *cwc_ret = static_cast<const Implementation &>(*this).end(); }); }
			auto CWC_CALL cwc$enumerator$next$1() noexcept -> const error * final { return call_and_return_error([&] { static_cast<Implementation &>(*this).next(); }); }
			auto CWC_CALL cwc$enumerator$get$2(Type * cwc_ret) const noexcept -> const error * final { return call_and_return_error([&] { *cwc_ret = static_cast<const Implementation &>(*this).get(); }); }
			//detect missing methods:
			void end();
			void next();
			void get();
		};
	}

	template<typename Type, uint8 Byte0, uint8 Byte1, uint8 Byte2, uint8 Byte3, uint8 Byte4, uint8 Byte5, uint8 Byte6, uint8 Byte7, uint8 Byte8, uint8 Byte9, uint8 ByteA, uint8 ByteB, uint8 ByteC, uint8 ByteD, uint8 ByteE, uint8 ByteF>
	auto begin(intrusive_ptr<enumerator<Type, Byte0, Byte1, Byte2, Byte3, Byte4, Byte5, Byte6, Byte7, Byte8, Byte9, ByteA, ByteB, ByteC, ByteD, ByteE, ByteF>> enum_) -> typename enumerator<Type, Byte0, Byte1, Byte2, Byte3, Byte4, Byte5, Byte6, Byte7, Byte8, Byte9, ByteA, ByteB, ByteC, ByteD, ByteE, ByteF>::cwc_iterator { return {std::move(enum_)}; }

	template<typename Type, uint8 Byte0, uint8 Byte1, uint8 Byte2, uint8 Byte3, uint8 Byte4, uint8 Byte5, uint8 Byte6, uint8 Byte7, uint8 Byte8, uint8 Byte9, uint8 ByteA, uint8 ByteB, uint8 ByteC, uint8 ByteD, uint8 ByteE, uint8 ByteF>
	auto end(const intrusive_ptr<enumerator<Type, Byte0, Byte1, Byte2, Byte3, Byte4, Byte5, Byte6, Byte7, Byte8, Byte9, ByteA, ByteB, ByteC, ByteD, ByteE, ByteF>> &) -> typename enumerator<Type, Byte0, Byte1, Byte2, Byte3, Byte4, Byte5, Byte6, Byte7, Byte8, Byte9, ByteA, ByteB, ByteC, ByteD, ByteE, ByteF>::cwc_iterator { return {}; }
}
