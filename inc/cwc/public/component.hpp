
//          Copyright Michael Florian Hava.
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
		void CWC_CALL cwc$component$dynamic_cast$2(error_handle * cwc_error, const uuid * id, void ** result) const noexcept =0;
		virtual
		void CWC_CALL cwc$component$dynamic_cast_fast$3(error_handle * cwc_error, const uuid * id, void ** result) const noexcept =0;
	public:
	};

	namespace internal {
		template<>
		struct interface_id<component> final : uuid_constant<0x45, 0x91, 0x4, 0xE, 0xEF, 0xBF, 0x5E, 0x84, 0xA1, 0x58, 0x53, 0x3E, 0xD4, 0xAC, 0xFF, 0x93> {};

		template<typename Implementation, typename TypeList>
		class vtable_implementation<component, Implementation, TypeList> : public default_implementation_chaining<Implementation, TypeList> {
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
			void CWC_CALL cwc$component$dynamic_cast$2(error_handle * cwc_error, const uuid * id, void ** result) const noexcept final { return cwc_error->call_and_store([&] { internal::cast_to_interface<true>(this, *id, result); }); }
			void CWC_CALL cwc$component$dynamic_cast_fast$3(error_handle * cwc_error, const uuid * id, void ** result) const noexcept final { return cwc_error->call_and_store([&] { internal::cast_to_interface<false>(this, *id, result); }); }
		public:
			vtable_implementation() =default;
			~vtable_implementation() =default;
		};
	}
}
