
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once

namespace cwc {
	namespace internal {
		CWC_PACK_BEGIN
		struct metadata_entry {
			ascii_string key;
			ascii_string value;
		};
		CWC_PACK_END

		struct metadata_enumerator : virtual component {
			using cwc_self = metadata_enumerator;
			struct cwc_interface : component::cwc_interface {
				static auto cwc_uuid() -> uuid { return {0xD1, 0x77, 0x3, 0x30, 0x42, 0x57, 0x53, 0xB, 0x97, 0xB4, 0xC3, 0x4F, 0x1D, 0x4D, 0x42, 0xE1}; }
				using cwc_wrapper = cwc_self;
				virtual error_code CWC_CALL cwc$internal$metadata_enumerator$end$0(abi<const boolean>::out cwc_ret) const =0;
				virtual error_code CWC_CALL cwc$internal$metadata_enumerator$next$1() =0;
				virtual error_code CWC_CALL cwc$internal$metadata_enumerator$get$2(abi<const metadata_entry>::out cwc_ret) const =0;
			};
			template<typename CWCImplementation, typename CWCTypeList>
			struct cwc_interface_implementation : default_implementation_chaining<CWCImplementation, CWCTypeList> {
				virtual error_code CWC_CALL cwc$internal$metadata_enumerator$end$0(abi<const boolean>::out cwc_ret) const override final {
					return call_and_return_error([&] {
						abi<const boolean>::retval cwc_tmp = static_cast<const CWCImplementation *>(this)->end();
						from_abi(cwc_ret) = to_abi(cwc_tmp);
					});
				}
				virtual error_code CWC_CALL cwc$internal$metadata_enumerator$next$1() override final {
					return call_and_return_error([&] {
						static_cast<CWCImplementation *>(this)->next();
					});
				}
				virtual error_code CWC_CALL cwc$internal$metadata_enumerator$get$2(abi<const metadata_entry>::out cwc_ret) const override final {
					return call_and_return_error([&] {
						abi<const metadata_entry>::retval cwc_tmp = static_cast<const CWCImplementation *>(this)->get();
						from_abi(cwc_ret) = to_abi(cwc_tmp);
					});
				}
			};

			metadata_enumerator(cwc_interface * ptr);
			metadata_enumerator(const cwc_interface * ptr);

			metadata_enumerator(const metadata_enumerator &) =default;
			auto operator=(const metadata_enumerator &) -> metadata_enumerator & =default;
			CWC_MOVE_SEMANTICS_NOT_IMPLICIT_EMULATION(metadata_enumerator)

			auto end() const -> abi<const boolean>::ret;

			void next();

			auto get() const -> abi<const metadata_entry>::ret;

		protected:
			metadata_enumerator();
		};
	}
}