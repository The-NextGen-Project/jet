# ifndef NEXTGEN_CONFIG_H
# define NEXTGEN_CONFIG_H
#   if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) || defined(__WIN32) && !defined(__CYGWIN__)
#       define NG_OS_WINDOWS true
#       include <Windows.h>
#   elif __APPLE__
#       define NG_OS_APPLE true
#       include <TargetConditionals.h>
#       include <sys/types.h>
#       include <sys/mman.h>
#   elif __linux__ || __unix__
#       define NG_OS_LINUX true
#       include <unistd.h>
#       include <sys/types.h>
#       include <sys/mman.h>
#   else
#       error "Could Not Find A Valid OS"
#   endif
#   if defined(__GNUC__) || defined(__clang__) || defined(__psp2__)
#       define NG_INLINE	inline
#       define NG_AINLINE	inline __attribute__((always_inline))
#   elif defined(_MSC_VER)
#       define NG_INLINE	__inline
#       define NG_AINLINE	__forceinline
#   endif
#   if defined(_M_IX86)
#       define NG_FAST __fastcall
#   elif defined(__i386__)
#       define NG_FAST __attribute__((fastcall))
#   endif
#   if defined(NG_OS_WINDOWS)
#       define COLOR_WHITE     0
#       define COLOR_RED       4
#       define COLOR_GREEN     2
#       define COLOR_YELLOW    14
#       define COLOR_BLUE      9
#       define COLOR_CYAN      3
#       define COLOR_RESET     7
#   elif defined(NG_OS_LINUX) || defined(NG_OS_APPLE)
#       define COLOR_WHITE     "\x1B[1;37m"
#       define COLOR_RED       "\x1B[1;31m"
#       define COLOR_GREEN     "\x1B[1;32m"
#       define COLOR_YELLOW    "\x1B[1;33m"
#       define COLOR_BLUE      "\x1B[1;34m"
#       define COLOR_CYAN      "\x1B[1;36m"
#       define COLOR_RESET     "\x1B[0m"
#   else
#       error "Could Not Find A Valid OS"
#   endif
/* Older versions of MSVC do not have stdint.h values that are properly defined from C99 */
#   if defined(_MSC_VER) && (_MSC_VER < 1700)
typedef __int8 int8_t;
        typedef __int16 int16_t;
        typedef __int32 int32_t;
        typedef __int64 int64_t;
        typedef unsigned __int8 uint8_t;
        typedef unsigned __int16 uint16_t;
        typedef unsigned __int32 uint32_t;
        typedef unsigned __int64 uint64_t;
#   ifdef _WIN64
#       define UINTPTR_MAX 0xffffffffffffffff
        typedef __int64 intptr_t;
        typedef unsigned __int64 uintptr_t;
#   else
#       define UINTPTR_MAX 0xffffffff
        typedef __int32 intptr_t;
        typedef unsigned __int32 uintptr_t;
#   endif
#   else
#       define HAS_STDINT
#       include <cstdint>
#   endif
#   include <iostream>
#   include <iomanip>
#   include <vector>
#   include <fstream>
#   include <string>
#   include <functional>
#   include <ios>
#   include <exception>
#   include <cstring>
#   include <unordered_set>
#   include <memory>
#   include <array>
#   include <unordered_map>
#   include <algorithm>
#   include <fmt/os.h>
#   include <fmt/format.h>
#   ifdef HAS_STDINT /* This is non-specific compiler detection */
#       if UINTPTR_MAX == 0xffffffff
#           ifndef BIT32
#               define BIT32
#           endif
#       elif UINTPTR_MAX == 0xffffffffffffffff
#           ifndef BIT64
#               define BIT64
#           endif
#       else
#           error "This Compiler Does not have this detection"
#       endif
#   endif
#   if !defined(BIT32) && !defined(BIT64)
#     ifdef _WIN64 /* Only windows has this here because MSVC sometimes does not have stdint.h */
        define BIT64
#     else
#       define BIT32
#     endif
#   endif
#   if defined(BIT32)
#       define FNV_PRIME 16777619u
#       define FNV_OFF   2166136261u
#   elif defined(BIT64)
#       define FNV_PRIME 1099511628211u
#       define FNV_OFF   14695981039346656037u
#   endif
// I want a shortcut for unreachable code here ...
# define UNREACHABLE PANIC("unreachable")
// Function Lambdas
# define LAMBDA(Lambda, Ret, ...) typename\
= typename std::enable_if<std::is_convertible<Lambda, std::function<Ret \
(__VA_ARGS__)\
>>::value>::type
# define FOR(var, range) for (auto var = 0; var < range; ++var)
# define CAST(ty, value) ((ty)(value))


#endif //NEXTGEN_CONFIG_H
