#ifndef LIBFILEZILLA_PRIVATE_DEFS_HEADER
#define LIBFILEZILLA_PRIVATE_DEFS_HEADER

#if defined(_MSC_VER)
	#if _MSC_VER < 1900
		#error You need to use a C++14 compiler, such as Visual Studio 2015 or higher.
	#endif
#else
	#if defined(__cplusplus) && (__cplusplus < 201300) // For some reason GCC 4.9 thinks the correct value is 2013000 :(
		#error You need to use a C++14 compiler. Try passing -std=c++14 as compiler flag.
	#endif
#endif

// Set some mandatory defines which are used to select platform implementations
#if !defined(FZ_WINDOWS) && !defined(FZ_MAC) && !defined(FZ_UNIX)
	#if defined(_WIN32) || defined(_WIN64)
		#define FZ_WINDOWS 1
	#elif defined(__APPLE__)
		#define FZ_MAC 1
	#else
		#define FZ_UNIX 1
	#endif
#endif

#if defined(BUILDING_LIBFILEZILLA) && defined(HAVE_CONFIG_H)
#include "config.hpp"
#endif

#if defined(FZ_WINDOWS) && defined(_MSC_VER)
// Incredibly noisy warning, disable it.
#pragma warning(disable:4251)

// Useless warnings
#if defined(BUILDING_LIBFILEZILLA) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#endif //defined(FZ_WINDOWS) && defined(_MSC_VER)

#endif
