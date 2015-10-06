#ifndef LIBFILEZILLA_PRIVATE_VISIBILITY_HEADER
#define LIBFILEZILLA_PRIVATE_VISIBILITY_HEADER

#ifdef FZ_WINDOWS
  #ifdef BUILDING_LIBFILEZILLA
	#define FZ_PUBLIC_SYMBOL __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
  #else
	#define FZ_PUBLIC_SYMBOL __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
  #endif
  #define FZ_PRIVATE_SYMBOL
#else
  #define FZ_PUBLIC_SYMBOL __attribute__((visibility("default")))
  #define FZ_PRIVATE_SYMBOL __attribute__((visibility("hidden")))
#endif

#endif
