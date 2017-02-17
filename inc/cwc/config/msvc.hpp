
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once

#define CWC_CALL       __cdecl
#define CWC_EXPORT     __declspec(dllexport)
#define CWC_PACK_BEGIN __pragma(pack(push, 1))
#define CWC_PACK_END   __pragma(pack(pop))

//support for autolinking
#define CWC_AUTOLINK_SUPPORTED

#define CWC_AUTOLINK_TOOLKIT "msvc" CWC_STRINGIFY(_MSC_VER)

#ifdef _DEBUG
	#define CWC_AUTOLINK_DEBUG
#endif