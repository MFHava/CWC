
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once

namespace cwc {
	namespace internal {
		template<typename Type, typename Func, typename... Args>
		void call(Type & self, Func func, Args &&... args) {
			const auto err = (self.*func)(std::forward<Args>(args)...);
			internal::validate(err);
		}

		template<typename Type, typename Func, typename... Args>
		void call(const intrusive_ptr<Type> & ptr, Func func, Args &&... args) { call(*ptr, func, std::forward<Args>(args)...); }

		template<typename Type, typename Func, typename... Args>
		void call(intrusive_ptr<Type> & ptr, Func func, Args &&... args) { call(*ptr, func, std::forward<Args>(args)...); }
	}
}