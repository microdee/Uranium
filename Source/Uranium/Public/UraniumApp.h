// MESO Digital Interiors GmbH. (C) 2020-
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "CefIncludesStart.h"

#include "include/cef_base.h"
#include "include/cef_app.h"
#include "include/cef_browser.h"
#include "include/cef_client.h"
#include "include/cef_version.h"

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