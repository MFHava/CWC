
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once
#include <exception>

namespace cwc {
	CWC_PACK_BEGIN
	struct error_handle {
		error_handle(error_handle &) =delete;
		error_handle(error_handle &&) noexcept =delete;
		auto operator=(error_handle &) -> error_handle & =delete;
		auto operator=(error_handle &&) noexcept -> error_handle & =delete;

		template<typename Func>
		void call_and_store(Func func) noexcept {
			try { func(); }
			catch(...) { store(std::current_exception()); }
		}

		template<typename Type, typename Func, typename... Args>
		void call(Type & self, Func func, Args &&... args) {
			(self.*func)(this, &args...);
			rethrow_if_necessary();
		}

		void clear() noexcept;

		void rethrow_if_necessary() const;
	protected:
		//! @throws iff msg.empty()
		error_handle(array_ref<char> msg);
		~error_handle() noexcept;
	private:
		template<uint8 Level0 = 0, uint8 Level1 = 0, uint8 Level2 = 0, uint8 Level3 = 0, uint8 Level4 = 0, uint8 Level5 = 0, uint8 Level6 = 0, uint8 Level7 = 0>
		inline
		static
		constexpr
		auto error_code_v{(static_cast<uint64>(Level0) << 56) | (static_cast<uint64>(Level1) << 48) | (static_cast<uint64>(Level2) << 40) | (static_cast<uint64>(Level3) << 32) | (static_cast<uint64>(Level4) << 24) | (static_cast<uint64>(Level5) << 16) | (static_cast<uint64>(Level6) << 8) | static_cast<uint64>(Level7)};

		//exceptions of the standard library: http://en.cppreference.com/w/cpp/error/exception
		enum class error_code : uint64 {
			std98_exception = error_code_v<>,
				std98_logic_error = error_code_v<1>,
					std98_invalid_argument = error_code_v<1, 1>,
					std98_domain_error = error_code_v<1, 2>,
					std98_length_error = error_code_v<1, 3>,
					std98_out_of_range = error_code_v<1, 4>,
					std11_future_error = error_code_v<1, 5>,
				std98_runtime_error = error_code_v<2>,
					std98_range_error = error_code_v<2, 1>,
					std98_overflow_error = error_code_v<2, 2>,
					std98_underflow_error = error_code_v<2, 3>,
					std11_regex_error = error_code_v<2, 4>,
					std11_system_error = error_code_v<2, 5>,
						std11_ios_base_failure = error_code_v<2, 5, 1>,
						std17_filesystem_error = error_code_v<2, 5, 2>,
				std98_bad_typeid = error_code_v<3>,
				std98_bad_cast = error_code_v<4>,
					std17_bad_any_cast = error_code_v<4, 1>,
				std98_bad_alloc = error_code_v<5>,
					std11_bad_array_new_length = error_code_v<5, 1>,
				std98_bad_exception = error_code_v<6>,
				std11_bad_weak_ptr = error_code_v<7>,
				std11_bad_function_call = error_code_v<8>,
				std17_bad_variant_access = error_code_v<9>,
				std17_bad_optional_access = error_code_v<10>,
		};

		void store(std::exception_ptr eptr) noexcept;

		optional<error_code> code;
		array_ref<char> msg;
	};
	CWC_PACK_END

	template<std::size_t Size>
	struct sized_error_handle final : error_handle {
		sized_error_handle() : error_handle{buffer} {}
	private:
		char buffer[Size];
	};

	using default_error_handle = sized_error_handle<256>;
}
