// Copyright Epic Games, Inc. All Rights Reserved.

#include "CefUtils.h"
#include "Containers/UnrealString.h"
#include "Logging/LogMacros.h"
#include "Misc/Paths.h"
#include "Modules/ModuleManager.h"
#include "HAL/PlatformProcess.h"
#include "CefUtilsLog.h"

DEFINE_LOG_CATEGORY(LogUraniumCefUtils);

namespace UraniumCefUtils
{
#if PLATFORM_WINDOWS
	void* CEF3DLLHandle = nullptr;
	void* ElfHandle = nullptr;
	void* D3DHandle = nullptr;
	void* GLESHandle = nullptr;
	void* EGLHandle = nullptr;
#endif

	void* LoadDllCEF(const FString& Path)
	{
		if (Path.IsEmpty())
		{
			return nullptr;
		}
		void* Handle = FPlatformProcess::GetDllHandle(*Path);
		if (!Handle)
		{
			int32 ErrorNum = FPlatformMisc::GetLastError();
			TCHAR ErrorMsg[1024];
			FPlatformMisc::GetSystemErrorMessage(ErrorMsg, 1024, ErrorNum);
			UE_LOG(LogUraniumCefUtils, Error, TEXT("Failed to get CEF3 DLL handle for %s: %s (%d)"), *Path, ErrorMsg, ErrorNum);
		}
		return Handle;
	}

	void LoadCEF3Modules()
	{
#if PLATFORM_WINDOWS
	#if PLATFORM_64BITS
		// TODO: point to project binaries instead of engine binaries
		FString projectDir = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
		FString DllPath(FPaths::Combine(*projectDir, TEXT("Binaries/Win64")));
	#endif

		FPlatformProcess::PushDllDirectory(*DllPath);
		CEF3DLLHandle = LoadDllCEF(FPaths::Combine(*DllPath, TEXT("libcef.dll")));
		if (CEF3DLLHandle)
		{
			ElfHandle = LoadDllCEF(FPaths::Combine(*DllPath, TEXT("chrome_elf.dll")));
			D3DHandle = LoadDllCEF(FPaths::Combine(*DllPath, TEXT("d3dcompiler_47.dll")));
			GLESHandle = LoadDllCEF(FPaths::Combine(*DllPath, TEXT("libGLESv2.dll")));
			EGLHandle = LoadDllCEF(FPaths::Combine(*DllPath, TEXT("libEGL.dll")));
		}
		FPlatformProcess::PopDllDirectory(*DllPath);
#endif
	}

	void UnloadCEF3Modules()
	{
#if PLATFORM_WINDOWS
		FPlatformProcess::FreeDllHandle(CEF3DLLHandle);
		CEF3DLLHandle = nullptr;
		FPlatformProcess::FreeDllHandle(ElfHandle);
		ElfHandle = nullptr;
		FPlatformProcess::FreeDllHandle(D3DHandle);
		D3DHandle = nullptr;
		FPlatformProcess::FreeDllHandle(GLESHandle);
		GLESHandle = nullptr;
		FPlatformProcess::FreeDllHandle(EGLHandle);
		EGLHandle = nullptr;
#endif
	}
};