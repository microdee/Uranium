/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#include "NativeCefModuleLoader.h"
#if !defined(__UNREAL__)

void FNativeCefModuleLoader::LoadModules()
{
	auto currdir = std::filesystem::current_path();
	CEF3DLLHandle = LoadDllCEF(currdir / L"libcef.dll");
	ElfHandle = LoadDllCEF(currdir / L"chrome_elf.dll");
	D3DHandle = LoadDllCEF(currdir / L"d3dcompiler_47.dll");
	GLESHandle = LoadDllCEF(currdir / L"libGLESv2.dll");
	EGLHandle = LoadDllCEF(currdir / L"libEGL.dll");
}

void FNativeCefModuleLoader::UnloadModules()
{
	FreeLibrary(CEF3DLLHandle); CEF3DLLHandle = nullptr;
	FreeLibrary(ElfHandle); ElfHandle = nullptr;
	FreeLibrary(D3DHandle); D3DHandle = nullptr;
	FreeLibrary(GLESHandle); GLESHandle = nullptr;
	FreeLibrary(EGLHandle); EGLHandle = nullptr;
}

HMODULE FNativeCefModuleLoader::LoadDllCEF(const std::filesystem::path path)
{
	if (path.empty() || path.extension() != L".dll")
	{
		return nullptr;
	}
	HMODULE handle = LoadLibraryW(path.c_str());
	if (!handle)
	{
		//DWORD ErrorNum = GetLastError();
	}
	return handle;
}

#endif