
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
	struct error_context {
		error_context(error_context &) =delete;
		error_context(error_context &&) noexcept =delete;
		auto operator=(error_context &) -> error_context & =delete;
		auto operator=(error_context &&) noexcept -> error_context & =delete;

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
		error_context(ptl::array_ref<char> msg);
		~error_context() noexcept;
	private:
		void store() noexcept;

		ptl::optional<ptl::bitset<64>> code;
		ptl::array_ref<char> msg;
	};
	CWC_PACK_END

	template<std::size_t Size>
	struct sized_error_context final : error_context {
		sized_error_context() : error_context{buffer} {}
	private:
		char buffer[Size];
	};

	using default_error_context = sized_error_context<256>;
}
