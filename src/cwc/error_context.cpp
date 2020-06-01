
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <any>
#include <regex>
#include <future>
#include <cstring>
#include <stdexcept>
#include <filesystem>
#include <cwc/cwc.hpp>

namespace cwc {
	namespace {
		using error_code = ptl::bitset<64>;

		template<uint8 Level0 = 0, uint8 Level1 = 0, uint8 Level2 = 0, uint8 Level3 = 0, uint8 Level4 = 0, uint8 Level5 = 0, uint8 Level6 = 0, uint8 Level7 = 0>
		inline
		static
		constexpr
		error_code error_code_v{(static_cast<uint64>(Level0) << 56) | (static_cast<uint64>(Level1) << 48) | (static_cast<uint64>(Level2) << 40) | (static_cast<uint64>(Level3) << 32) | (static_cast<uint64>(Level4) << 24) | (static_cast<uint64>(Level5) << 16) | (static_cast<uint64>(Level6) << 8) | static_cast<uint64>(Level7)};

		//exceptions of the standard library: http://en.cppreference.com/w/cpp/error/exception
		constexpr
		auto std98_exception{error_code_v<>},
		     	std98_logic_error{error_code_v<1>},
		     		std98_invalid_argument{error_code_v<1, 1>},
		     		std98_domain_error{error_code_v<1, 2>},
		     		std98_length_error{error_code_v<1, 3>},
		     		std98_out_of_range{error_code_v<1, 4>},
		     		std11_future_error{error_code_v<1, 5>},
		     	std98_runtime_error{error_code_v<2>},
		     		std98_range_error{error_code_v<2, 1>},
		     		std98_overflow_error{error_code_v<2, 2>},
		     		std98_underflow_error{error_code_v<2, 3>},
		     		std11_regex_error{error_code_v<2, 4>},
		     		std11_system_error{error_code_v<2, 5>},
		     			std11_ios_base_failure{error_code_v<2, 5, 1>},
		     			std17_filesystem_error{error_code_v<2, 5, 2>},
		     	std98_bad_typeid{error_code_v<3>},
		     	std98_bad_cast{error_code_v<4>},
		     		std17_bad_any_cast{error_code_v<4, 1>},
		     	std98_bad_alloc{error_code_v<5>},
		     		std11_bad_array_new_length{error_code_v<5, 1>},
		     	std98_bad_exception{error_code_v<6>},
		     	std11_bad_weak_ptr{error_code_v<7>},
		     	std11_bad_function_call{error_code_v<8>},
		     	std17_bad_variant_access{error_code_v<9>},
		     	std17_bad_optional_access{error_code_v<10>};

		constexpr
		error_code masks[]{
			error_code{0},
			error_code{0xFF} <<  0,
			error_code{0xFF} <<  8,
			error_code{0xFF} << 16,
			error_code{0xFF} << 24,
			error_code{0xFF} << 32,
			error_code{0xFF} << 40,
			error_code{0xFF} << 48,
			error_code{0xFF} << 56
		};
	}

	void error_context::clear() noexcept { code.reset(); }

	void error_context::rethrow_if_necessary() const {
		if(!code) return;

		auto error{*code};
		for(auto mask : masks) {
			error &= ~mask; //slice inheritance level
			if(error == std98_exception) throw std::exception{};
				if(error == std98_logic_error) throw std::logic_error{msg.data()};
					if(error == std98_invalid_argument) throw std::invalid_argument{msg.data()};
					if(error == std98_domain_error) throw std::domain_error{msg.data()};
					if(error == std98_length_error) throw std::length_error{msg.data()};
					if(error == std98_out_of_range) throw std::out_of_range{msg.data()};
					if(error == std11_future_error) continue; //NOTE: not constructible from message
				if(error == std98_runtime_error) throw std::runtime_error{msg.data()};
					if(error == std98_range_error) throw std::range_error{msg.data()};
					if(error == std98_overflow_error) throw std::overflow_error{msg.data()};
					if(error == std98_underflow_error) throw std::underflow_error{msg.data()};
					if(error == std11_regex_error) continue; //not constructible from message
					if(error == std11_system_error) continue; //not constructible from message
						if(error == std11_ios_base_failure) throw std::ios_base::failure{msg.data()};
						if(error == std17_filesystem_error) continue; //NOTE: not constructible from message
				if(error == std98_bad_typeid) throw std::bad_typeid{};
				if(error == std98_bad_cast) throw std::bad_cast{};
					if(error == std17_bad_any_cast) throw std::bad_any_cast{};
				if(error == std98_bad_alloc) throw std::bad_alloc{};
					if(error == std11_bad_array_new_length) throw std::bad_array_new_length{};
				if(error == std98_bad_exception) throw std::bad_exception{};
				if(error == std11_bad_weak_ptr) throw std::bad_weak_ptr{};
				if(error == std11_bad_function_call) throw std::bad_function_call{};
				if(error == std17_bad_variant_access) throw std::bad_variant_access{};
				if(error == std17_bad_optional_access) throw std::bad_optional_access{};
		}

		//unreachable
		std::terminate();
	}

	error_context::error_context(ptl::array_ref<char> msg) : msg{msg} { if(msg.empty()) throw std::invalid_argument{"message buffer must at least be able to store 1 char"}; }

	error_context::~error_context() noexcept =default;

	void error_context::store() noexcept { //lippincott function
		auto record{[&](const error_code & type, ptl::string_ref message) noexcept {
			code = type;
			msg[0] = '\0';
			std::strncat(msg.data(), message.data(), std::min(msg.size() - 1, message.size()));
		}};

		try { throw; }
			catch(const std::bad_optional_access & exc) { record(std17_bad_optional_access, exc.what()); }
			catch(const std::bad_variant_access & exc) { record(std17_bad_variant_access, exc.what()); }
			catch(const std::bad_function_call & exc) { record(std11_bad_function_call, exc.what()); }
			catch(const std::bad_weak_ptr & exc) { record(std11_bad_weak_ptr, exc.what()); }
			catch(const std::bad_exception & exc) { record(std98_bad_exception, exc.what()); }
				catch(const std::bad_array_new_length & exc) { record(std11_bad_array_new_length, exc.what()); }
			catch(const std::bad_alloc & exc) { record(std98_bad_alloc, exc.what()); }
				catch(const std::bad_any_cast & exc) { record(std17_bad_any_cast, exc.what()); }
			catch(const std::bad_cast & exc) { record(std98_bad_cast, exc.what()); }
			catch(const std::bad_typeid & exc) { record(std98_bad_typeid, exc.what()); }
					catch(const std::filesystem::filesystem_error & exc) { record(std17_filesystem_error, exc.what()); }
					catch(const std::ios_base::failure & exc) { record(std11_ios_base_failure, exc.what()); }
				catch(const std::system_error & exc) { record(std11_system_error, exc.what()); }
				catch(const std::regex_error & exc) { record(std11_regex_error, exc.what()); }
				catch(const std::underflow_error & exc) { record(std98_underflow_error, exc.what()); }
				catch(const std::overflow_error & exc) { record(std98_overflow_error, exc.what()); }
				catch(const std::range_error & exc) { record(std98_range_error, exc.what()); }
			catch(const std::runtime_error & exc) { record(std98_runtime_error, exc.what()); }
				catch(const std::future_error & exc) { record(std11_future_error, exc.what()); }
				catch(const std::out_of_range & exc) { record(std98_out_of_range, exc.what()); }
				catch(const std::length_error & exc) { record(std98_length_error, exc.what()); }
				catch(const std::domain_error & exc) { record(std98_domain_error, exc.what()); }
				catch(const std::invalid_argument & exc) { record(std98_invalid_argument, exc.what()); }
			catch(const std::logic_error & exc) { record(std98_logic_error, exc.what()); }
		catch(const std::exception & exc) { record(std98_exception, exc.what()); }
		catch(...) { record(std98_exception, ""); }
	}
}
