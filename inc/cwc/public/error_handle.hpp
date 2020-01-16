
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once

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
			catch(...) { store(); }
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
		void store() noexcept;

		enum class error_code : uint64;

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
