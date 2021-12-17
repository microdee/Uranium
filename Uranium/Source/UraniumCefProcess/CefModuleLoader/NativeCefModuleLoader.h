/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#pragma once
#if !defined(__UNREAL__)

#include "GenericCefModuleLoader.h"
#include <filesystem>
#include <Windows.h>
#include <winternl.h>

class FNativeCefModuleLoader : FGenericCefModuleLoader
{
public:
	virtual void LoadModules() override;
	virtual void UnloadModules() override;
private:
	
	HMODULE LoadDllCEF(const std::filesystem::path path);

	HMODULE CEF3DLLHandle = nullptr;
	HMODULE ElfHandle = nullptr;
	HMODULE D3DHandle = nullptr;
	HMODULE GLESHandle = nullptr;
	HMODULE EGLHandle = nullptr;
};

#endif