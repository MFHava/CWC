
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cassert>
#include <cwc/cwc.hpp>

namespace cwc::internal {
	namespace {
		std::size_t nifty_counter;
		std::aligned_storage_t<sizeof(factory_map), alignof(factory_map)> buffer;
	}

	factory_map & factories = reinterpret_cast<factory_map &>(buffer);

	factories_initializer::factories_initializer() { if(!nifty_counter++) new(&buffer) factory_map; }

	factories_initializer::~factories_initializer() noexcept { if(!--nifty_counter) factories.~factory_map(); }

	void factory_map::add(const uuid & id, function_pointer func) {
		assert(func);
		if(!mapping.emplace(id, func).second) throw std::logic_error{"detected duplicated export of component"};
	}

	auto factory_map::get(const uuid & id) const -> function_pointer { return mapping.at(id); }
}

extern "C" CWC_EXPORT void CWC_CALL cwc_main(::cwc::error_context * error, const ::cwc::internal::uuid * id, cwc::handle<cwc::component> * result) noexcept {
	assert(error);
	assert(id);
	assert(result);
	error->call_and_store([&] { *result = cwc::internal::factories.get(*id)(); });
}
