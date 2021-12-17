/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#include "UnrealCefModuleLoader.h"

#include "CefUtils.h"
#ifdef __UNREAL__

FUnrealCefModuleLoader::FUnrealCefModuleLoader()
{
}

FUnrealCefModuleLoader::~FUnrealCefModuleLoader()
{
}

void FUnrealCefModuleLoader::LoadModules()
{
	if(!UR_CEF_SANDBOXED || GIsEditor)
	{
		UraniumCefUtils::LoadCEF3Modules();
	}
}

void FUnrealCefModuleLoader::UnloadModules()
{
	if(!UR_CEF_SANDBOXED || GIsEditor)
	{
		UraniumCefUtils::UnloadCEF3Modules();
	}
}

#endif
