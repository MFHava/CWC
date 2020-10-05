
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
		     			//encoding known future_errc as derived exceptions: https://en.cppreference.com/w/cpp/thread/future_errc
		     				std11_future_error_$_future_errc_$_broken_promise{error_code_v<1, 5, 0, 1>},
		     				std11_future_error_$_future_errc_$_future_already_retrieved{error_code_v<1, 5, 0, 2>},
		     				std11_future_error_$_future_errc_$_promise_already_satisfied{error_code_v<1, 5, 0, 3>},
		     				std11_future_error_$_future_errc_$_no_state{error_code_v<1, 5, 0, 4>},
		     	std98_runtime_error{error_code_v<2>},
		     		std98_range_error{error_code_v<2, 1>},
		     		std98_overflow_error{error_code_v<2, 2>},
		     		std98_underflow_error{error_code_v<2, 3>},
		     		std11_regex_error{error_code_v<2, 4>},
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
		     		std11_system_error{error_code_v<2, 5>},
		     			//encoding known io_errc as derived exceptions: https://en.cppreference.com/w/cpp/io/io_errc
		     				std11_system_error_$_io_errc_$_stream{error_code_v<2, 5, 0, 1>},
		     			//encoding known future_errc as derived exceptions: https://en.cppreference.com/w/cpp/thread/future_errc
		     				std11_system_error_$_future_errc_$_broken_promise{error_code_v<2, 5, 0, 0, 1>},
		     				std11_system_error_$_future_errc_$_future_already_retrieved{error_code_v<2, 5, 0, 0, 2>},
		     				std11_system_error_$_future_errc_$_promise_already_satisfied{error_code_v<2, 5, 0, 0, 3>},
		     				std11_system_error_$_future_errc_$_no_state{error_code_v<2, 5, 0, 0, 4>},
		     			//enconding known errc as derived exceptions: https://en.cppreference.com/w/cpp/error/errc
		     				std11_system_error_$_errc_$_address_family_not_supported{error_code_v<2, 5, 0, 0, 0, 1>},
		     				std11_system_error_$_errc_$_address_in_use{error_code_v<2, 5, 0, 0, 0, 2>},
		     				std11_system_error_$_errc_$_address_not_available{error_code_v<2, 5, 0, 0, 0, 3>},
		     				std11_system_error_$_errc_$_already_connected{error_code_v<2, 5, 0, 0, 0, 4>},
		     				std11_system_error_$_errc_$_argument_list_too_long{error_code_v<2, 5, 0, 0, 0, 5>},
		     				std11_system_error_$_errc_$_argument_out_of_domain{error_code_v<2, 5, 0, 0, 0, 6>},
		     				std11_system_error_$_errc_$_bad_address{error_code_v<2, 5, 0, 0, 0, 7>},
		     				std11_system_error_$_errc_$_bad_file_descriptor{error_code_v<2, 5, 0, 0, 0, 8>},
		     				std11_system_error_$_errc_$_bad_message{error_code_v<2, 5, 0, 0, 0, 9>},
		     				std11_system_error_$_errc_$_broken_pipe{error_code_v<2, 5, 0, 0, 0, 10>},
		     				std11_system_error_$_errc_$_connection_aborted{error_code_v<2, 5, 0, 0, 0, 11>},
		     				std11_system_error_$_errc_$_connection_already_in_progress{error_code_v<2, 5, 0, 0, 0, 12>},
		     				std11_system_error_$_errc_$_connection_refused{error_code_v<2, 5, 0, 0, 0, 13>},
		     				std11_system_error_$_errc_$_connection_reset{error_code_v<2, 5, 0, 0, 0, 14>},
		     				std11_system_error_$_errc_$_cross_device_link{error_code_v<2, 5, 0, 0, 0, 15>},
		     				std11_system_error_$_errc_$_destination_address_required{error_code_v<2, 5, 0, 0, 0, 16>},
		     				std11_system_error_$_errc_$_device_or_resource_busy{error_code_v<2, 5, 0, 0, 0, 17>},
		     				std11_system_error_$_errc_$_directory_not_empty{error_code_v<2, 5, 0, 0, 0, 18>},
		     				std11_system_error_$_errc_$_executable_format_error{error_code_v<2, 5, 0, 0, 0, 19>},
		     				std11_system_error_$_errc_$_file_exists{error_code_v<2, 5, 0, 0, 0, 20>},
		     				std11_system_error_$_errc_$_file_too_large{error_code_v<2, 5, 0, 0, 0, 21>},
		     				std11_system_error_$_errc_$_filename_too_long{error_code_v<2, 5, 0, 0, 0, 22>},
		     				std11_system_error_$_errc_$_function_not_supported{error_code_v<2, 5, 0, 0, 0, 23>},
		     				std11_system_error_$_errc_$_host_unreachable{error_code_v<2, 5, 0, 0, 0, 24>},
		     				std11_system_error_$_errc_$_identifier_removed{error_code_v<2, 5, 0, 0, 0, 25>},
		     				std11_system_error_$_errc_$_illegal_byte_sequence{error_code_v<2, 5, 0, 0, 0, 26>},
		     				std11_system_error_$_errc_$_inappropriate_io_control_operation{error_code_v<2, 5, 0, 0, 0, 27>},
		     				std11_system_error_$_errc_$_interrupted{error_code_v<2, 5, 0, 0, 0, 28>},
		     				std11_system_error_$_errc_$_invalid_argument{error_code_v<2, 5, 0, 0, 0, 29>},
		     				std11_system_error_$_errc_$_invalid_seek{error_code_v<2, 5, 0, 0, 0, 30>},
		     				std11_system_error_$_errc_$_io_error{error_code_v<2, 5, 0, 0, 0, 31>},
		     				std11_system_error_$_errc_$_is_a_directory{error_code_v<2, 5, 0, 0, 0, 32>},
		     				std11_system_error_$_errc_$_message_size{error_code_v<2, 5, 0, 0, 0, 33>},
		     				std11_system_error_$_errc_$_network_down{error_code_v<2, 5, 0, 0, 0, 34>},
		     				std11_system_error_$_errc_$_network_reset{error_code_v<2, 5, 0, 0, 0, 35>},
		     				std11_system_error_$_errc_$_network_unreachable{error_code_v<2, 5, 0, 0, 0, 36>},
		     				std11_system_error_$_errc_$_no_buffer_space{error_code_v<2, 5, 0, 0, 0, 37>},
		     				std11_system_error_$_errc_$_no_child_process{error_code_v<2, 5, 0, 0, 0, 38>},
		     				std11_system_error_$_errc_$_no_link{error_code_v<2, 5, 0, 0, 0, 39>},
		     				std11_system_error_$_errc_$_no_lock_available{error_code_v<2, 5, 0, 0, 0, 40>},
		     				std11_system_error_$_errc_$_no_message_available{error_code_v<2, 5, 0, 0, 0, 41>},
		     				std11_system_error_$_errc_$_no_message{error_code_v<2, 5, 0, 0, 0, 42>},
		     				std11_system_error_$_errc_$_no_protocol_option{error_code_v<2, 5, 0, 0, 0, 43>},
		     				std11_system_error_$_errc_$_no_space_on_device{error_code_v<2, 5, 0, 0, 0, 44>},
		     				std11_system_error_$_errc_$_no_stream_resources{error_code_v<2, 5, 0, 0, 0, 45>},
		     				std11_system_error_$_errc_$_no_such_device_or_address{error_code_v<2, 5, 0, 0, 0, 46>},
		     				std11_system_error_$_errc_$_no_such_device{error_code_v<2, 5, 0, 0, 0, 47>},
		     				std11_system_error_$_errc_$_no_such_file_or_directory{error_code_v<2, 5, 0, 0, 0, 48>},
		     				std11_system_error_$_errc_$_no_such_process{error_code_v<2, 5, 0, 0, 0, 49>},
		     				std11_system_error_$_errc_$_not_a_directory{error_code_v<2, 5, 0, 0, 0, 50>},
		     				std11_system_error_$_errc_$_not_a_socket{error_code_v<2, 5, 0, 0, 0, 51>},
		     				std11_system_error_$_errc_$_not_a_stream{error_code_v<2, 5, 0, 0, 0, 52>},
		     				std11_system_error_$_errc_$_not_connected{error_code_v<2, 5, 0, 0, 0, 53>},
		     				std11_system_error_$_errc_$_not_enough_memory{error_code_v<2, 5, 0, 0, 0, 54>},
		     				std11_system_error_$_errc_$_not_supported{error_code_v<2, 5, 0, 0, 0, 55>},
		     				std11_system_error_$_errc_$_operation_canceled{error_code_v<2, 5, 0, 0, 0, 56>},
		     				std11_system_error_$_errc_$_operation_in_progress{error_code_v<2, 5, 0, 0, 0, 57>},
		     				std11_system_error_$_errc_$_operation_not_permitted{error_code_v<2, 5, 0, 0, 0, 58>},
		     				std11_system_error_$_errc_$_operation_not_supported{error_code_v<2, 5, 0, 0, 0, 59>},
		     				std11_system_error_$_errc_$_operation_would_block{error_code_v<2, 5, 0, 0, 0, 60>},
		     				std11_system_error_$_errc_$_owner_dead{error_code_v<2, 5, 0, 0, 0, 61>},
		     				std11_system_error_$_errc_$_permission_denied{error_code_v<2, 5, 0, 0, 0, 62>},
		     				std11_system_error_$_errc_$_protocol_error{error_code_v<2, 5, 0, 0, 0, 63>},
		     				std11_system_error_$_errc_$_protocol_not_supported{error_code_v<2, 5, 0, 0, 0, 64>},
		     				std11_system_error_$_errc_$_read_only_file_system{error_code_v<2, 5, 0, 0, 0, 65>},
		     				std11_system_error_$_errc_$_resource_deadlock_would_occur{error_code_v<2, 5, 0, 0, 0, 66>},
		     				std11_system_error_$_errc_$_resource_unavailable_try_again{error_code_v<2, 5, 0, 0, 0, 67>},
		     				std11_system_error_$_errc_$_result_out_of_range{error_code_v<2, 5, 0, 0, 0, 68>},
		     				std11_system_error_$_errc_$_state_not_recoverable{error_code_v<2, 5, 0, 0, 0, 69>},
		     				std11_system_error_$_errc_$_stream_timeout{error_code_v<2, 5, 0, 0, 0, 70>},
		     				std11_system_error_$_errc_$_text_file_busy{error_code_v<2, 5, 0, 0, 0, 71>},
		     				std11_system_error_$_errc_$_timed_out{error_code_v<2, 5, 0, 0, 0, 72>},
		     				std11_system_error_$_errc_$_too_many_files_open_in_system{error_code_v<2, 5, 0, 0, 0, 73>},
		     				std11_system_error_$_errc_$_too_many_files_open{error_code_v<2, 5, 0, 0, 0, 74>},
		     				std11_system_error_$_errc_$_too_many_links{error_code_v<2, 5, 0, 0, 0, 75>},
		     				std11_system_error_$_errc_$_too_many_symbolic_link_levels{error_code_v<2, 5, 0, 0, 0, 76>},
		     				std11_system_error_$_errc_$_value_too_large{error_code_v<2, 5, 0, 0, 0, 77>},
		     				std11_system_error_$_errc_$_wrong_protocol_type{error_code_v<2, 5, 0, 0, 0, 78>},
		     			std11_ios_base_failure{error_code_v<2, 5, 1>},
		     				//encoding known io_errc as derived exceptions: https://en.cppreference.com/w/cpp/io/io_errc
		     					std11_ios_base_failure_$_io_errc_$_stream{error_code_v<2, 5, 1, 0, 1>},
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

	void error_context::rethrow_if_necessary() const {
		if(!active) return;

		auto error{code};
		for(auto mask : masks) {
			error &= ~mask; //slice inheritance level
			if(error == std98_exception) throw std::exception{};
				if(error == std98_logic_error) throw std::logic_error{msg.data()};
					if(error == std98_invalid_argument) throw std::invalid_argument{msg.data()};
					if(error == std98_domain_error) throw std::domain_error{msg.data()};
					if(error == std98_length_error) throw std::length_error{msg.data()};
					if(error == std98_out_of_range) throw std::out_of_range{msg.data()};
					if(error == std11_future_error) continue;
						if(error == std11_future_error_$_future_errc_$_broken_promise) throw std::future_error{std::future_errc::broken_promise};
						if(error == std11_future_error_$_future_errc_$_future_already_retrieved) throw std::future_error{std::future_errc::future_already_retrieved};
						if(error == std11_future_error_$_future_errc_$_promise_already_satisfied) throw std::future_error{std::future_errc::promise_already_satisfied};
						if(error == std11_future_error_$_future_errc_$_no_state) throw std::future_error{std::future_errc::no_state};
				if(error == std98_runtime_error) throw std::runtime_error{msg.data()};
					if(error == std98_range_error) throw std::range_error{msg.data()};
					if(error == std98_overflow_error) throw std::overflow_error{msg.data()};
					if(error == std98_underflow_error) throw std::underflow_error{msg.data()};
					if(error == std11_regex_error) continue;
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
					if(error == std11_system_error) continue; //NOTE: not constructible from message
						if(error == std11_system_error_$_io_errc_$_stream) throw std::system_error{std::io_errc::stream, msg.data()};
						if(error == std11_system_error_$_future_errc_$_broken_promise) throw std::system_error{std::future_errc::broken_promise, msg.data()};
						if(error == std11_system_error_$_future_errc_$_future_already_retrieved) throw std::system_error{std::future_errc::future_already_retrieved, msg.data()};
						if(error == std11_system_error_$_future_errc_$_promise_already_satisfied) throw std::system_error{std::future_errc::promise_already_satisfied, msg.data()};
						if(error == std11_system_error_$_future_errc_$_no_state) throw std::system_error{std::future_errc::no_state, msg.data()};
						if(error == std11_system_error_$_errc_$_address_family_not_supported) throw std::system_error{std::make_error_code(std::errc::address_family_not_supported), msg.data()};
						if(error == std11_system_error_$_errc_$_address_in_use) throw std::system_error{std::make_error_code(std::errc::address_in_use), msg.data()};
						if(error == std11_system_error_$_errc_$_address_not_available) throw std::system_error{std::make_error_code(std::errc::address_not_available), msg.data()};
						if(error == std11_system_error_$_errc_$_already_connected) throw std::system_error{std::make_error_code(std::errc::already_connected), msg.data()};
						if(error == std11_system_error_$_errc_$_argument_list_too_long) throw std::system_error{std::make_error_code(std::errc::argument_list_too_long), msg.data()};
						if(error == std11_system_error_$_errc_$_argument_out_of_domain) throw std::system_error{std::make_error_code(std::errc::argument_out_of_domain), msg.data()};
						if(error == std11_system_error_$_errc_$_bad_address) throw std::system_error{std::make_error_code(std::errc::bad_address), msg.data()};
						if(error == std11_system_error_$_errc_$_bad_file_descriptor) throw std::system_error{std::make_error_code(std::errc::bad_file_descriptor), msg.data()};
						if(error == std11_system_error_$_errc_$_bad_message) throw std::system_error{std::make_error_code(std::errc::bad_message), msg.data()};
						if(error == std11_system_error_$_errc_$_broken_pipe) throw std::system_error{std::make_error_code(std::errc::broken_pipe), msg.data()};
						if(error == std11_system_error_$_errc_$_connection_aborted) throw std::system_error{std::make_error_code(std::errc::connection_aborted), msg.data()};
						if(error == std11_system_error_$_errc_$_connection_already_in_progress) throw std::system_error{std::make_error_code(std::errc::connection_already_in_progress), msg.data()};
						if(error == std11_system_error_$_errc_$_connection_refused) throw std::system_error{std::make_error_code(std::errc::connection_refused), msg.data()};
						if(error == std11_system_error_$_errc_$_connection_reset) throw std::system_error{std::make_error_code(std::errc::connection_reset), msg.data()};
						if(error == std11_system_error_$_errc_$_cross_device_link) throw std::system_error{std::make_error_code(std::errc::cross_device_link), msg.data()};
						if(error == std11_system_error_$_errc_$_destination_address_required) throw std::system_error{std::make_error_code(std::errc::destination_address_required), msg.data()};
						if(error == std11_system_error_$_errc_$_device_or_resource_busy) throw std::system_error{std::make_error_code(std::errc::device_or_resource_busy), msg.data()};
						if(error == std11_system_error_$_errc_$_directory_not_empty) throw std::system_error{std::make_error_code(std::errc::directory_not_empty), msg.data()};
						if(error == std11_system_error_$_errc_$_executable_format_error) throw std::system_error{std::make_error_code(std::errc::executable_format_error), msg.data()};
						if(error == std11_system_error_$_errc_$_file_exists) throw std::system_error{std::make_error_code(std::errc::file_exists), msg.data()};
						if(error == std11_system_error_$_errc_$_file_too_large) throw std::system_error{std::make_error_code(std::errc::file_too_large), msg.data()};
						if(error == std11_system_error_$_errc_$_filename_too_long) throw std::system_error{std::make_error_code(std::errc::filename_too_long), msg.data()};
						if(error == std11_system_error_$_errc_$_function_not_supported) throw std::system_error{std::make_error_code(std::errc::function_not_supported), msg.data()};
						if(error == std11_system_error_$_errc_$_host_unreachable) throw std::system_error{std::make_error_code(std::errc::host_unreachable), msg.data()};
						if(error == std11_system_error_$_errc_$_identifier_removed) throw std::system_error{std::make_error_code(std::errc::identifier_removed), msg.data()};
						if(error == std11_system_error_$_errc_$_illegal_byte_sequence) throw std::system_error{std::make_error_code(std::errc::illegal_byte_sequence), msg.data()};
						if(error == std11_system_error_$_errc_$_inappropriate_io_control_operation) throw std::system_error{std::make_error_code(std::errc::inappropriate_io_control_operation), msg.data()};
						if(error == std11_system_error_$_errc_$_interrupted) throw std::system_error{std::make_error_code(std::errc::interrupted), msg.data()};
						if(error == std11_system_error_$_errc_$_invalid_argument) throw std::system_error{std::make_error_code(std::errc::invalid_argument), msg.data()};
						if(error == std11_system_error_$_errc_$_invalid_seek) throw std::system_error{std::make_error_code(std::errc::invalid_seek), msg.data()};
						if(error == std11_system_error_$_errc_$_io_error) throw std::system_error{std::make_error_code(std::errc::io_error), msg.data()};
						if(error == std11_system_error_$_errc_$_is_a_directory) throw std::system_error{std::make_error_code(std::errc::is_a_directory), msg.data()};
						if(error == std11_system_error_$_errc_$_message_size) throw std::system_error{std::make_error_code(std::errc::message_size), msg.data()};
						if(error == std11_system_error_$_errc_$_network_down) throw std::system_error{std::make_error_code(std::errc::network_down), msg.data()};
						if(error == std11_system_error_$_errc_$_network_reset) throw std::system_error{std::make_error_code(std::errc::network_reset), msg.data()};
						if(error == std11_system_error_$_errc_$_network_unreachable) throw std::system_error{std::make_error_code(std::errc::network_unreachable), msg.data()};
						if(error == std11_system_error_$_errc_$_no_buffer_space) throw std::system_error{std::make_error_code(std::errc::no_buffer_space), msg.data()};
						if(error == std11_system_error_$_errc_$_no_child_process) throw std::system_error{std::make_error_code(std::errc::no_child_process), msg.data()};
						if(error == std11_system_error_$_errc_$_no_link) throw std::system_error{std::make_error_code(std::errc::no_link), msg.data()};
						if(error == std11_system_error_$_errc_$_no_lock_available) throw std::system_error{std::make_error_code(std::errc::no_lock_available), msg.data()};
						if(error == std11_system_error_$_errc_$_no_message_available) throw std::system_error{std::make_error_code(std::errc::no_message_available), msg.data()};
						if(error == std11_system_error_$_errc_$_no_message) throw std::system_error{std::make_error_code(std::errc::no_message), msg.data()};
						if(error == std11_system_error_$_errc_$_no_protocol_option) throw std::system_error{std::make_error_code(std::errc::no_protocol_option), msg.data()};
						if(error == std11_system_error_$_errc_$_no_space_on_device) throw std::system_error{std::make_error_code(std::errc::no_space_on_device), msg.data()};
						if(error == std11_system_error_$_errc_$_no_stream_resources) throw std::system_error{std::make_error_code(std::errc::no_stream_resources), msg.data()};
						if(error == std11_system_error_$_errc_$_no_such_device_or_address) throw std::system_error{std::make_error_code(std::errc::no_such_device_or_address), msg.data()};
						if(error == std11_system_error_$_errc_$_no_such_device) throw std::system_error{std::make_error_code(std::errc::no_such_device), msg.data()};
						if(error == std11_system_error_$_errc_$_no_such_file_or_directory) throw std::system_error{std::make_error_code(std::errc::no_such_file_or_directory), msg.data()};
						if(error == std11_system_error_$_errc_$_no_such_process) throw std::system_error{std::make_error_code(std::errc::no_such_process), msg.data()};
						if(error == std11_system_error_$_errc_$_not_a_directory) throw std::system_error{std::make_error_code(std::errc::not_a_directory), msg.data()};
						if(error == std11_system_error_$_errc_$_not_a_socket) throw std::system_error{std::make_error_code(std::errc::not_a_socket), msg.data()};
						if(error == std11_system_error_$_errc_$_not_a_stream) throw std::system_error{std::make_error_code(std::errc::not_a_stream), msg.data()};
						if(error == std11_system_error_$_errc_$_not_connected) throw std::system_error{std::make_error_code(std::errc::not_connected), msg.data()};
						if(error == std11_system_error_$_errc_$_not_enough_memory) throw std::system_error{std::make_error_code(std::errc::not_enough_memory), msg.data()};
						if(error == std11_system_error_$_errc_$_not_supported) throw std::system_error{std::make_error_code(std::errc::not_supported), msg.data()};
						if(error == std11_system_error_$_errc_$_operation_canceled) throw std::system_error{std::make_error_code(std::errc::operation_canceled), msg.data()};
						if(error == std11_system_error_$_errc_$_operation_in_progress) throw std::system_error{std::make_error_code(std::errc::operation_in_progress), msg.data()};
						if(error == std11_system_error_$_errc_$_operation_not_permitted) throw std::system_error{std::make_error_code(std::errc::operation_not_permitted), msg.data()};
						if(error == std11_system_error_$_errc_$_operation_not_supported) throw std::system_error{std::make_error_code(std::errc::operation_not_supported), msg.data()};
						if(error == std11_system_error_$_errc_$_operation_would_block) throw std::system_error{std::make_error_code(std::errc::operation_would_block), msg.data()};
						if(error == std11_system_error_$_errc_$_owner_dead) throw std::system_error{std::make_error_code(std::errc::owner_dead), msg.data()};
						if(error == std11_system_error_$_errc_$_permission_denied) throw std::system_error{std::make_error_code(std::errc::permission_denied), msg.data()};
						if(error == std11_system_error_$_errc_$_protocol_error) throw std::system_error{std::make_error_code(std::errc::protocol_error), msg.data()};
						if(error == std11_system_error_$_errc_$_protocol_not_supported) throw std::system_error{std::make_error_code(std::errc::protocol_not_supported), msg.data()};
						if(error == std11_system_error_$_errc_$_read_only_file_system) throw std::system_error{std::make_error_code(std::errc::read_only_file_system), msg.data()};
						if(error == std11_system_error_$_errc_$_resource_deadlock_would_occur) throw std::system_error{std::make_error_code(std::errc::resource_deadlock_would_occur), msg.data()};
						if(error == std11_system_error_$_errc_$_resource_unavailable_try_again) throw std::system_error{std::make_error_code(std::errc::resource_unavailable_try_again), msg.data()};
						if(error == std11_system_error_$_errc_$_result_out_of_range) throw std::system_error{std::make_error_code(std::errc::result_out_of_range), msg.data()};
						if(error == std11_system_error_$_errc_$_state_not_recoverable) throw std::system_error{std::make_error_code(std::errc::state_not_recoverable), msg.data()};
						if(error == std11_system_error_$_errc_$_stream_timeout) throw std::system_error{std::make_error_code(std::errc::stream_timeout), msg.data()};
						if(error == std11_system_error_$_errc_$_text_file_busy) throw std::system_error{std::make_error_code(std::errc::text_file_busy), msg.data()};
						if(error == std11_system_error_$_errc_$_timed_out) throw std::system_error{std::make_error_code(std::errc::timed_out), msg.data()};
						if(error == std11_system_error_$_errc_$_too_many_files_open_in_system) throw std::system_error{std::make_error_code(std::errc::too_many_files_open_in_system), msg.data()};
						if(error == std11_system_error_$_errc_$_too_many_files_open) throw std::system_error{std::make_error_code(std::errc::too_many_files_open), msg.data()};
						if(error == std11_system_error_$_errc_$_too_many_links) throw std::system_error{std::make_error_code(std::errc::too_many_links), msg.data()};
						if(error == std11_system_error_$_errc_$_too_many_symbolic_link_levels) throw std::system_error{std::make_error_code(std::errc::too_many_symbolic_link_levels), msg.data()};
						if(error == std11_system_error_$_errc_$_value_too_large) throw std::system_error{std::make_error_code(std::errc::value_too_large), msg.data()};
						if(error == std11_system_error_$_errc_$_wrong_protocol_type) throw std::system_error{std::make_error_code(std::errc::wrong_protocol_type), msg.data()};
						if(error == std11_ios_base_failure) throw std::ios_base::failure{msg.data()};
							if(error == std11_ios_base_failure_$_io_errc_$_stream) throw std::ios_base::failure{msg.data(), std::io_errc::stream};
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

	void error_context::store() noexcept { //lippincott function
		active = true;

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
					catch(const std::ios_base::failure & exc) {
						const auto & code{exc.code()};
						if(code == std::io_errc::stream) record(std11_ios_base_failure_$_io_errc_$_stream, exc.what());
						else record(std11_ios_base_failure, exc.what());
					}
				catch(const std::system_error & exc) {
					const auto & code{exc.code()};
					if(code == std::io_errc::stream) record(std11_system_error_$_io_errc_$_stream, exc.what());
					else if(code == std::future_errc::broken_promise) record(std11_system_error_$_future_errc_$_broken_promise, exc.what());
					else if(code == std::future_errc::future_already_retrieved) record(std11_system_error_$_future_errc_$_future_already_retrieved, exc.what());
					else if(code == std::future_errc::promise_already_satisfied) record(std11_system_error_$_future_errc_$_promise_already_satisfied, exc.what());
					else if(code == std::future_errc::no_state) record(std11_system_error_$_future_errc_$_no_state, exc.what());
					else if(code == std::errc::address_family_not_supported) record(std11_system_error_$_errc_$_address_family_not_supported, exc.what());
					else if(code == std::errc::address_in_use) record(std11_system_error_$_errc_$_address_in_use, exc.what());
					else if(code == std::errc::address_not_available) record(std11_system_error_$_errc_$_address_not_available, exc.what());
					else if(code == std::errc::already_connected) record(std11_system_error_$_errc_$_already_connected, exc.what());
					else if(code == std::errc::argument_list_too_long) record(std11_system_error_$_errc_$_argument_list_too_long, exc.what());
					else if(code == std::errc::argument_out_of_domain) record(std11_system_error_$_errc_$_argument_out_of_domain, exc.what());
					else if(code == std::errc::bad_address) record(std11_system_error_$_errc_$_bad_address, exc.what());
					else if(code == std::errc::bad_file_descriptor) record(std11_system_error_$_errc_$_bad_file_descriptor, exc.what());
					else if(code == std::errc::bad_message) record(std11_system_error_$_errc_$_bad_message, exc.what());
					else if(code == std::errc::broken_pipe) record(std11_system_error_$_errc_$_broken_pipe, exc.what());
					else if(code == std::errc::connection_aborted) record(std11_system_error_$_errc_$_connection_aborted, exc.what());
					else if(code == std::errc::connection_already_in_progress) record(std11_system_error_$_errc_$_connection_already_in_progress, exc.what());
					else if(code == std::errc::connection_refused) record(std11_system_error_$_errc_$_connection_refused, exc.what());
					else if(code == std::errc::connection_reset) record(std11_system_error_$_errc_$_connection_reset, exc.what());
					else if(code == std::errc::cross_device_link) record(std11_system_error_$_errc_$_cross_device_link, exc.what());
					else if(code == std::errc::destination_address_required) record(std11_system_error_$_errc_$_destination_address_required, exc.what());
					else if(code == std::errc::device_or_resource_busy) record(std11_system_error_$_errc_$_device_or_resource_busy, exc.what());
					else if(code == std::errc::directory_not_empty) record(std11_system_error_$_errc_$_directory_not_empty, exc.what());
					else if(code == std::errc::executable_format_error) record(std11_system_error_$_errc_$_executable_format_error, exc.what());
					else if(code == std::errc::file_exists) record(std11_system_error_$_errc_$_file_exists, exc.what());
					else if(code == std::errc::file_too_large) record(std11_system_error_$_errc_$_file_too_large, exc.what());
					else if(code == std::errc::filename_too_long) record(std11_system_error_$_errc_$_filename_too_long, exc.what());
					else if(code == std::errc::function_not_supported) record(std11_system_error_$_errc_$_function_not_supported, exc.what());
					else if(code == std::errc::host_unreachable) record(std11_system_error_$_errc_$_host_unreachable, exc.what());
					else if(code == std::errc::identifier_removed) record(std11_system_error_$_errc_$_identifier_removed, exc.what());
					else if(code == std::errc::illegal_byte_sequence) record(std11_system_error_$_errc_$_illegal_byte_sequence, exc.what());
					else if(code == std::errc::inappropriate_io_control_operation) record(std11_system_error_$_errc_$_inappropriate_io_control_operation, exc.what());
					else if(code == std::errc::interrupted) record(std11_system_error_$_errc_$_interrupted, exc.what());
					else if(code == std::errc::invalid_argument) record(std11_system_error_$_errc_$_invalid_argument, exc.what());
					else if(code == std::errc::invalid_seek) record(std11_system_error_$_errc_$_invalid_seek, exc.what());
					else if(code == std::errc::io_error) record(std11_system_error_$_errc_$_io_error, exc.what());
					else if(code == std::errc::is_a_directory) record(std11_system_error_$_errc_$_is_a_directory, exc.what());
					else if(code == std::errc::message_size) record(std11_system_error_$_errc_$_message_size, exc.what());
					else if(code == std::errc::network_down) record(std11_system_error_$_errc_$_network_down, exc.what());
					else if(code == std::errc::network_reset) record(std11_system_error_$_errc_$_network_reset, exc.what());
					else if(code == std::errc::network_unreachable) record(std11_system_error_$_errc_$_network_unreachable, exc.what());
					else if(code == std::errc::no_buffer_space) record(std11_system_error_$_errc_$_no_buffer_space, exc.what());
					else if(code == std::errc::no_child_process) record(std11_system_error_$_errc_$_no_child_process, exc.what());
					else if(code == std::errc::no_link) record(std11_system_error_$_errc_$_no_link, exc.what());
					else if(code == std::errc::no_lock_available) record(std11_system_error_$_errc_$_no_lock_available, exc.what());
					else if(code == std::errc::no_message_available) record(std11_system_error_$_errc_$_no_message_available, exc.what());
					else if(code == std::errc::no_message) record(std11_system_error_$_errc_$_no_message, exc.what());
					else if(code == std::errc::no_protocol_option) record(std11_system_error_$_errc_$_no_protocol_option, exc.what());
					else if(code == std::errc::no_space_on_device) record(std11_system_error_$_errc_$_no_space_on_device, exc.what());
					else if(code == std::errc::no_stream_resources) record(std11_system_error_$_errc_$_no_stream_resources, exc.what());
					else if(code == std::errc::no_such_device_or_address) record(std11_system_error_$_errc_$_no_such_device_or_address, exc.what());
					else if(code == std::errc::no_such_device) record(std11_system_error_$_errc_$_no_such_device, exc.what());
					else if(code == std::errc::no_such_file_or_directory) record(std11_system_error_$_errc_$_no_such_file_or_directory, exc.what());
					else if(code == std::errc::no_such_process) record(std11_system_error_$_errc_$_no_such_process, exc.what());
					else if(code == std::errc::not_a_directory) record(std11_system_error_$_errc_$_not_a_directory, exc.what());
					else if(code == std::errc::not_a_socket) record(std11_system_error_$_errc_$_not_a_socket, exc.what());
					else if(code == std::errc::not_a_stream) record(std11_system_error_$_errc_$_not_a_stream, exc.what());
					else if(code == std::errc::not_connected) record(std11_system_error_$_errc_$_not_connected, exc.what());
					else if(code == std::errc::not_enough_memory) record(std11_system_error_$_errc_$_not_enough_memory, exc.what());
					else if(code == std::errc::not_supported) record(std11_system_error_$_errc_$_not_supported, exc.what());
					else if(code == std::errc::operation_canceled) record(std11_system_error_$_errc_$_operation_canceled, exc.what());
					else if(code == std::errc::operation_in_progress) record(std11_system_error_$_errc_$_operation_in_progress, exc.what());
					else if(code == std::errc::operation_not_permitted) record(std11_system_error_$_errc_$_operation_not_permitted, exc.what());
					else if(code == std::errc::operation_not_supported) record(std11_system_error_$_errc_$_operation_not_supported, exc.what());
					else if(code == std::errc::operation_would_block) record(std11_system_error_$_errc_$_operation_would_block, exc.what());
					else if(code == std::errc::owner_dead) record(std11_system_error_$_errc_$_owner_dead, exc.what());
					else if(code == std::errc::permission_denied) record(std11_system_error_$_errc_$_permission_denied, exc.what());
					else if(code == std::errc::protocol_error) record(std11_system_error_$_errc_$_protocol_error, exc.what());
					else if(code == std::errc::protocol_not_supported) record(std11_system_error_$_errc_$_protocol_not_supported, exc.what());
					else if(code == std::errc::read_only_file_system) record(std11_system_error_$_errc_$_read_only_file_system, exc.what());
					else if(code == std::errc::resource_deadlock_would_occur) record(std11_system_error_$_errc_$_resource_deadlock_would_occur, exc.what());
					else if(code == std::errc::resource_unavailable_try_again) record(std11_system_error_$_errc_$_resource_unavailable_try_again, exc.what());
					else if(code == std::errc::result_out_of_range) record(std11_system_error_$_errc_$_result_out_of_range, exc.what());
					else if(code == std::errc::state_not_recoverable) record(std11_system_error_$_errc_$_state_not_recoverable, exc.what());
					else if(code == std::errc::stream_timeout) record(std11_system_error_$_errc_$_stream_timeout, exc.what());
					else if(code == std::errc::text_file_busy) record(std11_system_error_$_errc_$_text_file_busy, exc.what());
					else if(code == std::errc::timed_out) record(std11_system_error_$_errc_$_timed_out, exc.what());
					else if(code == std::errc::too_many_files_open_in_system) record(std11_system_error_$_errc_$_too_many_files_open_in_system, exc.what());
					else if(code == std::errc::too_many_files_open) record(std11_system_error_$_errc_$_too_many_files_open, exc.what());
					else if(code == std::errc::too_many_links) record(std11_system_error_$_errc_$_too_many_links, exc.what());
					else if(code == std::errc::too_many_symbolic_link_levels) record(std11_system_error_$_errc_$_too_many_symbolic_link_levels, exc.what());
					else if(code == std::errc::value_too_large) record(std11_system_error_$_errc_$_value_too_large, exc.what());
					else if(code == std::errc::wrong_protocol_type) record(std11_system_error_$_errc_$_wrong_protocol_type, exc.what());
					else record(std11_system_error, exc.what());
				}
				catch(const std::regex_error & exc) {
					switch(exc.code()) {
						case std::regex_constants::error_collate:    record(std11_regex_error_$_error_collate, exc.what());    break;
						case std::regex_constants::error_ctype:      record(std11_regex_error_$_error_ctype, exc.what());      break;
						case std::regex_constants::error_escape:     record(std11_regex_error_$_error_escape, exc.what());     break;
						case std::regex_constants::error_backref:    record(std11_regex_error_$_error_backref, exc.what());    break;
						case std::regex_constants::error_brack:      record(std11_regex_error_$_error_brack, exc.what());      break;
						case std::regex_constants::error_paren:      record(std11_regex_error_$_error_paren, exc.what());      break;
						case std::regex_constants::error_brace:      record(std11_regex_error_$_error_brace, exc.what());      break;
						case std::regex_constants::error_badbrace:   record(std11_regex_error_$_error_badbrace, exc.what());   break;
						case std::regex_constants::error_range:      record(std11_regex_error_$_error_range, exc.what());      break;
						case std::regex_constants::error_space:      record(std11_regex_error_$_error_space, exc.what());      break;
						case std::regex_constants::error_badrepeat:  record(std11_regex_error_$_error_badrepeat, exc.what());  break;
						case std::regex_constants::error_complexity: record(std11_regex_error_$_error_complexity, exc.what()); break;
						case std::regex_constants::error_stack:      record(std11_regex_error_$_error_stack, exc.what());      break;
						default: record(std11_regex_error, exc.what());
					}
				}
				catch(const std::underflow_error & exc) { record(std98_underflow_error, exc.what()); }
				catch(const std::overflow_error & exc) { record(std98_overflow_error, exc.what()); }
				catch(const std::range_error & exc) { record(std98_range_error, exc.what()); }
			catch(const std::runtime_error & exc) { record(std98_runtime_error, exc.what()); }
				catch(const std::future_error & exc) {
					const auto & code{exc.code()};
					if(code == std::future_errc::broken_promise) record(std11_future_error_$_future_errc_$_broken_promise, exc.what());
					else if(code == std::future_errc::future_already_retrieved) record(std11_future_error_$_future_errc_$_future_already_retrieved, exc.what());
					else if(code == std::future_errc::promise_already_satisfied) record(std11_future_error_$_future_errc_$_promise_already_satisfied, exc.what());
					else if(code == std::future_errc::no_state) record(std11_future_error_$_future_errc_$_no_state, exc.what());
					else record(std11_future_error, exc.what());
				}
				catch(const std::out_of_range & exc) { record(std98_out_of_range, exc.what()); }
				catch(const std::length_error & exc) { record(std98_length_error, exc.what()); }
				catch(const std::domain_error & exc) { record(std98_domain_error, exc.what()); }
				catch(const std::invalid_argument & exc) { record(std98_invalid_argument, exc.what()); }
			catch(const std::logic_error & exc) { record(std98_logic_error, exc.what()); }
		catch(const std::exception & exc) { record(std98_exception, exc.what()); }
		catch(...) { record(std98_exception, ""); }
	}
}
