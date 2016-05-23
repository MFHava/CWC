
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once

#define CWC_CALL __cdecl
#define CWC_EXPORT __declspec(dllexport)

#define CWC_PACK_BEGIN __pragma(pack(push, 1))
#define CWC_PACK_END __pragma(pack(pop))

#if _MSC_VER < 1800//1800 == VC12 == VS2013
	#error "unsupported version of VC++, use at least VS 2013!"
#endif

#if _MSC_VER < 1900//1900 == VC14 == VS2015
	#define CWC_NOEXCEPT throw()//Microsoft's non-standard throw()-implementation matches the semantics of noexcept (at least as a workaround)
	#define CWC_THREAD_LOCAL __declspec(thread)//does not allow dynamic initialization
	#define CWC_HAS_NO_IMPLICIT_MOVE_SEMANTIC//not supported
#else
	#define CWC_NOEXCEPT noexcept
	#define CWC_THREAD_LOCAL thread_local
#endif

//support for autolinking
#define CWC_AUTOLINK_SUPPORTED
#define CWC_AUTOLINK_TOOLKIT "msvc" CWC_STRINGIFY(_MSC_VER)
#ifdef _DEBUG
	#define CWC_AUTOLINK_DEBUG
#endif