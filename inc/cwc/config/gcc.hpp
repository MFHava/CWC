
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once

#define CWC_CALL       __attribute__((__cdecl__))
#define CWC_EXPORT     __attribute__((visibility("default")))
#define CWC_PACK_BEGIN _Pragma("pack(push, 1)")
#define CWC_PACK_END   _Pragma("pack(pop)")

//no support for autolinking on any platform