/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#pragma once
#ifdef __UNREAL__

#include "CoreMinimal.h"

#include <thread>

#include "Modules/ModuleManager.h"
#include "CefCompat.h"

DECLARE_LOG_CATEGORY_EXTERN(LogUraniumCefProcess, Log, All);

class FUraniumCefProcessModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
};

#endif