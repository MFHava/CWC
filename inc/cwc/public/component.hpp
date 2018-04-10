
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

		template<typename Self, typename TypeList>
		friend
		class internal::cast_to_interface;

		virtual void                    CWC_CALL cwc$component$new$0() const noexcept =0;
		virtual void                    CWC_CALL cwc$component$delete$1() const noexcept =0;
		virtual const internal::error * CWC_CALL cwc$component$cast$2(const uuid * id, void ** result) const noexcept =0;
	public:
		constexpr
		static auto cwc_uuid() -> uuid { return {static_cast<uint8>(0x45), static_cast<uint8>(0x91), static_cast<uint8>(0x4), static_cast<uint8>(0xE), static_cast<uint8>(0xEF), static_cast<uint8>(0xBF), static_cast<uint8>(0x5E), static_cast<uint8>(0x84), static_cast<uint8>(0xA1), static_cast<uint8>(0x58), static_cast<uint8>(0x53), static_cast<uint8>(0x3E), static_cast<uint8>(0xD4), static_cast<uint8>(0xAC), static_cast<uint8>(0xFF), static_cast<uint8>(0x93)}; }
	};

	namespace internal {
		template<typename Implementation, typename TypeList>
		class vtable_implementation<component, Implementation, TypeList> : public internal::default_implementation_chaining<Implementation, TypeList> {
			template<typename Type>
			friend
			struct intrusive_ptr;

			mutable std::atomic<uint64> cwc_reference_counter{1};

			vtable_implementation(const vtable_implementation &) =delete;
			vtable_implementation(vtable_implementation &&) =delete;

			auto operator=(const vtable_implementation &) -> vtable_implementation & =delete;
			auto operator=(vtable_implementation &&) -> vtable_implementation & =delete;

			void                    CWC_CALL cwc$component$new$0() const noexcept final { ++cwc_reference_counter; }
			void                    CWC_CALL cwc$component$delete$1() const noexcept final { if(!--cwc_reference_counter) delete static_cast<const Implementation *>(this); }
			const internal::error * CWC_CALL cwc$component$cast$2(const uuid * id, void ** result) const noexcept final { return internal::call_and_return_error([&] { internal::cast_to_interface<Implementation, typename Implementation::cwc_interfaces>::cast(const_cast<Implementation *>(static_cast<const Implementation *>(this)), *id, result); }); }
		protected:
			//! @brief retrieve intrusive_ptr for requested interface
			//! @tparam TargetType type to cast new pointer to
			//! @returns intrusive_ptr iff cast is valid
			template<typename TargetType>
			auto intrusive_from_this()       -> intrusive_ptr<      TargetType> {
				static_assert(std::is_base_of<TargetType, Implementation>::value, "invalid cast");
				typename std::remove_const<TargetType>::type * result;//remove constness of TargetType to allow conversion from 'mutable Type' to 'const TargetType'
				internal::cast_to_interface<Implementation, typename Implementation::cwc_interfaces>::cast(static_cast<Implementation *>(this), TargetType::cwc_uuid(), reinterpret_cast<void **>(&result));
				return intrusive_ptr<      TargetType>{result};
			}

			//! @brief retrieve intrusive_ptr for requested interface
			//! @tparam TargetType type to cast new pointer to
			//! @returns intrusive_ptr iff cast is valid
			template<typename TargetType>
			auto intrusive_from_this() const -> intrusive_ptr<const TargetType> {
				static_assert(std::is_base_of<TargetType, Implementation>::value, "invalid cast");
				const TargetType * result;
				internal::cast_to_interface<Implementation, typename Implementation::cwc_interfaces>::cast(static_cast<Implementation *>(this), TargetType::cwc_uuid(), reinterpret_cast<void **>(&result));
				return intrusive_ptr<const TargetType>{result};
			}
		public:
			vtable_implementation() =default;
			~vtable_implementation() =default;
		};
	}
}
