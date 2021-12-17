/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/
#pragma once

#include <thread>

#include "CefIncludesStart.h"

#include "include/cef_app.h"
#include "include/cef_browser.h"

#include "CefIncludesEnd.h"

class UraniumCefApp
	: public CefApp
	, public CefRenderProcessHandler
	, public CefBrowserProcessHandler
{
public:

	std::shared_ptr<std::thread> ParentCheckerThread;
	static void ParentCheckerBody(CefString pidStr /* intentional copy */);

	/** Load and unload CEF modules around this function */
	static int Main(HINSTANCE hInstanceIn);

	virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override;

	virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override;

	virtual void OnBeforeCommandLineProcessing(const CefString& processType, CefRefPtr<CefCommandLine> commandLine) override;

	//void OnBrowserCreated(CefRefPtr<CefBrowser> browser) override;

	virtual void OnBrowserDestroyed(CefRefPtr<CefBrowser> browser) override;

	virtual void OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) override;

	virtual void OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) override;

	virtual void OnFocusedNodeChanged(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefDOMNode> node) override;

	//bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) override;

	//void OnRenderThreadCreated(CefRefPtr<CefListValue> extra_info) override;

	IMPLEMENT_REFCOUNTING(UraniumCefApp);
};
