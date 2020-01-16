
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cwc/cwc.hpp>

namespace cwc::internal {
	namespace {
		std::size_t nifty_counter;
		std::aligned_storage_t<sizeof(factory_map), alignof(factory_map)> buffer;
	}

	factory_map & factories = reinterpret_cast<factory_map &>(buffer);

	factories_initializer::factories_initializer() { if(nifty_counter++ == 0) new(&buffer) factory_map; }
	factories_initializer::~factories_initializer() { if(!--nifty_counter) factories.~factory_map(); }
}

extern "C" CWC_EXPORT void CWC_CALL cwc_factory(::cwc::error_handle * cwc_error, const ::cwc::uuid * id, cwc::intrusive_ptr<cwc::component> * result) {
	assert(cwc_error);
	assert(id);
	assert(result);
	cwc_error->call_and_store([&] { *result = cwc::internal::factories.at(*id); });
}
