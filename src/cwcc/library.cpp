
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#define CWC_CONTEXT_INIT_STRING ""
#define CWC_CONTEXT_INIT_IS_NOT_FILE
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
		} instance{name};
		const auto init{reinterpret_cast<void(CWC_CALL *)(cwc::intrusive_ptr<cwc::context>)>(GetProcAddress(instance.handle, "cwc_init"))};
		if(!init) throw std::logic_error{"could not find entry point 'cwc_init' in bundle \"" + name + '"'};
		init(cwc::this_context());
		
		const auto definitionPtr{reinterpret_cast<void(CWC_CALL *)(cwc::string_ref *)>(GetProcAddress(instance.handle, "cwc_reflect"))};
		if(!definitionPtr) throw std::logic_error{"could not find entry point 'cwc_reflect' in bundle \"" + name + '"'};

		const auto exportsPtr{reinterpret_cast<void(CWC_CALL *)(::cwc::array_ref<const ::cwc::string_ref> *)>(GetProcAddress(instance.handle, "cwc_exports"))};
		if(!exportsPtr) throw std::logic_error{"could not find entry point 'cwc_exports' in bundle \"" + name + '"'};

		cwc::string_ref definition;
		definitionPtr(&definition);
		std::copy(std::begin(definition), std::end(definition), std::back_inserter(definition_));

		cwc::array_ref<const cwc::string_ref> exports;
		exportsPtr(&exports);
		std::copy(std::begin(exports), std::end(exports), std::back_inserter(exports_));
	}
}
