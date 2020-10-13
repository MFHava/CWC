
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


	auto factory_map::find(const uuid & id) const noexcept -> std::vector<std::pair<uuid, function_pointer>>::const_iterator {
		return std::lower_bound(begin(mapping), end(mapping), id, [](const auto & lhs, const auto & rhs) { return lhs.first < rhs; });
	}

	void factory_map::add(const uuid & id, function_pointer func) {
		if(const auto it{find(id)}; it == cend(mapping) || it->first != id) mapping.emplace(it, id, func);
		else throw std::logic_error{"detected duplicated export of component"};
	}

	auto factory_map::get(const uuid & id) const -> function_pointer {
		if(const auto it{find(id)}; it != end(mapping) && it->first == id) return it->second;
		throw std::invalid_argument{"requested un-exported component"};
	}
}

extern "C" CWC_EXPORT ::cwc::internal::error_callback CWC_CALL cwc_main(const ::cwc::internal::uuid * id, cwc::handle<cwc::component> * result) noexcept {
	assert(id);
	assert(result);
	return cwc::internal::call_and_store([&] { *result = cwc::internal::factories.get(*id)(); });
}
