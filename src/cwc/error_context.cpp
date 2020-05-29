
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cstring>
#include <stdexcept>
#include <cwc/cwc.hpp>

namespace cwc {
	namespace {
		template<uint8 Level0 = 0, uint8 Level1 = 0, uint8 Level2 = 0, uint8 Level3 = 0, uint8 Level4 = 0, uint8 Level5 = 0, uint8 Level6 = 0, uint8 Level7 = 0>
		inline
		static
		constexpr
		auto error_code_v{(static_cast<uint64>(Level0) << 56) | (static_cast<uint64>(Level1) << 48) | (static_cast<uint64>(Level2) << 40) | (static_cast<uint64>(Level3) << 32) | (static_cast<uint64>(Level4) << 24) | (static_cast<uint64>(Level5) << 16) | (static_cast<uint64>(Level6) << 8) | static_cast<uint64>(Level7)};
	}

	//exceptions of the standard library: http://en.cppreference.com/w/cpp/error/exception
	enum class error_context::error_code : uint64 {
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

	void error_context::clear() noexcept { code.reset(); }

	void error_context::rethrow_if_necessary() const {
		if(!code) return;

		struct unknown_error : std::exception {
			unknown_error() {}
			auto what() const noexcept -> const char * override { return "received unknown error code"; }
		};

		using UI = std::underlying_type_t<error_code>;

		auto error{*code};
		for(auto mask : {UI{0}, UI{255}, UI{255} << 8, UI{255} << 16, UI{255} << 24, UI{255} << 32, UI{255} << 40, UI{255} << 48, UI{255} << 56})
			switch(error = static_cast<error_code>(~mask & static_cast<UI>(error))) {//slice inheritance level
				case error_code::std98_exception: throw std::exception{};
				case error_code::std98_logic_error: throw std::logic_error{msg.data()};
				case error_code::std98_invalid_argument: throw std::invalid_argument{msg.data()};
				case error_code::std98_domain_error: throw std::domain_error{msg.data()};
				case error_code::std98_length_error: throw std::length_error{msg.data()};
				case error_code::std98_out_of_range: throw std::out_of_range{msg.data()};
				case error_code::std11_future_error: continue; //NOTE: not constructible from message
				case error_code::std98_runtime_error: throw std::runtime_error{msg.data()};
				case error_code::std98_range_error: throw std::range_error{msg.data()};
				case error_code::std98_overflow_error: throw std::overflow_error{msg.data()};
				case error_code::std98_underflow_error: throw std::underflow_error{msg.data()};
				case error_code::std11_regex_error: continue; //not constructible from message
				case error_code::std11_system_error: continue; //not constructible from message
				case error_code::std11_ios_base_failure: throw std::ios_base::failure{msg.data()};
				case error_code::std17_filesystem_error: continue; //NOTE: currently not available
				case error_code::std98_bad_typeid: throw std::bad_typeid{};
				case error_code::std98_bad_cast: throw std::bad_cast{};
				case error_code::std17_bad_any_cast: continue; //NOTE: currently not available
				case error_code::std98_bad_alloc: throw std::bad_alloc{};
				case error_code::std11_bad_array_new_length: throw std::bad_array_new_length{};
				case error_code::std98_bad_exception: throw std::bad_exception{};
				case error_code::std11_bad_weak_ptr: throw std::bad_weak_ptr{};
				case error_code::std11_bad_function_call: throw std::bad_function_call{};
				case error_code::std17_bad_optional_access: throw std::bad_optional_access{};
				case error_code::std17_bad_variant_access: throw std::bad_variant_access{};
			}
		throw unknown_error{};
	}

	error_context::error_context(ptl::array_ref<char> msg) : msg{msg} { if(msg.empty()) throw std::invalid_argument{"message buffer must at least be able to store 1 char"}; }

	error_context::~error_context() noexcept =default;

	void error_context::store() noexcept { //lippincott function
		auto record{[&](error_code type, ptl::string_ref message) noexcept {
			code = type;
			msg[0] = '\0';
			std::strncat(msg.data(), message.data(), std::min(msg.size() - 1, message.size()));
		}};

		try { throw; }
		catch(const std::bad_optional_access & exc) { record(error_code::std17_bad_optional_access, exc.what()); }
		catch(const std::bad_variant_access & exc) { record(error_code::std17_bad_variant_access, exc.what()); }
		catch(const std::bad_function_call & exc) { record(error_code::std11_bad_function_call, exc.what()); }
		catch(const std::bad_weak_ptr & exc) { record(error_code::std11_bad_weak_ptr, exc.what()); }
		catch(const std::bad_exception & exc) { record(error_code::std98_bad_exception, exc.what()); }
		catch(const std::bad_array_new_length & exc) { record(error_code::std11_bad_array_new_length, exc.what()); }
		catch(const std::bad_alloc & exc) { record(error_code::std98_bad_alloc, exc.what()); }
		//NOTE: currently not available: catch(const std::bad_any_cast & exc) { record(error_code::std17_bad_any_cast, exc.what()); }
		catch(const std::bad_cast & exc) { record(error_code::std98_bad_cast, exc.what()); }
		catch(const std::bad_typeid & exc) { record(error_code::std98_bad_typeid, exc.what()); }
		//NOTE: currently not available: catch(const std::filesystem::filesystem_error & exc) { record(error_code::std17_filesystem_error, exc.what()); }
		catch(const std::ios_base::failure & exc) { record(error_code::std11_ios_base_failure, exc.what()); }
		//NOTE: not constructable from msg: catch(const std::system_error & exc) { record(error_code::std11_system_error, exc.what()); }
		//NOTE: not constructable from msg: catch(const std::regex_error & exc) { record(error_code::std11_regex_error, exc.what()); }
		catch(const std::underflow_error & exc) { record(error_code::std98_underflow_error, exc.what()); }
		catch(const std::overflow_error & exc) { record(error_code::std98_overflow_error, exc.what()); }
		catch(const std::range_error & exc) { record(error_code::std98_range_error, exc.what()); }
		catch(const std::runtime_error & exc) { record(error_code::std98_runtime_error, exc.what()); }
		//NOTE: not constructable from msg: catch(const std::future_error & exc) { record(error_code::std11_future_error, exc.what()); }
		catch(const std::out_of_range & exc) { record(error_code::std98_out_of_range, exc.what()); }
		catch(const std::length_error & exc) { record(error_code::std98_length_error, exc.what()); }
		catch(const std::domain_error & exc) { record(error_code::std98_domain_error, exc.what()); }
		catch(const std::invalid_argument & exc) { record(error_code::std98_invalid_argument, exc.what()); }
		catch(const std::logic_error & exc) { record(error_code::std98_logic_error, exc.what()); }
		catch(const std::exception & exc) { record(error_code::std98_exception, exc.what()); }
		catch(...) { record(error_code::std98_exception, ""); }
	}
}
