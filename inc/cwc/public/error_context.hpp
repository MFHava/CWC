
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
	struct error_context final {
		error_context(const error_context &) =delete;
		error_context(error_context &&) noexcept =delete;
		auto operator=(const error_context &) -> error_context & =delete;
		auto operator=(error_context &&) noexcept -> error_context & =delete;
		~error_context() noexcept =default;

		using default_buffer = ptl::array<char, 256>;

		//! @throws iff msg.empty()
		explicit
		error_context(ptl::array_ref<char> msg);

		template<typename Func>
		void call_and_store(Func func) noexcept {
			try { func(); }
			catch(...) { store(); }
		}

		template<typename Type, typename Func, typename... Args>
		void call(Type & self, Func func, Args &&... args) {
			(self.*func)(std::addressof(args)..., this);
			rethrow_if_necessary();
		}

		void rethrow_if_necessary() const;
	private:
		void store() noexcept;

		ptl::bitset<64> code;
		ptl::array_ref<char> msg;
		bool active{false};
	};
	CWC_PACK_END
}
