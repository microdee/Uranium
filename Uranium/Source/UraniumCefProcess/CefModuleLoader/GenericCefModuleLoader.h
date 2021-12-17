/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/
#pragma once

class FGenericCefModuleLoader
{
public:
	virtual ~FGenericCefModuleLoader() = default;

	virtual void LoadModules() = 0;
	virtual void UnloadModules() = 0;
};
