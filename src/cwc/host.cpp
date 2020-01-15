
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <limits>
#include <climits>
#include <shared_mutex>
#include <unordered_map>
#include <cwc/cwc.hpp>

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

		constexpr char path_separator{'\\'};
		constexpr std::string_view dll_prefix{""}, dll_suffix{".dll"};
	}
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

		constexpr char path_separator{'/'};
		constexpr std::string_view dll_prefix{"lib"}, dll_suffix{".so"};
	}
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

		constexpr char path_separator{'/'};
		constexpr std::string_view dll_prefix{"lib"}, dll_suffix{".so"};
	}
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

	static_assert(CHAR_BIT == 8);
	static_assert(validate_floating_point_v<cwc::float32, 4>);
	static_assert(validate_floating_point_v<cwc::float64, 8>);

	auto make_path(std::string file) -> std::string {
		if(std::find(std::begin(file), std::end(file), path_separator) == std::end(file)) {
			file.insert(std::begin(file), '.');
			file.insert(std::begin(file), path_separator);
		}
		return file;
	}
}

namespace cwc {
	struct context::pimpl final {
		pimpl(bool force_local) noexcept : force_local{force_local} {}

		~pimpl() noexcept {
			factories.clear();//clear all factories before unloading respective dlls
			for(const auto & [handle, factory] : dlls) {
				(void)factory;
				FreeLibrary(handle);
			}
		}

		auto factory(error_handle & cwc_error, const std::type_info * type, std::string_view fqn, std::string_view dll) const -> intrusive_ptr<component> {
retry:
			{
				const std::shared_lock lock{mutex};
				if(const auto it{factories.find(type)}; it != std::end(factories)) return it->second;
			}
			load_factory(cwc_error, type, std::string{fqn}, std::string{dll});
			goto retry;
		}
	private:
		using entry_point = void(CWC_CALL *)(error_handle *, const string_ref *, intrusive_ptr<component> *);

		const bool force_local;
		mutable std::shared_mutex mutex;
		mutable std::unordered_map<const std::type_info *, intrusive_ptr<component>> factories;
		mutable std::unordered_map<HMODULE, entry_point> dlls;

		auto make_name(std::string file) const -> std::string {
			static const auto base{[] {
				auto exe{GetExecutableFileName()};
				if(const auto it{std::find(std::rbegin(exe), std::rend(exe), path_separator)}; it != std::rend(exe)) exe.erase(it.base(), std::end(exe));
				return exe;
			}()};
			if(force_local) file.insert(std::begin(file), std::begin(base), std::end(base));
			file.insert(std::find(std::rbegin(file), std::rend(file), path_separator).base(), std::begin(dll_prefix), std::end(dll_prefix));
			file.insert(std::end(file), std::begin(dll_suffix), std::end(dll_suffix));
			return file;
		}

		void load_factory(error_handle & cwc_error, const std::type_info * type, std::string fqn, std::string dll) const {
			const std::lock_guard lock{mutex};

			const auto handle{LoadLibrary(make_path(make_name(dll)).c_str())};
			if(!handle) throw std::runtime_error{"could not load bundle \"" + dll + '"'};
			if(dlls.count(handle)) FreeLibrary(handle); //keep only one "load count" per context

			using entry_point = void(CWC_CALL *)(error_handle *, const string_ref *, intrusive_ptr<component> *);
			const auto factory{reinterpret_cast<entry_point>(GetProcAddress(handle, "cwc_factory"))};
			if(!factory) {
				FreeLibrary(handle);
				throw std::logic_error{"could not find entry point 'cwc_factory' in bundle \"" + dll + '"'};
			}

			cwc::intrusive_ptr<cwc::component> ptr;
			const cwc::string_ref tmp{fqn.c_str()};
			factory(&cwc_error, &tmp, &ptr);
			try {
				if(!ptr) throw std::logic_error{"did not receive valid factory for component \"" + fqn + "\" from bundle \"" + dll + '"'};
				cwc_error.rethrow_if_necessary();
			} catch(...) {
				FreeLibrary(handle);
				throw;
			}

			//TODO: this can throw an exception... (bad_alloc)
			dlls[handle] = factory;
			factories[type] = std::move(ptr);
		}
	};

	auto context::factory(error_handle & cwc_error, const std::type_info * type, std::string_view fqn, std::string_view dll) const -> intrusive_ptr<component> { return self->factory(cwc_error, type, fqn, dll); }

	context::context(bool force_local) : self{std::make_unique<pimpl>(force_local)} {}

	context::~context() noexcept =default;
}
