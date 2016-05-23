
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once

//TODO - NOTE: original content tested for GCC 4.8.2, started adapting for GCC 4.9.1
#define CWC_GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)

#define CWC_CALL __attribute__((__cdecl__))

#if defined(__MINGW32__)//MinGW only works correctly if you use the VC-syntax!
	#define CWC_EXPORT __declspec(dllexport)
#else
	#define CWC_EXPORT __attribute__((visibility("default"))) //GCC supports this and can hide any symbol that's not marked this way (-fvisibility=hidden)
#endif

#define CWC_PACK_BEGIN _Pragma("pack(push, 1)")
#define CWC_PACK_END _Pragma("pack(pop)")

#if CWC_GCC_VERSION < 40901
	#define CWC_HAS_NO_REGEX//has header with only the exception beeing implemented
#endif

#define CWC_HAS_NO_BAD_ARRAY_NEW_LENGTH//apparently missing from all GCC versions
#define CWC_NOEXCEPT noexcept
#define CWC_THREAD_LOCAL thread_local//is actually supported, but emulation in non-compliant compilers with the official name is impossible

#ifdef __MINGW32__//mingw does not support these features
	#define CWC_HAS_NO_THREADS
#endif

//no support for autolinking on any platform