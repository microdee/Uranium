/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#pragma once

#include "Modules/ModuleManager.h"
#include "CefCompat.h"

DECLARE_LOG_CATEGORY_EXTERN(LogUranium, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogUraniumConsole, Log, All);

class FUraniumModule : public IModuleInterface
{
	
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};