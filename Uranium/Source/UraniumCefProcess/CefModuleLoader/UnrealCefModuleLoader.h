/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#pragma once

#ifdef __UNREAL__

#include "CoreMinimal.h"

#include "GenericCefModuleLoader.h"

/**
 * 
 */
class FUnrealCefModuleLoader : public FGenericCefModuleLoader
{
public:
	FUnrealCefModuleLoader();
	virtual ~FUnrealCefModuleLoader() override;

	virtual void LoadModules() override;
	virtual void UnloadModules() override;
};

#endif
