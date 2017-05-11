
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include <regex>
#include <limits>
#include <climits>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include "cwc/cwc.hpp"
#include "cwc/internal/error_handling.hpp"

/*!
	@page context_init Context Initialization
	In order to use CWC-based components the CWC host (in most cases an executable that uses components) must initialize the CWC context.
	The context is statically initialized by including <cwc/host.hpp>. Note that this header may only be included once by the CWC host and may never be included by a CWC component.

	Several aspects of the context initialization are configurable:
	| Flag | Description | Default |
	| --- | --- | --- |
	| @c CWC_CONTEXT_INIT_STRING | source of the context initialization | @c "cwc.ini" <br/> (iff @c CWC_CONTEXT_INIT_IS_NOT_FILE is not defined) |
	| @c CWC_CONTEXT_INIT_IS_NOT_FILE | controls how @c CWC_CONTEXT_INIT_STRING is interpreted <br/> if not defined: @c CWC_CONTEXT_INIT_STRING is name of an INI-file <br/> if defined: @c CWC_CONTEXT_INIT_STRING is string with complete configuration in INI-format | not defined |
	| @c CWC_CONTEXT_MAX_EXCEPTION_MESSAGE_LENGTH | maximum length of exceptions messages - longer exception messages will be truncated when transfered by CWC | @c 256 |

	@attention No static object may depend on the CWC context as this may lead to Static-Initialization-Order-Fiasco!
*/

#if !defined(CWC_CONTEXT_MAX_EXCEPTION_MESSAGE_LENGTH)
	#define CWC_CONTEXT_MAX_EXCEPTION_MESSAGE_LENGTH 256
#endif

#if !defined(CWC_CONTEXT_INIT_IS_NOT_FILE) && !defined(CWC_CONTEXT_INIT_STRING)
	#define CWC_CONTEXT_INIT_STRING "cwc.ini"
#endif

#if !defined(CWC_CONTEXT_INIT_STRING)
	#error CWC_CONTEXT_INIT_STRING must be set!
#endif

#if CWC_OS_WINDOWS
	#define WIN32_LEAN_AND_MEAN
	#define NOSERVICE
	#define NOMCX
	#define NOIME
	#ifndef NOMINMAX
		#define NOMINMAX
	#endif
	#include <Windows.h>
#elif CWC_OS_LINUX
	#include <dlfcn.h>
	#define HMODULE void *
	#define LoadLibrary(file) dlopen(file, RTLD_NOW)
	#define GetProcAddress(dll, function) dlsym(dll, function)
#else
	#error unknown operating system
#endif

namespace {
	//validate that platform conforms to basic CWC ABI requirements
	template<typename FloatingPoint, size_t ExpectedSize>
	struct validate_floating_point final {
		enum {
			value = sizeof(FloatingPoint) == ExpectedSize
			     && std::numeric_limits<FloatingPoint>::is_specialized
			     && std::numeric_limits<FloatingPoint>::is_iec559
			     && std::is_floating_point<FloatingPoint>::value
		};
	};
	static_assert(CHAR_BIT == 8, "unsupported character type (not 8 bits)");
	static_assert(validate_floating_point<cwc::float32, 4>::value, "unsupported single precision floating point");
	static_assert(validate_floating_point<cwc::float64, 8>::value, "unsupported double precision floating point");

	thread_local cwc::utf8 last_message[CWC_CONTEXT_MAX_EXCEPTION_MESSAGE_LENGTH]{0};

	class context_impl final : public cwc::interface_implementation<cwc::context, context_impl> {
		using factory_export_type = cwc::internal::error_code(CWC_CALL *)(const cwc::string_ref *, cwc::intrusive_ptr<cwc::component> *);

		using factory_map = std::unordered_map<std::string, cwc::intrusive_ptr<cwc::component>>;
		using key_value_map = std::unordered_map<std::string, std::string>;
		using config_map = std::unordered_map<std::string, key_value_map>;
		using dll_map = std::unordered_map<HMODULE, factory_export_type>;

		factory_map component_factories;
		std::unordered_map<std::string, factory_map> plugin_factories;
		const config_map configuration;

		auto load_dll(dll_map & map, const std::string & file) -> HMODULE {
			const auto handle = LoadLibrary(file.c_str());
			if(handle && !map.count(handle)) { 
				if(const auto init = reinterpret_cast<void (CWC_CALL *)(cwc::intrusive_ptr<context>)>(GetProcAddress(handle, "cwc_init"))) {
					if(const auto factory = reinterpret_cast<factory_export_type>(GetProcAddress(handle, "cwc_factory"))) {
						init(intrusive_from_this<context>());
						map[handle] = factory;
					} else throw std::logic_error{"could not find entry point 'cwc_factory' in bundle \"" + file + '"'};
				} else throw std::logic_error{"could not find entry point 'cwc_init' in bundle \"" + file + '"'};
			}
			return handle;
		}

		auto create_factory(factory_export_type factory, const std::string & file, const std::string & fqn) -> cwc::intrusive_ptr<cwc::component> {
			cwc::intrusive_ptr<cwc::component> ptr;
			const cwc::string_ref tmp{fqn.c_str()};
			if(factory(&tmp, &ptr) != cwc::internal::error_code::no_error) throw std::logic_error{"could not retrieve factory for component \"" + fqn + "\" from bundle \"" + file + '"'};
			if(!ptr) throw std::logic_error{"did not receive valid factory for component \"" + fqn + "\" from bundle \"" + file + '"'};
			return ptr;
		}

		auto parse_ini(std::istream & is) -> config_map {
			config_map result;
			if(is) {
				const std::regex comment_or_whitespace{R"(\s*(?:[;#].*)?)"},
				                 section{R"(\s*\[([^\s;#]+)\]\s*(?:[;#].*)?)"},
				                 key_value_pair{R"(\s*([^\s;#]+)\s*=\s*([^\s;#]+)\s*(?:[;#].*)?)"};
				std::string line, current_section;
				while(std::getline(is, line)) {
					std::smatch matched;
					if(std::regex_match(line, matched, comment_or_whitespace)) {} //nothing to do here
					else if(std::regex_match(line, matched, key_value_pair)) result[current_section][std::move(matched[1])] = std::move(matched[2]);
					else if(std::regex_match(line, matched, section)) current_section = std::move(matched[1]);
					else throw std::logic_error{"invalid configuration file"};
				}
			}
			return result;
		}

		void load_component(dll_map & map, const std::string & fqn, const std::string & file) {
			if(const auto library = load_dll(map, file)) component_factories.emplace(fqn, create_factory(map.at(library), file, fqn));
			else throw std::logic_error{"could not load bundle \"" + file + '"'};
		}

		void load_plugins(dll_map & map, const std::string & fqn, const key_value_map & mapping) {
			for(const auto & m : mapping)
				if(const auto library = load_dll(map, m.second))
					plugin_factories[fqn].emplace(m.first, create_factory(map.at(library), m.second, fqn));
		}

		class config_section_enumerator : public cwc::interface_implementation<cwc::config_section_enumerator, config_section_enumerator> {
			key_value_map::const_iterator it, last;
		public:
			config_section_enumerator(const key_value_map & config) : it{std::begin(config)}, last{std::end(config)} {}

			auto end() const -> bool { return it == last; }

			void next() {
				if(end()) throw std::runtime_error{"already at end"};
				++it;
			}

			auto get() const -> cwc::config_entry {
				if(end()) throw std::runtime_error{"at end"};
				return {it->first.c_str(), it->second.c_str()};
			}
		};

		class config_sections_enumerator : public cwc::interface_implementation<cwc::config_sections_enumerator, config_sections_enumerator> {
			config_map::const_iterator it, last;
		public:
			config_sections_enumerator(const config_map & config) : it{std::begin(config)}, last{std::end(config)} {}

			auto end() const -> bool { return it == last; }

			void next() {
				if(end()) throw std::runtime_error{"already at end"};
				++it;
			}

			auto get() const -> cwc::config_section {
				if(end()) throw std::runtime_error{"at end"};
				return {it->first.c_str(), cwc::make_intrusive<config_section_enumerator>(it->second)};
			}
		};

	public:
		context_impl(std::istream & is) : configuration{parse_ini(is)} {
			key_value_map components;
			config_map plugins;
			const auto it = configuration.find("cwc.mapping");
			if(it != std::end(configuration)) {
				for(const auto & mapping : it->second) {
					if(mapping.second.front() != '[') components[mapping.first] = mapping.second;
					else {//key maps to section
						if(mapping.second.back() != ']') throw std::invalid_argument{"malformed configuration"};
						const auto & it = configuration.find(mapping.second.substr(1, mapping.second.size() - 2));
						if(it != std::end(configuration))
							for(const auto & entry: it->second)
								plugins[mapping.first].insert(entry);
					}
				}
			}

			dll_map map;//ensure that cwc_init is only called once
			for(const auto & c : components) load_component(map, c.first, c.second);
			for(const auto & p : plugins) load_plugins(map, p.first, p.second);
		}

		auto version() const noexcept -> cwc::string_ref { return CWC_VERSION; }

		void error(cwc::string_ref msg) const noexcept {
			last_message[0] = '\0';
			std::strncat(last_message, msg.c_str(), sizeof(last_message) - 1);
		}

		auto error() const noexcept -> cwc::string_ref { return last_message; }

		auto config() const -> cwc::intrusive_ptr<cwc::config_sections_enumerator> { return cwc::make_intrusive<config_sections_enumerator>(configuration); }

		auto factory(const cwc::string_ref & fqn, const cwc::optional<const cwc::string_ref> & id) const -> cwc::intrusive_ptr<cwc::component> { return id ? plugin_factories.at(fqn.c_str()).at(id->c_str()) : component_factories.at(fqn.c_str()); }
	};

	const cwc::intrusive_ptr<cwc::context> instance = [] {
#ifdef CWC_CONTEXT_INIT_IS_NOT_FILE
		std::istringstream is{CWC_CONTEXT_INIT_STRING};
		return cwc::make_intrusive<context_impl>(is);
#else
		std::ifstream is{CWC_CONTEXT_INIT_STRING};
		if(!is) throw std::invalid_argument{"could not open file \"" + std::string{CWC_CONTEXT_INIT_STRING} +"\" for context initialization"};
		return cwc::make_intrusive<context_impl>(is);
#endif
	}();
}

namespace cwc {
	auto this_context() -> intrusive_ptr<context> { return instance; }
}