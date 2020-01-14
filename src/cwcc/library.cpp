
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cwc/host.hpp>
#include <sstream>
#include "library.hpp"

namespace cwcc {
	library::library(const std::string & name) {
		struct guard {
			HMODULE handle;

			guard(const std::string & name) : handle{LoadLibrary(make_path(name).c_str())} {
				if(!handle) throw std::logic_error{"could not load bundle '" + name + "'"};
			}

			~guard() { FreeLibrary(handle); }
		} guard{name};

		const auto definitionPtr{reinterpret_cast<void(CWC_CALL *)(cwc::string_ref *)>(GetProcAddress(guard.handle, "cwc_reflect"))};
		if(!definitionPtr) throw std::logic_error{"could not find entry point 'cwc_reflect' in bundle \"" + name + '"'};

		const auto exportsPtr{reinterpret_cast<void(CWC_CALL *)(::cwc::array_ref<const ::cwc::string_ref> *)>(GetProcAddress(guard.handle, "cwc_exports"))};
		if(!exportsPtr) throw std::logic_error{"could not find entry point 'cwc_exports' in bundle \"" + name + '"'};

		cwc::string_ref definition;
		definitionPtr(&definition);
		std::copy(std::begin(definition), std::end(definition), std::back_inserter(definition_));

		cwc::array_ref<const cwc::string_ref> exports;
		exportsPtr(&exports);
		std::transform(std::begin(exports), std::end(exports), std::back_inserter(exports_), [](auto sr) -> std::string { return {sr.begin(), sr.end()}; });
	}
}
