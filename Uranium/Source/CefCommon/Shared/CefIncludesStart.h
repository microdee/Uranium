/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#ifdef __UNREAL__

#ifdef UR_CEF_INCLUDE_REGION
#error Uranium CEF include regions cannot be nested, one region is already opened before
#endif
#define UR_CEF_INCLUDE_REGION 1

THIRD_PARTY_INCLUDES_START
#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/AllowWindowsPlatformAtomics.h"

// temporarily undefine simplistic UE4 macros CEF might also use

#pragma push_macro("TEXT")
#pragma push_macro("CONSTEXPR")
#pragma push_macro("PI")
#pragma push_macro("dynamic_cast")
#pragma push_macro("VARARGS")
#pragma push_macro("CDECL")
#pragma push_macro("STDCALL")
#pragma push_macro("FORCEINLINE")
#pragma push_macro("FORCENOINLINE")
#pragma push_macro("ABSTRACT")
#pragma push_macro("LINE_TERMINATOR")
#pragma push_macro("LINE_TERMINATOR_ANSI")
#pragma push_macro("DLLEXPORT")
#pragma push_macro("DLLIMPORT")
#pragma push_macro("LIKELY")
#pragma push_macro("UNLIKELY")
#pragma push_macro("RESTRICT")
#pragma push_macro("MOBILE")
#pragma push_macro("CONSOLE")
#pragma push_macro("PLATFORM_WINDOWS")
#pragma push_macro("PLATFORM_COMPILER_CLANG")
#pragma push_macro("PLATFORM_APPLE")
#pragma push_macro("PLATFORM_MAC")
#pragma push_macro("PLATFORM_LINUX")
#pragma push_macro("PLATFORM_FREEBSD")
#pragma push_macro("PLATFORM_UNIX")

#undef TEXT
#undef CONSTEXPR
#undef PI
#undef dynamic_cast
#undef VARARGS
#undef CDECL
#undef STDCALL
#undef FORCEINLINE
#undef FORCENOINLINE
#undef ABSTRACT
#undef LINE_TERMINATOR
#undef LINE_TERMINATOR_ANSI
#undef DLLEXPORT
#undef DLLIMPORT
#undef LIKELY
#undef UNLIKELY
#undef RESTRICT
#undef MOBILE
#undef CONSOLE
#undef PLATFORM_WINDOWS
#undef PLATFORM_COMPILER_CLANG
#undef PLATFORM_APPLE
#undef PLATFORM_MAC
#undef PLATFORM_LINUX
#undef PLATFORM_FREEBSD
#undef PLATFORM_UNIX

#if DO_CHECK
#define UR_DO_CHECK 1

#pragma push_macro("DO_CHECK")
#pragma push_macro("checkCode")
#pragma push_macro("check")
#pragma push_macro("checkf")
#pragma push_macro("verify")
#pragma push_macro("verifyf")
#pragma push_macro("unimplemented")
#pragma push_macro("ensure")

#undef DO_CHECK
#undef checkCode
#undef check
#undef checkf
#undef verify
#undef verifyf
#undef unimplemented
#undef ensure

#endif

#ifndef BUILDING_CEF_SHARED
#define BUILDING_CEF_SHARED 0
#endif

#ifndef USING_CEF_SHARED
#define USING_CEF_SHARED 1
#endif

#define UR_INCL_STR2(x) #x
#define UR_INCL_STR(x) UR_INCL_STR2(x)

// UE4 might use combination of compiler specific macros, which CEF might not tolerate well.
// Sanitizing such macros here:

// prefer MSVC compiler macros over GCC if they're both present for some god forsaken reason
#if defined(_MSC_VER) && defined(__GNUC__)

#if _MSC_VER > 0
#pragma message ("Uranium: _MSC_VER - __GNUC__ macro collision detected, temporarily undefining __GNUC__ (GCC: " UR_INCL_STR(__GNUC__) ", MSVC: " UR_INCL_STR(_MSC_VER) ")")
#define UR_MSVC_GNUC_AVOIDANCE __GNUC__
#pragma push_macro("__GNUC__")
#undef __GNUC__
#endif

#endif

#undef UR_INCL_STR
#undef UR_INCL_STR2

#endif