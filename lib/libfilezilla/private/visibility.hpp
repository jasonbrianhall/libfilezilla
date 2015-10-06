#ifndef LIBFILEZILLA_PRIVATE_VISIBILITY_HEADER
#define LIBFILEZILLA_PRIVATE_VISIBILITY_HEADER


// Symbol visibility. There are two main cases: Building libfilezilla and using it
#ifdef BUILDING_LIBFILEZILLA

  // Two cases when building: Windows, other platform
  #ifdef FZ_WINDOWS
 
    // Under Windows we can either use Visual Studio or a proper compiler
    #ifdef _MSC_VER
      #ifdef DLL_EXPORT
        #define FZ_PUBLIC_SYMBOL __declspec(dllexport)
      #endif
    #else
      #ifdef DLL_EXPORT
        #define FZ_PUBLIC_SYMBOL __declspec(dllexport)
      #else
        #define FZ_PUBLIC_SYMBOL __attribute__((visibility("default")))
        #define FZ_PRIVATE_SYMBOL __attribute__((visibility("hidden")))
      #endif
    #endif

  #else

    #define FZ_PUBLIC_SYMBOL __attribute__((visibility("default")))
    #define FZ_PRIVATE_SYMBOL __attribute__((visibility("hidden")))

  #endif

#else

  // Under MSW it makes a difference whether we use a static library or a DLL
  #if defined(FZ_WINDOWS) && defined(FZ_USING_DLL)
    #define FZ_PUBLIC_SYMBOL __declspec(dllimport)
  #endif

#endif


#ifndef FZ_PUBLIC_SYMBOL
#define FZ_PUBLIC_SYMBOL
#endif

#ifndef FZ_PRIVATE_SYMBOL
#define FZ_PRIVATE_SYMBOL
#endif

#endif
