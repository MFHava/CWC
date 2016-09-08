
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once

namespace cwc {
	namespace internal {
		struct context_interface : component::cwc_interface {
			static auto cwc_uuid() -> uuid { return {0xFE, 0xFF, 0xDB, 0x25, 0xC3, 0xC0, 0x5E, 0xD1, 0x9C, 0x2E, 0xDB, 0x38, 0xB5, 0x3B, 0x69, 0x73}; }
			using cwc_wrapper = void;
			virtual error_code CWC_CALL cwc$internal$context$version$0(abi<const version>::out version) const =0;
			virtual error_code CWC_CALL cwc$internal$context$create$1(abi<const ascii_string>::in fqn, abi<component>::out result) const =0;
			virtual error_code CWC_CALL cwc$internal$context$create$2(abi<const ascii_string>::in fqn, abi<const plugin_id>::in id, abi<component>::out result) const =0;
			virtual error_code CWC_CALL cwc$internal$context$error$3(abi<const ascii_string>::in msg) const =0;
			virtual error_code CWC_CALL cwc$internal$context$error$4(abi<const ascii_string>::out msg) const =0;
			virtual error_code CWC_CALL cwc$internal$context$config$5(abi<config_sections_enumerator>::out enumerator) const =0;
			virtual error_code CWC_CALL cwc$internal$context$config$6(abi<const ascii_string>::in section, abi<config_section_enumerator>::out enumerator) const =0;
			virtual error_code CWC_CALL cwc$internal$context$config$7(abi<const ascii_string>::in section, abi<const ascii_string>::in key, abi<const ascii_string>::out value) const =0;
		};

		auto this_context() -> context_interface &;

		template<typename Component, typename... Params>
		auto create_component(Params &&... params) -> component::cwc_interface * {
			component::cwc_interface * ptr{nullptr};
			validate(this_context().cwc$internal$context$create$1(Component::cwc_component::cwc_fqn(), &ptr));
			reference_guard tmp{ptr};
			using Factory = typename Component::cwc_factory;
			using FactoryInterface = typename Factory::cwc_interface;
			FactoryInterface * factory_ptr{nullptr};
			validate(ptr->cwc$component$as$2(FactoryInterface::cwc_uuid(), reinterpret_cast<void**>(&factory_ptr)));
			Factory factory{factory_ptr};
			auto result = factory(std::forward<Params>(params)...);
			return to_abi(result);
		}

		template<typename Component, typename... Params>
		auto create_component(const plugin_id & id, Params &&... params) -> component::cwc_interface * {
			component::cwc_interface * ptr{nullptr};
			validate(this_context().cwc$internal$context$create$2(Component::cwc_component::cwc_fqn(), id, &ptr));
			reference_guard tmp{ptr};
			using Factory = typename Component::cwc_factory;
			using FactoryInterface = typename Factory::cwc_interface;
			FactoryInterface * factory_ptr{nullptr};
			validate(ptr->cwc$component$as$2(FactoryInterface::cwc_uuid(), reinterpret_cast<void**>(&factory_ptr)));
			Factory factory{factory_ptr};
			auto result = factory(std::forward<Params>(params)...);
			return to_abi(result);
		}
	}
}