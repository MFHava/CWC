
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <string>
#include <stdexcept>
#include <algorithm>
#include <filesystem>

#ifdef _WIN32
	#define UNICODE
	#define WIN32_LEAN_AND_MEAN
	#define NOSERVICE
	#define NOMCX
	#define NOTIME
	#define NOIME
	#ifndef NOMINMAX
		#define NOMINMAX
	#endif
	#include <Windows.h>

	namespace {
		using handle = HMODULE;

		auto executable_name() -> std::filesystem::path {
			static_assert(sizeof(DWORD) == 4);
			for(std::wstring result(MAX_PATH, '\0');; result.resize(result.size() * 2)) {
				if(result.size() > std::numeric_limits<DWORD>::max()) throw std::runtime_error{"exceeding DWORD for path size"};
				if(const auto size{GetModuleFileNameW(nullptr, result.data(), static_cast<DWORD>(result.size()))}; size < result.size()) {
					result.resize(size);
					return result;
				}
			}
		}

		constexpr
		std::string_view dll_prefix{""}, dll_suffix{".dll"};
	}
#elif __linux__
	#include <dlfcn.h>
	#include <unistd.h>
	#include <linux/limits.h>

	#define LoadLibrary(file) dlopen(file, RTLD_NOW)
	#define GetProcAddress(dll, function) dlsym(dll, function)
	#define FreeLibrary(dll) dlclose(dll)

	namespace {
		using handle = void *;

		auto executable_name() -> std::filesystem::path {
			for(std::string result(PATH_MAX, '\0');; result.resize(result.size() * 2)) {
				if(const auto size{readlink("/proc/self/exe", result.data(), result.size())}; size < static_cast<ssize_t>(result.size())) {
					if(size == -1) throw std::runtime_error{"readlink failed"};
					result.resize(size);
					return result;
				}
			}
		}

		constexpr
		std::string_view dll_prefix{"lib"}, dll_suffix{".so"};
	}
#elif __APPLE__
	#include <dlfcn.h>
	#include <unistd.h>
	#include <libproc.h>

	#define LoadLibrary(file) dlopen(file, RTLD_NOW)
	#define GetProcAddress(dll, function) dlsym(dll, function)
	#define FreeLibrary(dll) dlclose(dll)

	namespace {
		using handle = void *;

		auto executable_name() -> std::filesystem::path {
			const auto pid{getpid()};
			char tmp[PROC_PIDPATHINFO_MAXSIZE];
			if(proc_pidpath(pid, tmp, sizeof(tmp)) == -1) throw std::runtime_error{"proc_pidpath failed"};
			return tmp;
		}

		constexpr
		std::string_view dll_prefix{"lib"}, dll_suffix{".dylib"};
	}

	
#elif defined(__HAIKU__)
	#include <image.h>
	#define LoadLibrary(file) std::max(load_add_on(file), image_id{0})
	#define GetProcAddress(dll, function) [&] {\
		void * result{nullptr};\
		get_image_symbol(dll, function, B_SYMBOL_TYPE_DATA, &result);\
		return result;\
	}()
	#define FreeLibrary(dll) unload_add_on(dll)

	namespace {
		using handle = image_id;

		auto executable_name() -> std::filesystem::path {
			image_info info;
			info.name[0] = '\0';
			for(int32 cookie{0}; get_next_image_info(B_CURRENT_TEAM, &cookie, &info) == B_OK && info.type != B_APP_IMAGE;);
			if(info.type != B_APP_IMAGE) throw std::runtime_error{"could not find APP_IMAGE"};
			return info.name;
		}

		constexpr
		std::string_view dll_prefix{"lib"}, dll_suffix{".so"};
	}
#else
	#error unknown operating system
#endif

#include <cwc/cwc.hpp>

namespace cwc::internal {
	namespace {
		const auto base_path{executable_name().remove_filename()};
	}

	struct context::native_handle final {
		handle lib;

		native_handle(const char * dll) {
			auto end{dll};
			while(*end != '\0') ++end;
			const auto it{std::find(std::make_reverse_iterator(end), std::make_reverse_iterator(dll), '/').base()};

			auto fullpath{base_path};
			if(it != dll) fullpath += std::string_view{dll, static_cast<std::size_t>(it - dll)};
			fullpath += dll_prefix;
			fullpath += std::string_view{it, static_cast<std::size_t>(end - it)};
			fullpath += dll_suffix;

			lib = LoadLibrary(fullpath.c_str());
			if(!lib) throw std::runtime_error{"could not load library"};
		}

		native_handle(const native_handle &) =delete;
		auto operator=(const native_handle &) -> native_handle & =delete;

		~native_handle() noexcept { FreeLibrary(lib); }

		auto resolve(const char * class_) const -> const void * {
			using namespace std::string_literals;
			const auto ptr{reinterpret_cast<const void *>(GetProcAddress(lib, ("cwc_export_"s + class_).c_str()))};
			if(!ptr) throw std::runtime_error{"could not find entry point"};
			return ptr;
		}
	};

	context::context(const char * dll, const char * class_, version ver) : lib{std::make_unique<const native_handle>(dll)} {
		const auto ptr{lib->resolve(class_)};
		const auto & h{*reinterpret_cast<const header *>(ptr)};
		//TODO: handle different header versions (changes will always only be additive)
		if(h.cversion < ver) throw std::runtime_error{"version mismatch detected"};
		vptr = reinterpret_cast<const char *>(ptr) + h.size;
	}

	context::~context() noexcept =default;
}
