
// restore temporary macro undefs

#pragma pop_macro("TEXT")
#pragma pop_macro("CONSTEXPR")
#pragma pop_macro("PI")
#pragma pop_macro("dynamic_cast")
#pragma pop_macro("VARARGS")
#pragma pop_macro("CDECL")
#pragma pop_macro("STDCALL")
#pragma pop_macro("FORCEINLINE")
#pragma pop_macro("FORCENOINLINE")
#pragma pop_macro("ABSTRACT")
#pragma pop_macro("LINE_TERMINATOR")
#pragma pop_macro("LINE_TERMINATOR_ANSI")
#pragma pop_macro("DLLEXPORT")
#pragma pop_macro("DLLIMPORT")
#pragma pop_macro("LIKELY")
#pragma pop_macro("UNLIKELY")
#pragma pop_macro("RESTRICT")
#pragma pop_macro("MOBILE")
#pragma pop_macro("CONSOLE")
#pragma pop_macro("PLATFORM_WINDOWS")
#pragma pop_macro("PLATFORM_COMPILER_CLANG")
#pragma pop_macro("PLATFORM_APPLE")
#pragma pop_macro("PLATFORM_MAC")
#pragma pop_macro("PLATFORM_LINUX")
#pragma pop_macro("PLATFORM_FREEBSD")
#pragma pop_macro("PLATFORM_UNIX")

#ifdef UR_DO_CHECK
#undef UR_DO_CHECK

#pragma pop_macro("DO_CHECK")
#pragma pop_macro("checkCode")
#pragma pop_macro("check")
#pragma pop_macro("checkf")
#pragma pop_macro("verify")
#pragma pop_macro("verifyf")
#pragma pop_macro("unimplemented")
#pragma pop_macro("ensure")

#endif

#ifdef UR_MSVC_GNUC_AVOIDANCE
#pragma pop_macro("__GNUC__")
#endif

#include "Windows/HideWindowsPlatformAtomics.h"
#include "Windows/HideWindowsPlatformTypes.h"
THIRD_PARTY_INCLUDES_END

#undef UR_CEF_INCLUDE_REGION