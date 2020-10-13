
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once

namespace cwc::internal {
	enum class error_selector : std::uint32_t;

	using error_callback = const void * (CWC_CALL *)(error_selector) noexcept;

	auto store_last_error() noexcept -> error_callback;

	template<typename Func>
	auto call_and_store(Func func) noexcept -> error_callback {
		try { return func(), nullptr; }
		catch(...) { return store_last_error(); }
	}
	void rethrow_last_error(error_callback callback);
}
