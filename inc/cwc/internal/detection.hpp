
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

#define CWC_COMPIELR_GNU   0
#define CWC_COMPILER_MSVC  0

#if defined(__GNUG__)
	#undef  CWC_COMPILER_GNU
	#define CWC_COMPILER_GNU 1
#elif defined(_MSC_VER)
	#undef  CWC_COMPILER_MSVC
	#define CWC_COMPILER_MSVC 1
#else
	#error unknown compiler
#endif

#define CWC_CALL
#if CWC_COMPILER_MSVC
	#define CWC_EXPORT __declspec(dllexport)
	#ifndef _M_X64//still using 32bit system
		#undef  CWC_CALL
		#define CWC_CALL __cdecl
	#endif
#elif CWC_COMPILER_GNU
	#define CWC_EXPORT __attribute__((visibility("default")))
	#ifndef __x86_64__//still using 32bit system
		#undef  CWC_CALL
		#define CWC_CALL __attribute__((__cdecl__))
	#endif
#else
	#error unknown compiler
#endif
