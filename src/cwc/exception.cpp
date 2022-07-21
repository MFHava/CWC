
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <any>
#include <ios>
//TODO: [C++20] #include <format>
#include <regex>
#include <future>
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
	namespace {
		template<std::uint8_t Level0 = 0, std::uint8_t Level1 = 0, std::uint8_t Level2 = 0, std::uint8_t Level3 = 0, std::uint8_t Level4 = 0, std::uint8_t Level5 = 0, std::uint8_t Level6 = 0, std::uint8_t Level7 = 0>
		constexpr
		std::uint64_t error_code_v{(static_cast<std::uint64_t>(Level0) << 56) | (static_cast<std::uint64_t>(Level1) << 48) | (static_cast<std::uint64_t>(Level2) << 40) | (static_cast<std::uint64_t>(Level3) << 32) | (static_cast<std::uint64_t>(Level4) << 24) | (static_cast<std::uint64_t>(Level5) << 16) | (static_cast<std::uint64_t>(Level6) <<  8) | static_cast<std::uint64_t>(Level7)};

		//top-level/rather generic exceptions of the standard library: http://en.cppreference.com/w/cpp/error/exception
		constexpr
		std::uint64_t std98_exception{error_code_v<1>},
		              	std98_logic_error{error_code_v<1, 1>},
		              		std98_invalid_argument{error_code_v<1, 1, 1>},
		              		std98_domain_error{error_code_v<1, 1, 2>},
		              		std98_length_error{error_code_v<1, 1, 3>},
		              		std98_out_of_range{error_code_v<1, 1, 4>},
		              		std11_future_error{error_code_v<1, 1, 5>},
		              			std11_future_error_broken_promise{error_code_v<1, 1, 5, 1>},
		              			std11_future_error_future_already_retrieved{error_code_v<1, 1, 5, 2>},
		              			std11_future_error_promise_already_satisfied{error_code_v<1, 1, 5, 3>},
		              			std11_future_error_no_state{error_code_v<1, 1, 5, 4>},
		              	std98_runtime_error{error_code_v<1, 2>},
		              		std98_range_error{error_code_v<1, 2, 1>},
		              		std98_overflow_error{error_code_v<1, 2, 2>},
		              		std98_underflow_error{error_code_v<1, 2, 3>},
		              		std11_system_error{error_code_v<1, 2, 4>}, //currently not supported
		              			//TODO: in theory could be future_category or iostream_category...
		              			std11_system_error_generic_error{error_code_v<1, 2, 4, 1>}, //currently not supported
		              				//TODO: generic errors could theoretically be mapped (generic => POSIX)
		              			std11_system_error_sytem_error{error_code_v<1, 2, 4, 2>}, //currently not supported
		              				//TODO: system errors are harder to forward generically...
		              			std11_ios_base_failure{error_code_v<1, 2, 4, 3>},
		              				std11_ios_base_failure_iostream_error{error_code_v<1, 2, 4, 3, 1>}, //currently not supported
		              					std11_ios_base_failure_iostream_error_stream{error_code_v<1, 2, 4, 3, 1, 1>},
		              			std11_filesystem_error{error_code_v<1, 2, 4, 4>}, //currently not supported
		              				//TODO: categories...
		              		std11_regex_error{error_code_v<1, 2, 5>},
		              			std11_regex_error_collate{error_code_v<1, 2, 5, 1>},
		              			std11_regex_error_ctype{error_code_v<1, 2, 5, 2>},
		              			std11_regex_error_escape{error_code_v<1, 2, 5, 3>},
		              			std11_regex_error_backref{error_code_v<1, 2, 5, 4>},
		              			std11_regex_error_brack{error_code_v<1, 2, 5, 5>},
		              			std11_regex_error_paren{error_code_v<1, 2, 5, 6>},
		              			std11_regex_error_brace{error_code_v<1, 2, 5, 7>},
		              			std11_regex_error_badbrace{error_code_v<1, 2, 5, 8>},
		              			std11_regex_error_range{error_code_v<1, 2, 5, 9>},
		              			std11_regex_error_space{error_code_v<1, 2, 5, 10>},
		              			std11_regex_error_badrepeat{error_code_v<1, 2, 5, 11>},
		              			std11_regex_error_complexity{error_code_v<1, 2, 5, 12>},
		              			std11_regex_error_stack{error_code_v<1, 2, 5, 13>},
		              		std20_format_error{error_code_v<1, 2, 6>},
		              		std20_nonexistent_local_time{error_code_v<1, 2, 7>}, //currently not supported
		              		std20_ambiguous_local_time{error_code_v<1, 2, 8>}, //currently not supported
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
		auto error_code(const Exception & exc) noexcept -> std::uint64_t {
			if constexpr(std::is_same_v<unknown_exception, Exception>) return cwc_unknown_exception;
			else if constexpr(std::is_same_v<std::exception, Exception>) return std98_exception;
			else if constexpr(std::is_same_v<std::logic_error, Exception>) return std98_logic_error;
			else if constexpr(std::is_same_v<std::invalid_argument, Exception>) return std98_invalid_argument;
			else if constexpr(std::is_same_v<std::domain_error, Exception>) return std98_domain_error;
			else if constexpr(std::is_same_v<std::length_error, Exception>) return std98_length_error;
			else if constexpr(std::is_same_v<std::out_of_range, Exception>) return std98_out_of_range;
			else if constexpr(std::is_same_v<std::runtime_error, Exception>) return std98_runtime_error;
			else if constexpr(std::is_same_v<std::future_error, Exception>) {
				if(const auto & code{exc.code()}; code.category() == std::future_category())
					switch(static_cast<std::future_errc>(code.value())) {
						case std::future_errc::broken_promise: return std11_future_error_broken_promise;
						case std::future_errc::future_already_retrieved: return std11_future_error_future_already_retrieved;
						case std::future_errc::promise_already_satisfied: return std11_future_error_promise_already_satisfied;
						case std::future_errc::no_state: return std11_future_error_no_state;
					}
				return std11_future_error;
			} else if constexpr(std::is_same_v<std::range_error, Exception>) return std98_range_error;
			else if constexpr(std::is_same_v<std::overflow_error, Exception>) return std98_overflow_error;
			else if constexpr(std::is_same_v<std::underflow_error, Exception>) return std98_underflow_error;
			else if constexpr(std::is_same_v<std::ios_base::failure, Exception>) {
				if(const auto & code{exc.code()}; code.category() == std::iostream_category())
					switch(static_cast<std::io_errc>(code.value())) {
						case std::io_errc::stream: return std11_ios_base_failure_iostream_error_stream;
						default: return std11_ios_base_failure_iostream_error;
					}
				return std11_ios_base_failure;
			} else if constexpr(std::is_same_v<std::regex_error, Exception>) {
				switch(exc.code()) {
					case std::regex_constants::error_collate: return std11_regex_error_collate;
					case std::regex_constants::error_ctype: return std11_regex_error_ctype;
					case std::regex_constants::error_escape: return std11_regex_error_escape;
					case std::regex_constants::error_backref: return std11_regex_error_backref;
					case std::regex_constants::error_brack: return std11_regex_error_brack;
					case std::regex_constants::error_paren: return std11_regex_error_paren;
					case std::regex_constants::error_brace: return std11_regex_error_brace;
					case std::regex_constants::error_badbrace: return std11_regex_error_badbrace;
					case std::regex_constants::error_range: return std11_regex_error_range;
					case std::regex_constants::error_space: return std11_regex_error_space;
					case std::regex_constants::error_badrepeat: return std11_regex_error_badrepeat;
					case std::regex_constants::error_complexity: return std11_regex_error_complexity;
					case std::regex_constants::error_stack: return std11_regex_error_stack;
					default: return std11_regex_error;
				}
			}
			//TODO: [C++20] else if constexpr(std::is_same_v<std::format_error, Exception>) return std20_format_error;
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

		enum class operation { dtor, what, type, };
	}

	struct exception::vtable final { //TODO: rethink layout? (double indirection is actually unnecessary as we don't really need a vtable...)
		void(*func)(operation, const unsigned char *, void *) noexcept;

		void dtor(const unsigned char * self) const noexcept { func(operation::dtor, self, nullptr); }
		auto what(const unsigned char * self) const noexcept -> const char * {
			const char * result;
			func(operation::what, self, &result);
			return result;
		}
		auto type(const unsigned char * self) const noexcept -> std::uint64_t {
			std::uint64_t result;
			func(operation::type, self, &result);
			return result;
		}
	};

	template<typename T>
	void exception::store(const T & exc) noexcept {
		static_assert(sizeof(T) <= sizeof(buffer));
		static_assert(std::is_nothrow_copy_constructible_v<T>);
		static_assert(std::is_nothrow_destructible_v<T>);

		static constexpr vtable vtable{
			+[](operation op, const unsigned char * obj, void * param) noexcept {
				const auto & self{*reinterpret_cast<const T *>(obj)};
				switch(op) {
					case operation::dtor:
						self.~T();
						break;
					case operation::what:
						*reinterpret_cast<const char **>(param) = self.what();
						break;
					case operation::type:
						*reinterpret_cast<std::uint64_t *>(param) = error_code(self);
						break;
				}
			}
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
				//TODO: [C++20] catch(const std::format_error & exc) { store(exc); }
				catch(const std::regex_error & exc) { store(exc); }
				catch(const std::ios_base::failure & exc) { store(exc); }
				catch(const std::underflow_error & exc) { store(exc); }
				catch(const std::overflow_error & exc) { store(exc); }
				catch(const std::range_error & exc) { store(exc); }
			catch(const std::runtime_error & exc) { store(exc); }
				catch(const std::future_error & exc) { store(exc); }
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
			+[](operation op, const unsigned char *, void * param) noexcept {
				if(op == operation::type) *reinterpret_cast<std::uint64_t *>(param) = 0;
			}
		};
		vptr = &vtable;
	}

	exception::~exception() noexcept { vptr->dtor(buffer); }

	namespace {
		template<typename Exception>
		void throw_(const char * msg) {
			if constexpr(std::is_constructible_v<Exception, const char *>) {
				assert(msg);
				throw Exception{msg};
			} else throw Exception{};
		}

		struct future_error_broken_promise : std::future_error { future_error_broken_promise() : std::future_error{std::future_errc::broken_promise} {} };
		struct future_error_future_already_retrieved : std::future_error { future_error_future_already_retrieved() : std::future_error{std::future_errc::future_already_retrieved} {} };
		struct future_error_promise_already_satisfied : std::future_error { future_error_promise_already_satisfied() : std::future_error{std::future_errc::promise_already_satisfied} {} };
		struct future_error_no_state : std::future_error { future_error_no_state() : std::future_error{std::future_errc::no_state} {} };

		struct regex_error_collate : std::regex_error { regex_error_collate() : std::regex_error{std::regex_constants::error_collate} {} };
		struct regex_error_ctype : std::regex_error { regex_error_ctype() : std::regex_error{std::regex_constants::error_ctype} {} };
		struct regex_error_escape : std::regex_error { regex_error_escape() : std::regex_error{std::regex_constants::error_escape} {} };
		struct regex_error_backref : std::regex_error { regex_error_backref() : std::regex_error{std::regex_constants::error_backref} {} };
		struct regex_error_brack : std::regex_error { regex_error_brack() : std::regex_error{std::regex_constants::error_brack} {} };
		struct regex_error_paren : std::regex_error { regex_error_paren() : std::regex_error{std::regex_constants::error_paren} {} };
		struct regex_error_brace : std::regex_error { regex_error_brace() : std::regex_error{std::regex_constants::error_brace} {} };
		struct regex_error_badbrace : std::regex_error { regex_error_badbrace() : std::regex_error{std::regex_constants::error_badbrace} {} };
		struct regex_error_range : std::regex_error { regex_error_range() : std::regex_error{std::regex_constants::error_range} {} };
		struct regex_error_space : std::regex_error { regex_error_space() : std::regex_error{std::regex_constants::error_space} {} };
		struct regex_error_badrepeat : std::regex_error { regex_error_badrepeat() : std::regex_error{std::regex_constants::error_badrepeat} {} };
		struct regex_error_complexity : std::regex_error { regex_error_complexity() : std::regex_error{std::regex_constants::error_complexity} {} };
		struct regex_error_stack : std::regex_error { regex_error_stack() : std::regex_error{std::regex_constants::error_stack} {} };

		struct ios_base_failure_iostream_error_stream : std::ios::failure { ios_base_failure_iostream_error_stream(const char * msg) : std::ios::failure{msg, std::io_errc::stream} {} };

		const std::unordered_map<std::uint64_t, void(*)(const char *)> exceptions{
			{std98_exception, throw_<std::exception>},
			{std98_logic_error, throw_<std::logic_error>},
			{std98_invalid_argument, throw_<std::invalid_argument>},
			{std98_domain_error, throw_<std::domain_error>},
			{std98_length_error, throw_<std::length_error>},
			{std98_out_of_range, throw_<std::out_of_range>},
			{std11_future_error_broken_promise, throw_<future_error_broken_promise>},
			{std11_future_error_future_already_retrieved, throw_<future_error_future_already_retrieved>},
			{std11_future_error_promise_already_satisfied, throw_<future_error_promise_already_satisfied>},
			{std11_future_error_no_state, throw_<future_error_no_state>},
			{std98_runtime_error, throw_<std::runtime_error>},
			{std98_range_error, throw_<std::range_error>},
			{std98_overflow_error, throw_<std::overflow_error>},
			{std98_underflow_error, throw_<std::underflow_error>},
			{std11_ios_base_failure, throw_<std::ios::failure>},
			{std11_ios_base_failure_iostream_error_stream, throw_<ios_base_failure_iostream_error_stream>},
			{std11_regex_error_collate, throw_<regex_error_collate>},
			{std11_regex_error_ctype, throw_<regex_error_ctype>},
			{std11_regex_error_escape, throw_<regex_error_escape>},
			{std11_regex_error_backref, throw_<regex_error_backref>},
			{std11_regex_error_brack, throw_<regex_error_brack>},
			{std11_regex_error_paren, throw_<regex_error_paren>},
			{std11_regex_error_brace, throw_<regex_error_brace>},
			{std11_regex_error_badbrace, throw_<regex_error_badbrace>},
			{std11_regex_error_range, throw_<regex_error_range>},
			{std11_regex_error_space, throw_<regex_error_space>},
			{std11_regex_error_badrepeat, throw_<regex_error_badrepeat>},
			{std11_regex_error_complexity, throw_<regex_error_complexity>},
			{std11_regex_error_stack, throw_<regex_error_stack>},
			//TODO: [C++20] {std20_format_error, throw_<std::format_error>},
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
		auto error{vptr->type(buffer)};
		if(!error) return; //no exception stored...

		for(auto mask : masks)
			if(const auto it{exceptions.find(error &= mask)}; it != exceptions.end())
				it->second(vptr->what(buffer));

		std::abort(); //unreachable
	}
}
