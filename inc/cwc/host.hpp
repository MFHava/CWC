
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
#include "cwc.hpp"
#include "internal/error_handling.hpp"

/*!
	@page context_init Context Initialization
	In order to use CWC-based components the CWC host (in most cases an executable that uses components) must initialize the CWC context.
	The context is statically initialized by including <cwc/host.hpp>. Note that this header may only be included once by the CWC host and may never be included by a CWC component.

	Several aspects of the context initialization are configurable:
	| Flag | Description | Default |
	| --- | --- | --- |
	| @c CWC_CONTEXT_INIT_STRING | source of the context initialization | @c "cwc.ini" <br/> (iff @c CWC_CONTEXT_INIT_IS_NOT_FILE is not defined) |
	| @c CWC_CONTEXT_INIT_IS_NOT_FILE | controls how @c CWC_CONTEXT_INIT_STRING is interpreted <br/> if not defined: @c CWC_CONTEXT_INIT_STRING is name of an INI-file <br/> if defined: @c CWC_CONTEXT_INIT_STRING is string with complete configuration in INI-format | not defined |
	| @c CWC_CONTEXT_INIT_PREFIX_DLL_PATH | override OS specific dll-path behavior and load DLLs only relative to host executable | @c true |
	| @c CWC_CONTEXT_MAX_EXCEPTION_MESSAGE_LENGTH | maximum length of exceptions messages - longer exception messages will be truncated when transfered by CWC | @c 256 |

	@attention No static object may depend on the CWC context as this may lead to Static-Initialization-Order-Fiasco!
*/

#if !defined(CWC_CONTEXT_INIT_PREFIX_DLL_PATH)
	#define CWC_CONTEXT_INIT_PREFIX_DLL_PATH true
#endif

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
	namespace {
		auto GetExecutableFileName() -> std::string {
			static_assert(sizeof(DWORD) == 4);
			for(std::string result(MAX_PATH, '\0');; result.resize(result.size() * 2)) {
				assert(result.size() <= std::numeric_limits<DWORD>::max());
				if(const auto size{GetModuleFileName(nullptr, result.data(), static_cast<DWORD>(result.size()))}; size < result.size()) {
					result.resize(size);
					return result;
				}
			}
		}
	}
	#define DLL_PREFIX ""
	#define DLL_SUFFIX ".dll"
	#define PATH_SEPARATOR '\\'
#elif CWC_OS_LINUX
	#include <dlfcn.h>
	#include <unistd.h>
	#define LoadLibrary(file) dlopen(file, RTLD_NOW)
	#define GetProcAddress(dll, function) dlsym(dll, function)
	#define FreeLibrary(dll) dlclose(dll)
	namespace {
		using HMODULE = void *;

		auto GetExecutableFileName() -> std::string {
			for(std::string result(PATH_MAX, '\0');; result.resize(result.size() * 2)) {
				if(const auto size{readlink("/proc/self/exe", result.data(), result.size())}; size < static_cast<ssize_t>(result.size())) {
					assert(size != -1);
					result.resize(size);
					return result;
				}
			}
		}
	}
	#define DLL_PREFIX "lib"
	#define DLL_SUFFIX ".so"
	#define PATH_SEPARATOR '/'
#elif CWC_OS_HAIKU
	#include <image.h>
	#define LoadLibrary(file) std::max(load_add_on(file), image_id{0})
	#define FreeLibrary(dll) unload_add_on(dll)
	namespace {
		using HMODULE = image_id;//image_id == int32 (not a pointer!)

		auto GetProcAddress(HMODULE dll, const char * function) -> void * {
			void * result{nullptr};
			get_image_symbol(dll, function, B_SYMBOL_TYPE_TEXT, &result);
			return result;
		}

		auto GetExecutableFileName() -> std::string {
			image_info info;
			info.name[0] = '\0';
			for(int32 cookie{0}; get_next_image_info(B_CURRENT_TEAM, &cookie, &info) == B_OK && info.type != B_APP_IMAGE;);
			return info.name;
		}
	}
	#define DLL_PREFIX "lib"
	#define DLL_SUFFIX ".so"
	#define PATH_SEPARATOR '/'
#else
	#error unknown operating system
#endif

namespace {
	//validate that platform conforms to basic CWC ABI requirements
	template<typename FloatingPoint, std::size_t ExpectedSize>
	struct validate_floating_point final : std::bool_constant<
		sizeof(FloatingPoint) == ExpectedSize &&
		std::numeric_limits<FloatingPoint>::is_specialized &&
		std::numeric_limits<FloatingPoint>::is_iec559 &&
		std::is_floating_point_v<FloatingPoint>
	> {};

	template<typename FloatingPoint, std::size_t ExpectedSize>
	inline
	constexpr
	auto validate_floating_point_v{validate_floating_point<FloatingPoint, ExpectedSize>::value};

	static_assert(CHAR_BIT == 8, "unsupported character type (not 8 bits)");
	static_assert(validate_floating_point_v<cwc::float32, 4>, "unsupported single precision floating point");
	static_assert(validate_floating_point_v<cwc::float64, 8>, "unsupported double precision floating point");

	thread_local
	cwc::utf8 last_message[CWC_CONTEXT_MAX_EXCEPTION_MESSAGE_LENGTH]{0};

	thread_local
	cwc::internal::error last_error{{}, last_message};

	auto make_path(std::string file) -> std::string {
		using namespace std::string_literals;
		static const auto local{"."s + PATH_SEPARATOR};
		if(std::find(std::begin(file), std::end(file), PATH_SEPARATOR) == std::end(file)) file.insert(std::begin(file), std::begin(local), std::end(local));
		return file;
	}

	class context_impl final : public cwc::context {
		using factory_export_type = const cwc::internal::error *(CWC_CALL *)(const cwc::string_ref *, cwc::intrusive_ptr<cwc::component> *);

		using factory_map = std::unordered_map<std::string, cwc::intrusive_ptr<cwc::component>>;
		using key_value_map = std::unordered_map<std::string, std::string>;
		using config_map = std::unordered_map<std::string, key_value_map>;
		using dll_map = std::unordered_map<HMODULE, factory_export_type>;

		factory_map component_factories;
		std::unordered_map<std::string, factory_map> plugin_factories;
		const config_map configuration;

		static
		auto make_name(std::string file) -> std::string {
			static const std::string prefix{DLL_PREFIX}, suffix{DLL_SUFFIX};
			static const auto base{[] {
				auto exe{GetExecutableFileName()};
				if(const auto it{std::find(std::rbegin(exe), std::rend(exe), PATH_SEPARATOR)}; it != std::rend(exe)) exe.erase(it.base(), std::end(exe));
				return exe;
			}()};
			if constexpr(CWC_CONTEXT_INIT_PREFIX_DLL_PATH) file.insert(std::begin(file), std::begin(base), std::end(base));
			file.insert(std::find(std::rbegin(file), std::rend(file), PATH_SEPARATOR).base(), std::begin(prefix), std::end(prefix));
			file.insert(std::end(file), std::begin(suffix), std::end(suffix));
			return file;
		}

		auto load_dll(dll_map & map, const std::string & file) -> HMODULE {
			const auto handle{LoadLibrary(make_path(make_name(file)).c_str())};
			if(handle) {
				if(map.count(handle)) FreeLibrary(handle);
				else if(const auto init{reinterpret_cast<void (CWC_CALL *)(const cwc::context *)>(GetProcAddress(handle, "cwc_init"))}) {
					if(const auto factory{reinterpret_cast<factory_export_type>(GetProcAddress(handle, "cwc_factory"))}) {
						init(this);
						map[handle] = factory;
					} else throw std::logic_error{"could not find entry point 'cwc_factory' in bundle \"" + file + '"'};
				} else throw std::logic_error{"could not find entry point 'cwc_init' in bundle \"" + file + '"'};
			}
			return handle;
		}

		static
		auto create_factory(factory_export_type factory, const std::string & file, const std::string & fqn) -> cwc::intrusive_ptr<cwc::component> {
			cwc::intrusive_ptr<cwc::component> ptr;
			const cwc::string_ref tmp{fqn.c_str()};
			if(factory(&tmp, &ptr) != nullptr) throw std::logic_error{"could not retrieve factory for component \"" + fqn + "\" from bundle \"" + file + '"'};
			if(!ptr) throw std::logic_error{"did not receive valid factory for component \"" + fqn + "\" from bundle \"" + file + '"'};
			return ptr;
		}

		static
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
					else if(std::regex_match(line, matched, key_value_pair)) result[current_section][matched[1]] = matched[2];
					else if(std::regex_match(line, matched, section)) current_section = matched[1];
					else throw std::logic_error{"invalid configuration file"};
				}
			}
			return result;
		}

		auto CWC_CALL cwc$context$exception$0(const cwc::internal::error * err) const noexcept -> const cwc::internal::error * final { return this->exception(err); }
		auto CWC_CALL cwc$context$factory$1(const cwc::string_ref * fqn, cwc::intrusive_ptr<cwc::component> * cwc_ret) const noexcept -> const cwc::internal::error * final { return cwc::internal::call_and_return_error([&] { *cwc_ret = this->factory(*fqn); }); }
		auto CWC_CALL cwc$context$factory$2(const cwc::string_ref * fqn, const cwc::string_ref * id, cwc::intrusive_ptr<cwc::component> * cwc_ret) const noexcept -> const cwc::internal::error * final { return cwc::internal::call_and_return_error([&] { *cwc_ret = this->factory(*fqn, *id); }); }
	public:
		context_impl(std::istream & is) : configuration{parse_ini(is)} {
			key_value_map components;
			config_map plugins;

			if(const auto cit{configuration.find("cwc.mapping")}; cit != std::end(configuration)) {
				for(const auto & mapping : cit->second)
					if(mapping.second.front() != '[') components[mapping.first] = mapping.second;
					else {//key maps to section
						if(mapping.second.back() != ']') throw std::invalid_argument{"malformed configuration"};
						if(const auto section{configuration.find(mapping.second.substr(1, mapping.second.size() - 2))}; section != std::end(configuration))
							for(const auto & entry: section->second)
								plugins[mapping.first].insert(entry);
					}
			}

			dll_map map;//ensure that cwc_init is only called once
			for(const auto & [fqn, file] : components)
				if(const auto library{load_dll(map, file)}) component_factories.emplace(fqn, create_factory(map.at(library), file, fqn));
				else throw std::logic_error{"could not load bundle \"" + file + '"'};

			for(const auto & [fqn, mapping] : plugins)
				for(const auto & m : mapping)
					if(const auto library{load_dll(map, m.second)})
						plugin_factories[fqn].emplace(m.first, create_factory(map.at(library), m.second, fqn));
		}

		auto exception(const cwc::internal::error * err) const noexcept -> const cwc::internal::error * {
			assert(err);
			last_error.code = err->code;
			last_message[0] = '\0';
			std::strncat(last_message, err->message, std::min(sizeof(last_message) - 1, std::strlen(err->message)));
			return &last_error;
		}

		auto factory(cwc::string_ref fqn) const -> cwc::intrusive_ptr<cwc::component> {
			const std::string key{fqn};//TODO: this copies the string, which should not be necessary with C++20s generic find
			return component_factories.at(key);
		}

		auto factory(cwc::string_ref fqn, cwc::string_ref id) const -> cwc::intrusive_ptr<cwc::component> {
			const std::string key{fqn};//TODO: this copies the string, which should not be necessary with C++20s generic find
			return plugin_factories.at(key).at(std::string{id});
		}
	};

	const auto instance{[] {
#ifdef CWC_CONTEXT_INIT_IS_NOT_FILE
		std::istringstream is{CWC_CONTEXT_INIT_STRING};
#else
		std::ifstream is{CWC_CONTEXT_INIT_STRING};
		if(!is) throw std::invalid_argument{"could not open file \"" + std::string{CWC_CONTEXT_INIT_STRING} +"\" for context initialization"};
#endif
		return context_impl{is};
	}()};
}

auto ::cwc::this_context() -> const context & { return instance; }
