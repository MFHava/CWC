
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
		struct internal::cast_to_interface;

		virtual void                 CWC_CALL cwc$component$new$0() const noexcept =0;
		virtual void                 CWC_CALL cwc$component$delete$1() const noexcept =0;
		virtual internal::error_code CWC_CALL cwc$component$cast$2(uuid id, void ** result) noexcept =0;
		virtual internal::error_code CWC_CALL cwc$component$cast$3(uuid id, const void ** result) const noexcept =0;
	public:
		//! @brief retrieve intrusive_ptr for requested interface
		//! @tparam TargetType type to cast new pointer to
		//! @returns intrusive_ptr iff cast is valid
		template<typename TargetType>
		auto intrusive_from_this() -> intrusive_ptr<TargetType> {
			typename std::remove_const<TargetType>::type * tmp;//remove constness of TargetType to allow conversion from 'mutable Type' to 'const TargetType'
			internal::call(*this, &component::cwc$component$cast$2, TargetType::cwc_uuid(), reinterpret_cast<void **>(&tmp));
			return intrusive_ptr<TargetType>{tmp};
		}

		//! @brief retrieve intrusive_ptr for requested interface
		//! @tparam TargetType type to cast new pointer to
		//! @returns intrusive_ptr iff cast is valid
		template<typename TargetType>
		auto intrusive_from_this() const -> intrusive_ptr<const TargetType> {
			const TargetType * tmp;
			internal::call(*this, &component::cwc$component$cast$3, TargetType::cwc_uuid(), reinterpret_cast<const void **>(&tmp));
			return intrusive_ptr<const TargetType>{tmp};
		}

		template<typename Implementation, typename TypeList>
		class cwc_implementation : public internal::default_implementation_chaining<Implementation, TypeList>, internal::instance_counter {
			template<typename Type>
			friend
			struct intrusive_ptr;

			mutable std::atomic<uint64> cwc_reference_counter{1};

			cwc_implementation(const cwc_implementation &) =delete;
			cwc_implementation(cwc_implementation &&) =delete;

			auto operator=(const cwc_implementation &) -> cwc_implementation & =delete;
			auto operator=(cwc_implementation &&) -> cwc_implementation & =delete;

			void                 CWC_CALL cwc$component$new$0() const noexcept final { ++cwc_reference_counter; }
			void                 CWC_CALL cwc$component$delete$1() const noexcept final { if(!--cwc_reference_counter) delete static_cast<const Implementation *>(this); }
			internal::error_code CWC_CALL cwc$component$cast$2(uuid id, void ** result) noexcept final { return internal::call_and_return_error([&] { internal::cast_to_interface<Implementation, typename Implementation::cwc_interfaces>::cast(static_cast<Implementation *>(this), id, result); }); }
			internal::error_code CWC_CALL cwc$component$cast$3(uuid id, const void ** result) const noexcept final { return internal::call_and_return_error([&] { internal::cast_to_interface<const Implementation, typename Implementation::cwc_interfaces>::cast(const_cast<Implementation *>(static_cast<const Implementation *>(this)), id, result); }); }
		public:
			cwc_implementation() =default;
			~cwc_implementation() =default;
		};
		static auto cwc_uuid() -> uuid { return {0x45, 0x91, 0x4, 0xE, 0xEF, 0xBF, 0x5E, 0x84, 0xA1, 0x58, 0x53, 0x3E, 0xD4, 0xAC, 0xFF, 0x93}; }
	};
}