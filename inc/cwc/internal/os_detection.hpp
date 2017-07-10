
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#define CWC_OS_WINDOWS 0
#define CWC_OS_LINUX   0

#ifdef _WIN32
	#undef  CWC_OS_WINDOWS
	#define CWC_OS_WINDOWS 1
#elif __linux__
	#undef  CWC_OS_LINUX
	#define CWC_OS_LINUX 1
#else
	#error unknown operating system
#endif

#if CWC_COMPILER_IS_MSVC || (CWC_COMPILER_IS_Intel && CWC_OS_WINDOWS)
	#define CWC_CALL       __cdecl
	#define CWC_EXPORT     __declspec(dllexport)
	#define CWC_PACK_BEGIN __pragma(pack(push, 1))
	#define CWC_PACK_END   __pragma(pack(pop))
#elif CWC_COMPILER_IS_GNU || (CWC_COMPILER_IS_Intel && CWC_OS_LINUX)
	#define CWC_CALL       __attribute__((__cdecl__))
	#define CWC_EXPORT     __attribute__((visibility("default")))
	#define CWC_PACK_BEGIN _Pragma("pack(push, 1)")
	#define CWC_PACK_END   _Pragma("pack(pop)")
#endif