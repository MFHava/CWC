
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cwc/cwc.hpp>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <filesystem>

#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#define NOSERVICE
	#define NOMCX
	#define NOTIME
	#define NOIME
	#define NOMINMAX
	#include <Windows.h> //TODO: can this include be minimized?

	//TODO: all Windows-related code must be updated to support wchar_t => can impl::impl still be a shared implementation?!
	namespace cwc::internal {
		using handle = HMODULE;
	
		inline
		auto executable_name() -> std::string {
			static_assert(sizeof(DWORD) == 4);
			for(std::string result(MAX_PATH, '\0');; result.resize(result.size() * 2)) {
				if(result.size() > std::numeric_limits<DWORD>::max()) throw std::runtime_error{"exceeding DWORD for path size"};
				if(const auto size{GetModuleFileName(nullptr, result.data(), static_cast<DWORD>(result.size()))}; size < result.size()) {
					result.resize(size);
					return result;
				}
			}
		}

		inline
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

	namespace cwc::internal {
		using handle = void *;
	
		inline
		auto executable_name() -> std::string {
			for(std::string result(PATH_MAX, '\0');; result.resize(result.size() * 2)) {
				if(const auto size{readlink("/proc/self/exe", result.data(), result.size())}; size < static_cast<ssize_t>(result.size())) {
					if(size == -1) throw std::runtime_error{"readlink failed"};
					result.resize(size);
					return result;
				}
			}
		}

		inline
		constexpr
		std::string_view dll_prefix{"lib"}, dll_suffix{".so"};
	}
#elif defined(__HAIKU__)
	#include <image.h>
	#define LoadLibrary(file) std::max(load_add_on(file), image_id{0})
	#define GetProcAddress(dll, function) [] {\
		void * result{nullptr};\
		get_image_symbol(dll, function, B_SYMBOL_TYPE_TEXT, &result);\
		return result;\
	}()
	#define FreeLibrary(dll) unload_add_on(dll)

	namespace cwc::internal {
		using handle = image_id;
	
		inline
		auto executable_name() -> std::string {
			image_info info;
			info.name[0] = '\0';
			for(int32 cookie{0}; get_next_image_info(B_CURRENT_TEAM, &cookie, &info) == B_OK && info.type != B_APP_IMAGE;);
			//TODO: error handling?!
			return info.name;
		}

		inline
		constexpr
		std::string_view dll_prefix{"lib"}, dll_suffix{".so"};
	}
#else
	#error unknown operating system
#endif

namespace cwc::internal {
	class dll::impl final {
		using vptr_t = void(CWC_CALL *)(int, void **) noexcept;

		vptr_t vptr;
		handle lib;
	public:
		impl(const char * dll, const char * class_) {
			auto fullpath{executable_name()};
			if(const auto it{std::find(std::rbegin(fullpath), std::rend(fullpath), std::filesystem::path::preferred_separator)}; it != std::rend(fullpath)) fullpath.erase(it.base(), std::end(fullpath));
			fullpath += dll_prefix;
			fullpath += dll;
			fullpath += dll_suffix;
			lib = LoadLibrary(fullpath.c_str());
			if(!lib) throw std::runtime_error{"could not load library"};
			vptr = reinterpret_cast<vptr_t>(GetProcAddress(lib, class_));
			if(!vptr) {
				FreeLibrary(lib);
				throw std::runtime_error{"could not find entry point"};
			}
		}

		impl(const impl &) =delete;
		auto operator=(const impl &) -> impl & =delete;
		~impl() noexcept { FreeLibrary(lib); }

		void invoke(int op, void * args[]) const {
			int error{}; //TODO: use correct type for error handler
			args[0] = &error;

			vptr(op, args);

			if(error) {
				//TODO: evaluate error handler
				throw std::runtime_error{"exception happened"};
			}
		}
	};

	void dll::invoke(int op, void * args[]) const { self->invoke(op, args); }

	dll::dll(const char * dll, const char * class_) : self{std::make_unique<impl>(dll, class_)} {}

	dll::~dll() noexcept =default;
}
