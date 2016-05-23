
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cwc/cwc.hpp>
#include <cwc/internal/metadata_enumerator.hpp>

namespace cwc {
	namespace internal {
		metadata_enumerator::metadata_enumerator(cwc_interface * ptr) : component{ptr} {}

		metadata_enumerator::metadata_enumerator(const cwc_interface * ptr) : component{ptr} {}

		auto metadata_enumerator::end() const -> abi<const boolean>::ret {
			abi<const boolean>::param cwc_ret;
			validate(cwc_marshall<cwc_self>()->cwc$internal$metadata_enumerator$end$0(to_abi(cwc_ret)));
			return from_abi(cwc_ret);
		}

		void metadata_enumerator::next() { validate(cwc_marshall<cwc_self>()->cwc$internal$metadata_enumerator$next$1()); }

		auto metadata_enumerator::get() const -> abi<const metadata_entry>::ret {
			abi<const metadata_entry>::param cwc_ret;
			validate(cwc_marshall<cwc_self>()->cwc$internal$metadata_enumerator$get$2(to_abi(cwc_ret)));
			return from_abi(cwc_ret);
		}

		metadata_enumerator::metadata_enumerator() {}
	}
}