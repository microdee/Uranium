/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#pragma once
#ifdef __UNREAL__

#include "CefModuleLoader/UnrealCefModuleLoader.h"
typedef FUnrealCefModuleLoader FCefModuleLoader;

#else

#include "CefModuleLoader/NativeCefModuleLoader.h"
typedef FNativeCefModuleLoader FCefModuleLoader;

#endif

template<typename TModuleLoader>
class TScopedCefModuleLoader
{
private:
	TModuleLoader Loader;
public:
	
	TScopedCefModuleLoader() : Loader()
	{
		Loader.LoadModules();
	}

	~TScopedCefModuleLoader()
	{
		Loader.UnloadModules();
	}
};

typedef TScopedCefModuleLoader<FCefModuleLoader> FScopedCefModuleLoader;