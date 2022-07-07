
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <any>
#include <ios>
#include <cassert>
#include <cstring>
#include <variant>
#include <optional>
#include <stdexcept>
#include <functional>
#include <unordered_map>
#include <cwc/cwc.hpp>

namespace cwc {
	auto unknown_exception::what() const noexcept -> const char * { return "unknown type (not derived from std::exception) was caught"; } //TODO: include information of caught type?
}

namespace cwc::internal {
	struct exception::vtable final { //TODO: rethink layout?
		void(*dtor)(const unsigned char *) noexcept;
		const char *(*what)(const unsigned char *) noexcept;
		std::uint64_t type{0};
	};

	namespace {
		template<std::uint8_t Level0 = 0, std::uint8_t Level1 = 0, std::uint8_t Level2 = 0, std::uint8_t Level3 = 0, std::uint8_t Level4 = 0, std::uint8_t Level5 = 0, std::uint8_t Level6 = 0, std::uint8_t Level7 = 0>
		constexpr
		std::uint64_t error_code_v{(static_cast<std::uint64_t>(Level0) << 56)
								  |(static_cast<std::uint64_t>(Level1) << 48)
								  |(static_cast<std::uint64_t>(Level2) << 40)
								  |(static_cast<std::uint64_t>(Level3) << 32)
								  |(static_cast<std::uint64_t>(Level4) << 24)
								  |(static_cast<std::uint64_t>(Level5) << 16)
								  |(static_cast<std::uint64_t>(Level6) <<  8)
								  | static_cast<std::uint64_t>(Level7)
								  };

		//top-level/rather generic exceptions of the standard library: http://en.cppreference.com/w/cpp/error/exception
		constexpr
		std::uint64_t std98_exception{error_code_v<1>},
		              	std98_logic_error{error_code_v<1, 1>},
		              		std98_invalid_argument{error_code_v<1, 1, 1>},
		              		std98_domain_error{error_code_v<1, 1, 2>},
		              		std98_length_error{error_code_v<1, 1, 3>},
		              		std98_out_of_range{error_code_v<1, 1, 4>},
		              	std98_runtime_error{error_code_v<1, 2>},
		              		std98_range_error{error_code_v<1, 2, 1>},
		              		std98_overflow_error{error_code_v<1, 2, 2>},
		              		std98_underflow_error{error_code_v<1, 2, 3>},
		              		std11_ios_base_failure{error_code_v<1, 2, 4>},
		              	std98_bad_typeid{error_code_v<1, 3>},
		              	std98_bad_cast{error_code_v<1, 4>},
		              		std17_bad_any_cast{error_code_v<1, 4, 1>},
		              	std98_bad_alloc{error_code_v<1, 5>},
		              		std11_bad_array_new_length{error_code_v<1, 5, 1>},
		              	std98_bad_exception{error_code_v<1, 6>},
		              	std11_bad_weak_ptr{error_code_v<1, 7>},
		              	std11_bad_function_call{error_code_v<1, 8>},
		              	std17_bad_variant_access{error_code_v<1, 9>},
		              	std17_bad_optional_access{error_code_v<1, 10>},
		              cwc_unknown_exception{error_code_v<2>}; //marker caught type not derived from std::exception => will be rethrown is internally defined type with hardcoded message directly derived from std::exception

		template<typename>
		constexpr
		bool dependent_false_v{false}; //TODO: [C++??] will become unnecessary if P1830 or P2593 are adopted!

		template<typename Exception>
		constexpr
		auto error_code() noexcept -> std::uint64_t {
			if constexpr(std::is_same_v<unknown_exception, Exception>) return cwc_unknown_exception;
			else if constexpr(std::is_same_v<std::exception, Exception>) return std98_exception;
			else if constexpr(std::is_same_v<std::logic_error, Exception>) return std98_logic_error;
			else if constexpr(std::is_same_v<std::invalid_argument, Exception>) return std98_invalid_argument;
			else if constexpr(std::is_same_v<std::domain_error, Exception>) return std98_domain_error;
			else if constexpr(std::is_same_v<std::length_error, Exception>) return std98_length_error;
			else if constexpr(std::is_same_v<std::out_of_range, Exception>) return std98_out_of_range;
			else if constexpr(std::is_same_v<std::runtime_error, Exception>) return std98_runtime_error;
			else if constexpr(std::is_same_v<std::range_error, Exception>) return std98_range_error;
			else if constexpr(std::is_same_v<std::overflow_error, Exception>) return std98_overflow_error;
			else if constexpr(std::is_same_v<std::underflow_error, Exception>) return std98_underflow_error;
			else if constexpr(std::is_same_v<std::ios_base::failure, Exception>) return std11_ios_base_failure;
			else if constexpr(std::is_same_v<std::bad_typeid, Exception>) return std98_bad_typeid;
			else if constexpr(std::is_same_v<std::bad_cast, Exception>) return std98_bad_cast;
			else if constexpr(std::is_same_v<std::bad_any_cast, Exception>) return std17_bad_any_cast;
			else if constexpr(std::is_same_v<std::bad_alloc, Exception>) return std98_bad_alloc;
			else if constexpr(std::is_same_v<std::bad_array_new_length, Exception>) return std11_bad_array_new_length;
			else if constexpr(std::is_same_v<std::bad_exception, Exception>) return std98_bad_exception;
			else if constexpr(std::is_same_v<std::bad_weak_ptr, Exception>) return std11_bad_weak_ptr;
			else if constexpr(std::is_same_v<std::bad_function_call, Exception>) return std11_bad_function_call;
			else if constexpr(std::is_same_v<std::bad_variant_access, Exception>) return std17_bad_variant_access;
			else if constexpr(std::is_same_v<std::bad_optional_access, Exception>) return std17_bad_optional_access;
			else static_assert(dependent_false_v<Exception>);
		}
	}

	template<typename T>
	void exception::store(const T & exc) noexcept {
		static_assert(sizeof(T) <= sizeof(buffer));
		static_assert(std::is_nothrow_copy_constructible_v<T>);
		static_assert(std::is_nothrow_destructible_v<T>);

		static constexpr vtable vtable{
			+[](const unsigned char * self) noexcept { reinterpret_cast<const T *>(self)->~T(); },
			+[](const unsigned char * self) noexcept { return reinterpret_cast<const T *>(self)->what(); },
			error_code<T>()
		};
		vptr = &vtable;
		new(buffer) T{exc};
	}

	void exception::catch_() noexcept { //lippincott function
		try { throw; }
			catch(const std::bad_optional_access & exc) { store(exc); }
			catch(const std::bad_variant_access & exc) { store(exc); }
			catch(const std::bad_function_call & exc) { store(exc); }
			catch(const std::bad_weak_ptr & exc) { store(exc); }
			catch(const std::bad_exception & exc) { store(exc); }
				catch(const std::bad_array_new_length & exc) { store(exc); }
			catch(const std::bad_alloc & exc) { store(exc); }
				catch(const std::bad_any_cast & exc) { store(exc); }
			catch(const std::bad_cast & exc) { store(exc); }
			catch(const std::bad_typeid & exc) { store(exc); }
				catch(const std::ios_base::failure & exc) { store(exc); }
				catch(const std::underflow_error & exc) { store(exc); }
				catch(const std::overflow_error & exc) { store(exc); }
				catch(const std::range_error & exc) { store(exc); }
			catch(const std::runtime_error & exc) { store(exc); }
				catch(const std::out_of_range & exc) { store(exc); }
				catch(const std::length_error & exc) { store(exc); }
				catch(const std::domain_error & exc) { store(exc); }
				catch(const std::invalid_argument & exc) { store(exc); }
			catch(const std::logic_error & exc) { store(exc); }
			catch(const unknown_exception & exc) { store(exc); }
		catch(const std::exception & exc) { store(exc); }
		catch(...) { store(unknown_exception{}); }
	}

	exception::exception() noexcept {
		static constexpr vtable vtable{
			+[](const unsigned char *) noexcept {},
			+[](const unsigned char *) noexcept -> const char * { return nullptr; }
		};
		vptr = &vtable;
	}

	exception::~exception() noexcept { vptr->dtor(buffer); }

	namespace {
		template<typename Exception>
		void throw_(const char * msg) {
			if constexpr(std::is_constructible_v<Exception, const char *>) throw Exception{msg};
			else throw Exception{};
		}

		const std::unordered_map<std::uint64_t, void(*)(const char *)> exceptions{
			{std98_exception, throw_<std::exception>},
			{std98_logic_error, throw_<std::logic_error>},
			{std98_invalid_argument, throw_<std::invalid_argument>},
			{std98_domain_error, throw_<std::domain_error>},
			{std98_length_error, throw_<std::length_error>},
			{std98_out_of_range, throw_<std::out_of_range>},
			{std98_runtime_error, throw_<std::runtime_error>},
			{std98_range_error, throw_<std::range_error>},
			{std98_overflow_error, throw_<std::overflow_error>},
			{std98_underflow_error, throw_<std::underflow_error>},
			{std11_ios_base_failure, throw_<std::ios::failure>},
			{std98_bad_typeid, throw_<std::bad_typeid>},
			{std98_bad_cast, throw_<std::bad_cast>},
			{std17_bad_any_cast, throw_<std::bad_any_cast>},
			{std98_bad_alloc, throw_<std::bad_alloc>},
			{std11_bad_array_new_length, throw_<std::bad_array_new_length>},
			{std98_bad_exception, throw_<std::bad_exception>},
			{std11_bad_weak_ptr, throw_<std::bad_weak_ptr>},
			{std11_bad_function_call, throw_<std::bad_function_call>},
			{std17_bad_variant_access, throw_<std::bad_variant_access>},
			{std17_bad_optional_access, throw_<std::bad_optional_access>},
			{cwc_unknown_exception, throw_<unknown_exception>}
		};

		constexpr
		std::uint64_t masks[]{
			~(std::uint64_t{0}),
			~(std::uint64_t{0xFF} <<  0),
			~(std::uint64_t{0xFF} <<  8),
			~(std::uint64_t{0xFF} << 16),
			~(std::uint64_t{0xFF} << 24),
			~(std::uint64_t{0xFF} << 32),
			~(std::uint64_t{0xFF} << 40),
			~(std::uint64_t{0xFF} << 48),
			~(std::uint64_t{0xFF} << 56)
		};
	}

	void exception::throw_() const {
		if(!vptr->type) return; //no exception stored...
		const auto msg{vptr->what(buffer)};
		assert(msg);

		auto error{vptr->type};
		for(auto mask : masks) {
			error &= mask; //slice inheritance level
			if(const auto it{exceptions.find(error)}; it != exceptions.end()) it->second(msg);
		}
		std::abort(); //unreachable
	}
}

