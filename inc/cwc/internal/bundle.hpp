
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once
#include "error_handling.hpp"

static ::cwc::intrusive_ptr<::cwc::context> cwc_context;

auto ::cwc::this_context() -> ::cwc::intrusive_ptr<context> { return cwc_context; }

extern "C" CWC_EXPORT void CWC_CALL cwc_init(::cwc::intrusive_ptr<::cwc::context> context) {
	assert(!cwc_context);
	cwc_context = context;
}

extern "C" CWC_EXPORT const ::cwc::internal::error * CWC_CALL cwc_factory(const ::cwc::string_ref * fqn, cwc::intrusive_ptr<cwc::component> * result) {
	assert(cwc_context);
	assert(result);
	return ::cwc::internal::call_and_return_error([&] {
		const auto it = cwc_factories.find(*fqn);
		if(it == std::end(cwc_factories)) throw std::logic_error{"unsupported component"};
		*result = it->second;
	});
}

extern "C" CWC_EXPORT void CWC_CALL cwc_reflect(::cwc::string_ref * definition) { *definition = cwc_definition; }