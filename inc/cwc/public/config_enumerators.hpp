
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once

namespace cwc {
	//! @brief entry in the configuration
	CWC_PACK_BEGIN
	struct config_entry {
		ascii_string key;//!< @brief key in configuration
		ascii_string value;//!< @brief associated value
	};
	CWC_PACK_END

	//! @brief enumerator for a section
	struct config_section_enumerator : virtual component {
		using cwc_self = config_section_enumerator;
		struct cwc_interface : component::cwc_interface {
			static auto cwc_uuid() -> uuid { return {0xFD, 0xE0, 0x42, 0xD0, 0x4, 0xC6, 0x52, 0xD6, 0x8B, 0x6E, 0x26, 0x2F, 0xA3, 0x18, 0x65, 0xF5}; }
			using cwc_wrapper = cwc_self;
			virtual internal::error_code CWC_CALL cwc$config_section_enumerator$end$0(internal::abi<const boolean>::out cwc_ret) const noexcept =0;
			virtual internal::error_code CWC_CALL cwc$config_section_enumerator$next$1() noexcept =0;
			virtual internal::error_code CWC_CALL cwc$config_section_enumerator$get$2(internal::abi<const config_entry>::out cwc_ret) const noexcept =0;
		};
		template<typename CWCImplementation, typename CWCTypeList>
		struct cwc_interface_implementation : internal::default_implementation_chaining<CWCImplementation, CWCTypeList> {
			virtual internal::error_code CWC_CALL cwc$config_section_enumerator$end$0(internal::abi<const boolean>::out cwc_ret) const noexcept override final {
				return internal::call_and_return_error([&] {
					internal::abi<const boolean>::retval cwc_tmp = static_cast<const CWCImplementation *>(this)->end();
					internal::from_abi(cwc_ret) = internal::to_abi(cwc_tmp);
				});
			}
			virtual internal::error_code CWC_CALL cwc$config_section_enumerator$next$1() noexcept override final {
				return internal::call_and_return_error([&] {
					static_cast<CWCImplementation *>(this)->next();
				});
			}
			virtual internal::error_code CWC_CALL cwc$config_section_enumerator$get$2(internal::abi<const config_entry>::out cwc_ret) const noexcept override final {
				return internal::call_and_return_error([&] {
					internal::abi<const config_entry>::retval cwc_tmp = static_cast<const CWCImplementation *>(this)->get();
					internal::from_abi(cwc_ret) = internal::to_abi(cwc_tmp);
				});
			}
		};

		//! @brief wrap an existing mutable component in the wrapper
		//! @param[in] ptr mutable component pointer
		config_section_enumerator(cwc_interface * ptr) : component{ptr} {}

		//! @brief wrap an existing const component in the wrapper
		//! @param[in] ptr mutable component pointer
		config_section_enumerator(const cwc_interface * ptr) : component{ptr} {}

		config_section_enumerator(const config_section_enumerator &) =default;
		config_section_enumerator(config_section_enumerator &&) noexcept =default;
		auto operator=(const config_section_enumerator &) -> config_section_enumerator & =default;
		auto operator=(config_section_enumerator &&) noexcept -> config_section_enumerator & =default;
		~config_section_enumerator() noexcept =default;

		//! @brief test if enumerator reached end
		//! @returns true iff end was reached
		auto end() const -> internal::abi<const boolean>::ret {
			internal::abi<const boolean>::param cwc_ret;
			internal::validate(cwc_marshall<cwc_self>()->cwc$config_section_enumerator$end$0(internal::to_abi(cwc_ret)));
			return internal::from_abi(cwc_ret);
		}

		//! @brief move to next entry in section
		void next() { internal::validate(cwc_marshall<cwc_self>()->cwc$config_section_enumerator$next$1()); }

		//! @brief retrieve current entry
		//! @returns name and value of current entry
		auto get() const -> internal::abi<const config_entry>::ret {
			internal::abi<const config_entry>::param cwc_ret;
			internal::validate(cwc_marshall<cwc_self>()->cwc$config_section_enumerator$get$2(internal::to_abi(cwc_ret)));
			return internal::from_abi(cwc_ret);
		}

	protected:
		config_section_enumerator() =default;
	};

	//! @brief enumerator for all sections
	struct config_sections_enumerator : virtual component {
		using cwc_self = config_sections_enumerator;
		struct cwc_interface : component::cwc_interface {
			static auto cwc_uuid() -> uuid { return {0x74, 0xEB, 0x4, 0x63, 0x95, 0x18, 0x56, 0x78, 0x84, 0x38, 0x9B, 0x10, 0x2B, 0x3D, 0xBD, 0xF4}; }
			using cwc_wrapper = cwc_self;
			virtual internal::error_code CWC_CALL cwc$config_sections_enumerator$end$0(internal::abi<const boolean>::out cwc_ret) const noexcept =0;
			virtual internal::error_code CWC_CALL cwc$config_sections_enumerator$next$1() noexcept =0;
			virtual internal::error_code CWC_CALL cwc$config_sections_enumerator$get$2(internal::abi<const ascii_string>::out cwc_ret) const noexcept =0;
		};
		template<typename CWCImplementation, typename CWCTypeList>
		struct cwc_interface_implementation : internal::default_implementation_chaining<CWCImplementation, CWCTypeList> {
			virtual internal::error_code CWC_CALL cwc$config_sections_enumerator$end$0(internal::abi<const boolean>::out cwc_ret) const noexcept override final {
				return internal::call_and_return_error([&] {
					internal::abi<const boolean>::retval cwc_tmp = static_cast<const CWCImplementation *>(this)->end();
					internal::from_abi(cwc_ret) = internal::to_abi(cwc_tmp);
				});
			}
			virtual internal::error_code CWC_CALL cwc$config_sections_enumerator$next$1() noexcept override final {
				return internal::call_and_return_error([&] {
					static_cast<CWCImplementation *>(this)->next();
				});
			}
			virtual internal::error_code CWC_CALL cwc$config_sections_enumerator$get$2(internal::abi<const ascii_string>::out cwc_ret) const noexcept override final {
				return internal::call_and_return_error([&] {
					internal::abi<const ascii_string>::retval cwc_tmp = static_cast<const CWCImplementation *>(this)->get();
					internal::from_abi(cwc_ret) = internal::to_abi(cwc_tmp);
				});
			}
		};

		//! @brief wrap an existing mutable component in the wrapper
		//! @param[in] ptr mutable component pointer
		config_sections_enumerator(cwc_interface * ptr) : component{ptr} {}

		//! @brief wrap an existing const component in the wrapper
		//! @param[in] ptr mutable component pointer
		config_sections_enumerator(const cwc_interface * ptr) : component{ptr} {}

		config_sections_enumerator(const config_sections_enumerator &) =default;
		config_sections_enumerator(config_sections_enumerator &&) noexcept =default;
		auto operator=(const config_sections_enumerator &) -> config_sections_enumerator & =default;
		auto operator=(config_sections_enumerator &&) noexcept -> config_sections_enumerator & =default;
		~config_sections_enumerator() noexcept =default;

		//! @brief test if enumerator reached end
		//! @returns true iff end was reached
		auto end() const -> internal::abi<const boolean>::ret {
			internal::abi<const boolean>::param cwc_ret;
			internal::validate(cwc_marshall<cwc_self>()->cwc$config_sections_enumerator$end$0(internal::to_abi(cwc_ret)));
			return internal::from_abi(cwc_ret);
		}

		//! @brief move to next entry in section
		void next() { internal::validate(cwc_marshall<cwc_self>()->cwc$config_sections_enumerator$next$1()); }

		//! @brief retrieve current section
		//! @returns name of current section
		auto get() const -> internal::abi<const ascii_string>::ret {
			internal::abi<const ascii_string>::param cwc_ret;
			internal::validate(cwc_marshall<cwc_self>()->cwc$config_sections_enumerator$get$2(internal::to_abi(cwc_ret)));
			return internal::from_abi(cwc_ret);
		}

	protected:
		config_sections_enumerator() =default;
	};
}