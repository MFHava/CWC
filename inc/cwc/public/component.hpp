
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once

namespace cwc {
	//! @brief base for all interfaces, every component can be cast to this interface
	class component {
		template<typename Type>
		friend
		struct intrusive_ptr;

		template<typename Self, typename TypeList, bool IncRefCount>
		friend
		class internal::cast_to_interface_helper;

		virtual
		void CWC_CALL cwc$component$new$0() const noexcept =0;
		virtual
		void CWC_CALL cwc$component$delete$1() const noexcept =0;
		virtual
		auto CWC_CALL cwc$component$dynamic_cast$2(const uuid * id, void ** result) const noexcept -> const internal::error * =0;
		virtual
		auto CWC_CALL cwc$component$dynamic_cast_fast$3(const uuid * id, void ** result) const noexcept -> const internal::error * =0;
	public:
	};

	namespace internal {
		template<>
		struct interface_id<component> final : uuid_constant<0x45, 0x91, 0x4, 0xE, 0xEF, 0xBF, 0x5E, 0x84, 0xA1, 0x58, 0x53, 0x3E, 0xD4, 0xAC, 0xFF, 0x93> {};

		template<typename Implementation, typename TypeList>
		class vtable_implementation<component, Implementation, TypeList> : public default_implementation_chaining<Implementation, TypeList> {
			template<typename Type>
			friend
			struct intrusive_ptr;

			mutable std::atomic<uint64> cwc_reference_counter{1};

			vtable_implementation(const vtable_implementation &) =delete;
			vtable_implementation(vtable_implementation &&) =delete;

			auto operator=(const vtable_implementation &) -> vtable_implementation & =delete;
			auto operator=(vtable_implementation &&) -> vtable_implementation & =delete;

			void CWC_CALL cwc$component$new$0() const noexcept final { ++cwc_reference_counter; }
			CWC_PRAGMA_WARNING_PUSH
			CWC_PRAGMA_WARNING_NON_VIRTUAL_DTOR
			void CWC_CALL cwc$component$delete$1() const noexcept final { if(!--cwc_reference_counter) delete static_cast<const Implementation *>(this); }
			CWC_PRAGMA_WARNING_POP
			auto CWC_CALL cwc$component$dynamic_cast$2(const uuid * id, void ** result) const noexcept -> const internal::error * final { return call_and_return_error([&] { internal::cast_to_interface<>(this, *id, result); }); }
			auto CWC_CALL cwc$component$dynamic_cast_fast$3(const uuid * id, void ** result) const noexcept -> const internal::error * final { return call_and_return_error([&] { internal::cast_to_interface<false>(this, *id, result); }); }
		protected:
			//! @brief retrieve intrusive_ptr for requested interface
			//! @tparam TargetType type to cast new pointer to
			//! @returns intrusive_ptr iff cast is valid
			template<typename TargetType>
			auto intrusive_from_this()       -> intrusive_ptr<      TargetType> {
				static_assert(std::is_base_of_v<TargetType, Implementation>, "invalid cast");
				std::remove_const_t<TargetType> * result;//remove constness of TargetType to allow conversion from 'mutable Type' to 'const TargetType'
				internal::cast_to_interface<>(this, interface_id<TargetType>::get(), reinterpret_cast<void **>(&result));
				return intrusive_ptr<      TargetType>{result};
			}

			//! @brief retrieve intrusive_ptr for requested interface
			//! @tparam TargetType type to cast new pointer to
			//! @returns intrusive_ptr iff cast is valid
			template<typename TargetType>
			auto intrusive_from_this() const -> intrusive_ptr<const TargetType> {
				static_assert(std::is_base_of_v<TargetType, Implementation>, "invalid cast");
				const TargetType * result;
				internal::cast_to_interface<>(this, interface_id<TargetType>::get(), reinterpret_cast<void **>(&result));
				return intrusive_ptr<const TargetType>{result};
			}
		public:
			vtable_implementation() =default;
			~vtable_implementation() =default;
		};
	}
}
