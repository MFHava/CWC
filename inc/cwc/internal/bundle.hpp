
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include <map>
#include "../cwc.hpp"
#include "error_handling.hpp"

namespace cwc {
	namespace internal {
		auto registered_factories() noexcept -> std::map<::cwc::string_ref, ::cwc::intrusive_ptr<cwc::component>> & {
			static std::map<::cwc::string_ref, ::cwc::intrusive_ptr<cwc::component>> factories;
			return factories;
		}
	}
}

namespace {
	void validate_factories() {
		for(const auto & pair : cwc::internal::registered_factories())
			if(!pair.second)
				throw std::logic_error{"detected unregistered exported component"};

		//TODO: validate that all required factories are present
	}

	::cwc::intrusive_ptr<::cwc::context> cwc_context;
}

auto ::cwc::this_context() -> ::cwc::intrusive_ptr<context> { return cwc_context; }

extern "C" CWC_EXPORT void CWC_CALL cwc_init(::cwc::intrusive_ptr<::cwc::context> context) {
	validate_factories();
	assert(!cwc_context);
	cwc_context = context;
}

extern "C" CWC_EXPORT const ::cwc::internal::error * CWC_CALL cwc_factory(const ::cwc::string_ref * fqn, cwc::intrusive_ptr<cwc::component> * result) {
	assert(cwc_context);
	assert(result);
	return ::cwc::internal::call_and_return_error([&] {
		const auto & factories{cwc::internal::registered_factories()};
		const auto it{factories.find(*fqn)};
		if(it == std::end(factories)) throw std::logic_error{"unsupported component"};
		*result = it->second;
	});
}

extern "C" CWC_EXPORT void CWC_CALL cwc_reflect(::cwc::string_ref * definition) { *definition = cwc_definition; }
