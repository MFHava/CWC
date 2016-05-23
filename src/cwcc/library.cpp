
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#define NOSERVICE
	#define NOMCX
	#define NOIME
	#ifndef NOMINMAX
		#define NOMINMAX
	#endif
	#include <Windows.h>
#else
	#include <dlfcn.h>
	#define HMODULE void *
	#define LoadLibrary(file) dlopen(file, RTLD_NOW)
	#define GetProcAddress(dll, function) dlsym(dll, function)
	#define FreeLibrary(dll) dlclose(dll)
#endif

#include <sstream>
#include <cwc/cwc.hpp>
#include <cwc/internal/metadata_enumerator.hpp>
#include "library.hpp"

namespace cwcc {
	namespace {
		using metadata_t = cwc::internal::error_code(CWC_CALL *)(cwc::internal::metadata_enumerator::cwc_interface **);
		using definition_t = cwc::ascii_string(CWC_CALL *)();

		const auto dummy = [] {
			cwc::this_context::init(cwc::this_context::init_mode::string, "");//empty init
			return 0;
		}();
	}

	library::library(const std::string & name) {
		struct guard {
			HMODULE handle;

			guard(const std::string & name) : handle{LoadLibrary(name.c_str())} {
				if(!handle) throw std::logic_error{"could not load bundle '" + name + "'"};
			}

			~guard() { FreeLibrary(handle); }
		} instance{name};
		const auto init = reinterpret_cast<void(CWC_CALL *)(cwc::internal::context_interface *)>(GetProcAddress(instance.handle, "cwc_init"));
		if(!init) throw std::logic_error{"could not find entry point 'cwc_init' in bundle \"" + name + '"'};
		init(&cwc::internal::this_context());
		
		const auto definitionPtr = reinterpret_cast<definition_t>(GetProcAddress(instance.handle, "cwc_reflect"));
		if(!definitionPtr) throw std::logic_error{"could not find entry point 'cwc_reflect' in bundle \"" + name + '"'};
		definition_ = definitionPtr();

		const auto metadataPtr = reinterpret_cast<metadata_t>(GetProcAddress(instance.handle, "cwc_metadata"));
		if(!metadataPtr) throw std::logic_error{"could not find entry point 'cwc_metadata' in bundle \"" + name + '"'};

		cwc::internal::metadata_enumerator::cwc_interface * enumerator;
		cwc::internal::validate(metadataPtr(&enumerator));
		if(!enumerator) throw std::runtime_error{"could not retrieve bundle's metadata"};
		std::stringstream ss;
		auto first = true;
		for(auto e = cwc::internal::from_abi(enumerator); !e.end(); e.next()) {
			const auto entry = e.get();
			if(first) first = false;
			else ss << ",\n";
			ss << "{\"" << entry.key << "\", \"" << entry.value << "\"}";
		}
		metadata_ = ss.str();
	}
}