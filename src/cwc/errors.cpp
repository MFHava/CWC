
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <any>
#include <regex>
#include <future>
#include <cassert>
#include <cstring>
#include <variant>
#include <optional>
#include <stdexcept>
#include <filesystem>
#include <cwc/cwc.hpp>

//TODO: add new C++20 exceptions
namespace cwc::internal {
	namespace {
		using error_code = std::uint64_t;

		struct unknown_exception final : std::exception {
			auto what() const noexcept -> const char * override { return "unknown type (not derived from std::exception) was caught"; }
		};

		template<std::uint8_t Level0 = 0, std::uint8_t Level1 = 0, std::uint8_t Level2 = 0, std::uint8_t Level3 = 0, std::uint8_t Level4 = 0, std::uint8_t Level5 = 0, std::uint8_t Level6 = 0, std::uint8_t Level7 = 0>
		constexpr
		error_code error_code_v{(static_cast<std::uint64_t>(Level0) << 56) | (static_cast<std::uint64_t>(Level1) << 48) | (static_cast<std::uint64_t>(Level2) << 40) | (static_cast<std::uint64_t>(Level3) << 32) | (static_cast<std::uint64_t>(Level4) << 24) | (static_cast<std::uint64_t>(Level5) << 16) | (static_cast<std::uint64_t>(Level6) << 8) | static_cast<std::uint64_t>(Level7)};


		//exceptions of the standard library: http://en.cppreference.com/w/cpp/error/exception
		constexpr
		auto std98_exception{error_code_v<>},
		     	cwc_unknown_exception{error_code_v<0, 0, 0, 0, 0, 0, 0, 1>}, //marker caught type not derived from std::exception => will be rethrown is internally defined type with hardcoded message directly derived from std::exception
		     	std98_logic_error{error_code_v<1>},
		     		std98_invalid_argument{error_code_v<1, 1>},
		     		std98_domain_error{error_code_v<1, 2>},
		     		std98_length_error{error_code_v<1, 3>},
		     		std98_out_of_range{error_code_v<1, 4>},
		     		std11_future_error{error_code_v<1, 5>}, //only for unknown error code => will be handled by logic_error
		     			//encoding known future_errc as derived exceptions: https://en.cppreference.com/w/cpp/thread/future_errc
		     				std11_future_error_$_future_errc_$_broken_promise{error_code_v<1, 5, 0, 1>},
		     				std11_future_error_$_future_errc_$_future_already_retrieved{error_code_v<1, 5, 0, 2>},
		     				std11_future_error_$_future_errc_$_promise_already_satisfied{error_code_v<1, 5, 0, 3>},
		     				std11_future_error_$_future_errc_$_no_state{error_code_v<1, 5, 0, 4>},
		     	std98_runtime_error{error_code_v<2>},
		     		std98_range_error{error_code_v<2, 1>},
		     		std98_overflow_error{error_code_v<2, 2>},
		     		std98_underflow_error{error_code_v<2, 3>},
		     		std11_regex_error{error_code_v<2, 4>}, //only for unknown error code => will be handled by runtime_error
		     			//encoding known regex_constants::error_type as derived exceptions: https://en.cppreference.com/w/cpp/regex/error_type
		     				std11_regex_error_$_error_collate{error_code_v<2, 4, 0, 1>},
		     				std11_regex_error_$_error_ctype{error_code_v<2, 4, 0, 2>},
		     				std11_regex_error_$_error_escape{error_code_v<2, 4, 0, 3>},
		     				std11_regex_error_$_error_backref{error_code_v<2, 4, 0, 4>},
		     				std11_regex_error_$_error_brack{error_code_v<2, 4, 0, 5>},
		     				std11_regex_error_$_error_paren{error_code_v<2, 4, 0, 6>},
		     				std11_regex_error_$_error_brace{error_code_v<2, 4, 0, 7>},
		     				std11_regex_error_$_error_badbrace{error_code_v<2, 4, 0, 8>},
		     				std11_regex_error_$_error_range{error_code_v<2, 4, 0, 9>},
		     				std11_regex_error_$_error_space{error_code_v<2, 4, 0, 10>},
		     				std11_regex_error_$_error_badrepeat{error_code_v<2, 4, 0, 11>},
		     				std11_regex_error_$_error_complexity{error_code_v<2, 4, 0, 12>},
		     				std11_regex_error_$_error_stack{error_code_v<2, 4, 0, 13>},
		     		std11_system_error{error_code_v<2, 5>}, //TODO: comment
		     			//encoding known io_errc as derived exceptions: https://en.cppreference.com/w/cpp/io/io_errc
		     				std11_system_error_$_io_errc_$_stream{error_code_v<2, 5, 0, 1>},
		     			//encoding known future_errc as derived exceptions: https://en.cppreference.com/w/cpp/thread/future_errc
		     				std11_system_error_$_future_errc_$_broken_promise{error_code_v<2, 5, 0, 0, 1>},
		     				std11_system_error_$_future_errc_$_future_already_retrieved{error_code_v<2, 5, 0, 0, 2>},
		     				std11_system_error_$_future_errc_$_promise_already_satisfied{error_code_v<2, 5, 0, 0, 3>},
		     				std11_system_error_$_future_errc_$_no_state{error_code_v<2, 5, 0, 0, 4>},
		     			std11_ios_base_failure{error_code_v<2, 5, 1>}, //unknown error code is actually fine
		     				//encoding known io_errc as derived exceptions: https://en.cppreference.com/w/cpp/io/io_errc
		     					std11_ios_base_failure_$_io_errc_$_stream{error_code_v<2, 5, 1, 0, 1>},
		     			std17_filesystem_error{error_code_v<2, 5, 2>}, //TODO: comment
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
	}

	enum class error_selector : std::uint32_t {
		type,
		what,
		system_error_code,
		system_error_category,
		filesystem_path1,
		filesystem_path2,
	};

	void rethrow_last_error(error_callback callback) {
		assert(callback);

		const auto type{reinterpret_cast<const error_code *>(callback(error_selector::type))};
		assert(type);
		const auto msg{reinterpret_cast<const char *>(callback(error_selector::what))};
		assert(msg);

		auto build_code{[&]() -> std::optional<std::error_code> {
			const auto category{reinterpret_cast<const char *>(callback(error_selector::system_error_category))};
			const auto code{static_cast<int>(reinterpret_cast<std::intptr_t>(callback(error_selector::system_error_code)))};
			if(!code || !category) return std::nullopt; //skip obvious error
			for(const auto & cat : {&std::generic_category(), &std::system_category()}) //test for categories with assumed stable error codes
				if(!std::strcmp(category, cat->name())) //assume that nobody will re-use category-name
					return std::error_code{code, *cat};
			return std::nullopt; //unknown error category => handle as base exception
		}};

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

		auto error{*type};
		for(auto mask : masks) {
			error &= ~mask; //slice inheritance level
			if(error == std98_exception) throw std::exception{};
				if(error == cwc_unknown_exception) throw unknown_exception{};
				if(error == std98_logic_error) throw std::logic_error{msg};
					if(error == std98_invalid_argument) throw std::invalid_argument{msg};
					if(error == std98_domain_error) throw std::domain_error{msg};
					if(error == std98_length_error) throw std::length_error{msg};
					if(error == std98_out_of_range) throw std::out_of_range{msg};
					if(error == std11_future_error) continue; //unknown error code => handle as logic_error
						if(error == std11_future_error_$_future_errc_$_broken_promise) throw std::future_error{std::future_errc::broken_promise};
						if(error == std11_future_error_$_future_errc_$_future_already_retrieved) throw std::future_error{std::future_errc::future_already_retrieved};
						if(error == std11_future_error_$_future_errc_$_promise_already_satisfied) throw std::future_error{std::future_errc::promise_already_satisfied};
						if(error == std11_future_error_$_future_errc_$_no_state) throw std::future_error{std::future_errc::no_state};
				if(error == std98_runtime_error) throw std::runtime_error{msg};
					if(error == std98_range_error) throw std::range_error{msg};
					if(error == std98_overflow_error) throw std::overflow_error{msg};
					if(error == std98_underflow_error) throw std::underflow_error{msg};
					if(error == std11_regex_error) continue; //unknown error code => handle as runtime_error
						if(error == std11_regex_error_$_error_collate) throw std::regex_error{std::regex_constants::error_collate};
						if(error == std11_regex_error_$_error_ctype) throw std::regex_error{std::regex_constants::error_ctype};
						if(error == std11_regex_error_$_error_escape) throw std::regex_error{std::regex_constants::error_escape};
						if(error == std11_regex_error_$_error_backref) throw std::regex_error{std::regex_constants::error_backref};
						if(error == std11_regex_error_$_error_brack) throw std::regex_error{std::regex_constants::error_brack};
						if(error == std11_regex_error_$_error_paren) throw std::regex_error{std::regex_constants::error_paren};
						if(error == std11_regex_error_$_error_brace) throw std::regex_error{std::regex_constants::error_brace};
						if(error == std11_regex_error_$_error_badbrace) throw std::regex_error{std::regex_constants::error_badbrace};
						if(error == std11_regex_error_$_error_range) throw std::regex_error{std::regex_constants::error_range};
						if(error == std11_regex_error_$_error_space) throw std::regex_error{std::regex_constants::error_space};
						if(error == std11_regex_error_$_error_badrepeat) throw std::regex_error{std::regex_constants::error_badrepeat};
						if(error == std11_regex_error_$_error_complexity) throw std::regex_error{std::regex_constants::error_complexity};
						if(error == std11_regex_error_$_error_stack) throw std::regex_error{std::regex_constants::error_stack};
					if(error == std11_system_error) {
						if(const auto code{build_code()}) throw std::system_error{*code, msg};
						continue; //unknown error code => handle as runtime_error
					}
						if(error == std11_system_error_$_io_errc_$_stream) throw std::system_error{std::io_errc::stream, msg};
						if(error == std11_system_error_$_future_errc_$_broken_promise) throw std::system_error{std::future_errc::broken_promise, msg};
						if(error == std11_system_error_$_future_errc_$_future_already_retrieved) throw std::system_error{std::future_errc::future_already_retrieved, msg};
						if(error == std11_system_error_$_future_errc_$_promise_already_satisfied) throw std::system_error{std::future_errc::promise_already_satisfied, msg};
						if(error == std11_system_error_$_future_errc_$_no_state) throw std::system_error{std::future_errc::no_state, msg};
						if(error == std11_ios_base_failure) throw std::ios_base::failure{msg};
							if(error == std11_ios_base_failure_$_io_errc_$_stream) throw std::ios_base::failure{msg, std::io_errc::stream};
						if(error == std17_filesystem_error) {
							const auto code{build_code()};
							if(!code) continue;

							using path_type = const std::filesystem::path::value_type *;
							const auto path1{reinterpret_cast<path_type>(callback(error_selector::filesystem_path1))};
							const auto path2{reinterpret_cast<path_type>(callback(error_selector::filesystem_path2))};

							path1 ? path2 ? throw std::filesystem::filesystem_error{msg, path1, path2, *code}
							              : throw std::filesystem::filesystem_error{msg, path1, *code}
							      : throw std::filesystem::filesystem_error{msg, *code};
							continue; //unknown error code => handle as runtime_error
						}
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

		std::abort(); //unreachable
	}

	namespace {
		auto to_error_code(const std::exception &) -> const error_code * { return &std98_exception; }
		auto to_error_code(const unknown_exception &) -> const error_code * { return &cwc_unknown_exception; }
		auto to_error_code(const std::logic_error &) -> const error_code * { return &std98_logic_error; }
		auto to_error_code(const std::invalid_argument &) -> const error_code * { return &std98_invalid_argument; }
		auto to_error_code(const std::domain_error &) -> const error_code * { return &std98_domain_error; }
		auto to_error_code(const std::length_error &) -> const error_code * { return &std98_length_error; }
		auto to_error_code(const std::out_of_range &) -> const error_code * { return &std98_out_of_range; }
		auto to_error_code(const std::future_error & exc) -> const error_code * {
			const auto & code{exc.code()};
			if(code.category() == std::future_category()) {
				switch(static_cast<std::future_errc>(code.value())) {
					case std::future_errc::broken_promise: return &std11_future_error_$_future_errc_$_broken_promise;
					case std::future_errc::future_already_retrieved: return &std11_future_error_$_future_errc_$_future_already_retrieved;
					case std::future_errc::promise_already_satisfied: return &std11_future_error_$_future_errc_$_promise_already_satisfied;
					case std::future_errc::no_state: return &std11_future_error_$_future_errc_$_no_state;
				}
			}
			return &std11_future_error;
		}
		auto to_error_code(const std::runtime_error &) -> const error_code * { return &std98_runtime_error; }
		auto to_error_code(const std::range_error &) -> const error_code * { return &std98_range_error; }
		auto to_error_code(const std::overflow_error &) -> const error_code * { return &std98_overflow_error; }
		auto to_error_code(const std::underflow_error &) -> const error_code * { return &std98_underflow_error; }
		auto to_error_code(const std::regex_error & exc) -> const error_code * {
			switch(exc.code()) {
				case std::regex_constants::error_collate: return &std11_regex_error_$_error_collate;
				case std::regex_constants::error_ctype: return &std11_regex_error_$_error_ctype;
				case std::regex_constants::error_escape: return &std11_regex_error_$_error_escape;
				case std::regex_constants::error_backref: return &std11_regex_error_$_error_backref;
				case std::regex_constants::error_brack: return &std11_regex_error_$_error_brack;
				case std::regex_constants::error_paren: return &std11_regex_error_$_error_paren;
				case std::regex_constants::error_brace: return &std11_regex_error_$_error_brace;
				case std::regex_constants::error_badbrace: return &std11_regex_error_$_error_badbrace;
				case std::regex_constants::error_range: return &std11_regex_error_$_error_range;
				case std::regex_constants::error_space: return &std11_regex_error_$_error_space;
				case std::regex_constants::error_badrepeat: return &std11_regex_error_$_error_badrepeat;
				case std::regex_constants::error_complexity: return &std11_regex_error_$_error_complexity;
				case std::regex_constants::error_stack: return &std11_regex_error_$_error_stack;
				default: return &std11_regex_error;
			}
		}
		auto to_error_code(const std::system_error &) -> const error_code * { return &std11_system_error; }
		auto to_error_code(const std::ios_base::failure & exc) -> const error_code * {
			const auto & code{exc.code()};
			if(code.category() == std::iostream_category()) {
				switch(static_cast<std::io_errc>(code.value())) {
					case std::io_errc::stream: return &std11_ios_base_failure_$_io_errc_$_stream;
				}
			}
			return &std11_ios_base_failure;
		}
		auto to_error_code(const std::filesystem::filesystem_error &) -> const error_code * { return &std17_filesystem_error; }
		auto to_error_code(const std::bad_typeid &) -> const error_code * { return &std98_bad_typeid; }
		auto to_error_code(const std::bad_cast &) -> const error_code * { return &std98_bad_cast; }
		auto to_error_code(const std::bad_any_cast &) -> const error_code * { return &std17_bad_any_cast; }
		auto to_error_code(const std::bad_alloc &) -> const error_code * { return &std98_bad_alloc; }
		auto to_error_code(const std::bad_array_new_length &) -> const error_code * { return &std11_bad_array_new_length; }
		auto to_error_code(const std::bad_exception &) -> const error_code * { return &std98_bad_exception; }
		auto to_error_code(const std::bad_weak_ptr &) -> const error_code * { return &std11_bad_weak_ptr; }
		auto to_error_code(const std::bad_function_call &) -> const error_code * { return &std11_bad_function_call; }
		auto to_error_code(const std::bad_variant_access &) -> const error_code * { return &std17_bad_variant_access; }
		auto to_error_code(const std::bad_optional_access &) -> const error_code * { return &std17_bad_optional_access; }


		template<typename>
		constexpr
		bool has_error_code{false};

		template<>
		constexpr
		bool has_error_code<std::system_error>{true};

		template<>
		constexpr
		bool has_error_code<std::filesystem::filesystem_error>{true};


		template<typename>
		constexpr
		bool has_paths{false};

		template<>
		constexpr
		bool has_paths<std::filesystem::filesystem_error>{true};


		thread_local
		struct {
			std::variant<std::monostate, std::bad_optional_access, std::bad_variant_access, std::bad_function_call, std::bad_weak_ptr, std::bad_exception, std::bad_array_new_length, std::bad_alloc, std::bad_any_cast, std::bad_cast, std::bad_typeid, std::filesystem::filesystem_error, std::ios_base::failure, std::system_error, std::regex_error, std::underflow_error, std::overflow_error, std::range_error, std::runtime_error, std::future_error, std::out_of_range, std::length_error, std::domain_error, std::invalid_argument, std::logic_error, std::exception, unknown_exception> exc;
			error_callback func{nullptr};
		} last_error;


		template<typename Exception>
		void store_error(const Exception & exc) noexcept {
			last_error.exc = exc;
			last_error.func = [](error_selector selector) noexcept -> const void * {
				//TODO: breaks currently on MSVC static_assert(std::is_nothrow_copy_constructible_v<Exception>);
				const auto & exc{std::get<Exception>(last_error.exc)};
				switch(selector) {
					case error_selector::type:
						return to_error_code(exc);
					case error_selector::what:
						return exc.what();
					case error_selector::system_error_code:
						if constexpr(has_error_code<Exception>) return reinterpret_cast<const void *>(std::intptr_t{exc.code().value()});
						break;
					case error_selector::system_error_category:
						if constexpr(has_error_code<Exception>) return exc.code().category().name();
						break;
					case error_selector::filesystem_path1:
						if constexpr(has_paths<Exception>) return exc.path1().empty() ? nullptr : exc.path1().c_str();
						break;
					case error_selector::filesystem_path2:
						if constexpr(has_paths<Exception>) return exc.path2().empty() ? nullptr : exc.path2().c_str();
						break;
				}
				return nullptr;
			};
		}
	}

	auto store_last_error() noexcept -> error_callback {//lippincott function
		try { throw; }
			catch(const std::bad_optional_access & exc) { store_error(exc); }
			catch(const std::bad_variant_access & exc) { store_error(exc); }
			catch(const std::bad_function_call & exc) { store_error(exc); }
			catch(const std::bad_weak_ptr & exc) { store_error(exc); }
			catch(const std::bad_exception & exc) { store_error(exc); }
				catch(const std::bad_array_new_length & exc) { store_error(exc); }
			catch(const std::bad_alloc & exc) { store_error(exc); }
				catch(const std::bad_any_cast & exc) { store_error(exc); }
			catch(const std::bad_cast & exc) { store_error(exc); }
			catch(const std::bad_typeid & exc) { store_error(exc); }
					catch(const std::filesystem::filesystem_error & exc) { store_error(exc); }
					catch(const std::ios_base::failure & exc) { store_error(exc); }
				catch(const std::system_error & exc) { store_error(exc); }
				catch(const std::regex_error & exc) { store_error(exc); }
				catch(const std::underflow_error & exc) { store_error(exc); }
				catch(const std::overflow_error & exc) { store_error(exc); }
				catch(const std::range_error & exc) { store_error(exc); }
			catch(const std::runtime_error & exc) { store_error(exc); }
				catch(const std::future_error & exc) { store_error(exc); }
				catch(const std::out_of_range & exc) { store_error(exc); }
				catch(const std::length_error & exc) { store_error(exc); }
				catch(const std::domain_error & exc) { store_error(exc); }
				catch(const std::invalid_argument & exc) { store_error(exc); }
			catch(const std::logic_error & exc) { store_error(exc); }
			catch(const unknown_exception & exc) { store_error(exc); }
		catch(const std::exception & exc) { store_error(exc); }
		catch(...) { store_error(unknown_exception{}); }
		return last_error.func;
	}
}
