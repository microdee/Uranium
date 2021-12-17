/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/
#pragma once

#include "CoreMinimal.h"

#include "CefIncludesStart.h"

#include "include/cef_base.h"
#include "include/cef_app.h"

#include "CefIncludesEnd.h"

class URANIUM_API FUraniumApp
	: public CefApp
	, public CefBrowserProcessHandler
{
public:

	virtual void OnBeforeCommandLineProcessing(CefString const& processType, CefRefPtr<CefCommandLine> commandLine) override;

	virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override;
	virtual void OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> commandLine) override;

	IMPLEMENT_REFCOUNTING(FUraniumApp);
};