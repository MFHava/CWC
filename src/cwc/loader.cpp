
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <map>
#include <cassert>
#include <shared_mutex>
#include <unordered_map>
#include <cwc/cwc.hpp>

#if defined(_WIN32)
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

		constexpr char path_separator{'\\'};
		constexpr std::string_view dll_prefix{""}, dll_suffix{".dll"};
		const std::string dll_name{"DLL"}; //TODO: C++20 will allow constexpr here
	}
#elif defined(__linux__)
	#include <climits>
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

		constexpr char path_separator{'/'};
		constexpr std::string_view dll_prefix{"lib"}, dll_suffix{".so"};
		const std::string dll_name{"SO"}; //TODO: C++20 will allow constexpr here
	}
#elif defined(__HAIKU__)
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

		constexpr char path_separator{'/'};
		constexpr std::string_view dll_prefix{"lib"}, dll_suffix{".so"};
		const std::string dll_name{"ADDON"}; //TODO: C++20 will allow constexpr here
	}
#else
	#error unknown operating system
#endif

namespace cwc {
	struct loader::pimpl final {
		pimpl(bool force_local) : executable_path{[&] {
			using namespace std::string_literals;
			if(!force_local) return ""s;
			auto exe{GetExecutableFileName()};
			if(const auto it{std::find(std::rbegin(exe), std::rend(exe), path_separator)}; it != std::rend(exe)) exe.erase(it.base(), std::end(exe));
			return exe;
		}()} {}

		~pimpl() noexcept {
			factories.clear();//clear all factories before unloading respective dlls
			for(const auto & [path, pair] : dlls) {
				const auto & [handle, factory]{pair};
				(void)factory;
				FreeLibrary(handle);
			}
		}

		using key_type = std::pair<const internal::uuid *, std::string>;

		auto factory(const internal::uuid & id, std::string_view dll) const -> handle<component> {
			key_type key{&id, dll};
			{
				const std::shared_lock lock{mutex};
				if(const auto it{factories.find(key)}; it != std::end(factories)) return it->second;
			}
			return load_factory(std::move(key));
		}
	private:
		using entry_point = internal::error_callback(CWC_CALL *)(const internal::uuid *, handle<component> *) noexcept;

		const std::string executable_path;
		mutable std::shared_mutex mutex;
		mutable std::map<key_type, handle<component>> factories;
		mutable std::unordered_map<std::string, std::pair<HMODULE, entry_point>> dlls;

		auto make_dll(std::string file) const -> std::string {
			file.insert(std::begin(file), std::begin(executable_path), std::end(executable_path));
			file.insert(std::find(std::rbegin(file), std::rend(file), path_separator).base(), std::begin(dll_prefix), std::end(dll_prefix));
			file.insert(std::end(file), std::begin(dll_suffix), std::end(dll_suffix));
			if(std::find(std::begin(file), std::end(file), path_separator) == std::end(file)) {
				file.insert(std::begin(file), '.');
				file.insert(std::begin(file), path_separator);
			}
			return file;
		}

		auto load_main(const std::string & path) const -> entry_point {
			auto dll{make_dll(path)};

			if(const auto it{dlls.find(dll)}; it == std::end(dlls)) {
				const auto handle{LoadLibrary(dll.c_str())};
				if(!handle) throw std::runtime_error{"could not load " + dll_name + " \"" + dll + '"'};
				const auto main{reinterpret_cast<entry_point>(GetProcAddress(handle, "cwc_main"))};
				if(!main) {
					FreeLibrary(handle);
					throw std::logic_error{"could not find entry point 'cwc_main' in " + dll_name +" \"" + dll + '"'};
				}
				dlls.emplace(std::move(dll), std::make_pair(handle, main));
				return main;
			} else return it->second.second;
		}


		auto load_factory(key_type && key) const -> handle<component> {
			const auto & [uuid, dll]{key};

			const std::lock_guard lock{mutex};
			const auto main{load_main(dll)};

			handle<component> ptr;
			const auto error{main(uuid, &ptr)};

			//TODO: attempt to unload dll if the following code throws?
			//TODO: would need info if dll was actually loaded for this call...

			if(!ptr) throw std::logic_error{"did not receive valid factory from " + dll_name + " \"" + dll + '"'};
			internal::rethrow_last_error(error);

			factories.emplace(std::move(key), ptr);
			return ptr;
		}
	};

	auto loader::factory(const internal::uuid & id, std::string_view dll) const -> handle<component> { return self->factory(id, dll); }

	loader::loader(bool force_local) : self{std::make_unique<pimpl>(force_local)} {}

	loader::~loader() noexcept =default;
}
