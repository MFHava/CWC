
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once
#include <atomic>

namespace cwc {
	//! @brief base for all interfaces, every component can be cast to this interface
	class component {
		template<typename Type>
		friend
		struct handle;

		virtual
		void CWC_CALL cwc$component$new$0() const noexcept =0;
		virtual
		void CWC_CALL cwc$component$delete$1() const noexcept =0;
		virtual
		void CWC_CALL cwc$component$dynamic_cast$2(const internal::uuid * id, void ** result) const noexcept =0;
		virtual
		void CWC_CALL cwc$component$dynamic_cast_fast$3(const internal::uuid * id, void ** result) const noexcept =0;
	protected:
		~component() noexcept =default;
	public:
	};

	namespace internal {
		template<>
		inline
		constexpr
		auto interface_id<component>{make_uuid<0x45, 0x91, 0x4, 0xE, 0xEF, 0xBF, 0x5E, 0x84, 0xA1, 0x58, 0x53, 0x3E, 0xD4, 0xAC, 0xFF, 0x93>};

		template<typename Implementation, typename TypeList>
		class vtable_wrapper<component, Implementation, TypeList> : public default_implementation_chaining<Implementation, TypeList> {
			mutable std::atomic<uint64> ref_count{1};

			vtable_wrapper(const vtable_wrapper &) =delete;
			vtable_wrapper(vtable_wrapper &&) =delete;

			auto operator=(const vtable_wrapper &) -> vtable_wrapper & =delete;
			auto operator=(vtable_wrapper &&) -> vtable_wrapper & =delete;

			void CWC_CALL cwc$component$new$0() const noexcept final { ++ref_count; }
			CWC_PRAGMA_WARNING_PUSH
			CWC_PRAGMA_WARNING_NON_VIRTUAL_DTOR
			void CWC_CALL cwc$component$delete$1() const noexcept final { if(!--ref_count) delete static_cast<const Implementation *>(this); }
			CWC_PRAGMA_WARNING_POP
			void CWC_CALL cwc$component$dynamic_cast$2(const internal::uuid * id, void ** result) const noexcept final { cast_to_interface<true, typename Implementation::cwc_interfaces>(*id, result); }
			void CWC_CALL cwc$component$dynamic_cast_fast$3(const internal::uuid * id, void ** result) const noexcept final { cast_to_interface<false, typename Implementation::cwc_interfaces>(*id, result); }

			template<bool IncRefCount, typename TL>
			void cast_to_interface(const internal::uuid & id, void ** result) const noexcept {
				if constexpr(TL::empty) {
					(void)id;
					*result = nullptr;
				} else {
					using Head = typename TL::template at<0>;
					if(id == interface_id<Head>) {
						using IdentityType = typename Implementation::cwc_interfaces::template at<1>; //does not work for classes that implement no additional interfaces...
						static_assert(!std::is_same_v<IdentityType, component>);

						using Cast = std::conditional_t<std::is_same_v<Head, component>, IdentityType, Head>;
						auto ptr{static_cast<Cast *>(const_cast<Implementation *>(static_cast<const Implementation *>(this)))};
						if constexpr(IncRefCount) ++ref_count;
						*result = ptr;
					} else {
						using Tail = typename TL::template erase_at<0>;
						cast_to_interface<IncRefCount, Tail>(id, result);
					}
				}
			}
		public:
			vtable_wrapper() =default;
			~vtable_wrapper() =default;
		};
	}
}
