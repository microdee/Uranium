/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#include "UraniumCefApp.h"

#ifndef UR_CEF_SANDBOXED
#define UR_CEF_SANDBOXED 0
#endif

#if UR_CEF_SANDBOXED
#include "CefIncludesStart.h"
#include "include/cef_sandbox_win.h"
#include "CefIncludesEnd.h"
#endif

void UraniumCefApp::ParentCheckerBody(CefString pidStr)
{
	// preventing this process to become a zombie

	const auto pid = _wtoi(pidStr.c_str());
	const auto ueProc = OpenProcess(SYNCHRONIZE, false, pid);
	WaitForSingleObject(ueProc, INFINITE);
	CloseHandle(ueProc);
	const auto currProc = GetCurrentProcess();
	TerminateProcess(currProc, 0);
}

int UraniumCefApp::Main(HINSTANCE hInstanceIn)
{
	CefEnableHighDPISupport();
	CefMainArgs mainArgs(hInstanceIn);
	
	CefRefPtr<CefApp> app(new UraniumCefApp);
	
#if UR_CEF_SANDBOXED

	CefScopedSandboxInfo sandboxInfo;
	int result = CefExecuteProcess(mainArgs, app, sandboxInfo.sandbox_info());

#else

	int result = CefExecuteProcess(mainArgs, app, nullptr);
	
#endif
	
	return result;
}

CefRefPtr<CefRenderProcessHandler> UraniumCefApp::GetRenderProcessHandler()
{
	return CefRefPtr<CefRenderProcessHandler>(this);
}

CefRefPtr<CefBrowserProcessHandler> UraniumCefApp::GetBrowserProcessHandler()
{
	return CefRefPtr<CefBrowserProcessHandler>(this);
}

void UraniumCefApp::OnBeforeCommandLineProcessing(const CefString& processType, CefRefPtr<CefCommandLine> commandLine)
{
	if(commandLine->HasSwitch(L"ur-ue-pid"))
	{
		ParentCheckerThread = std::make_shared<std::thread>(
			&UraniumCefApp::ParentCheckerBody,
			commandLine->GetSwitchValue(L"ur-ue-pid")
		);
	}
}

//void UraniumCefApp::OnBrowserCreated(CefRefPtr<CefBrowser> browser)
//{
//}

void UraniumCefApp::OnBrowserDestroyed(CefRefPtr<CefBrowser> browser)
{
}

void UraniumCefApp::OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
{
	// TODO
}

void UraniumCefApp::OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
{
	// TODO
}

void UraniumCefApp::OnFocusedNodeChanged(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefDOMNode> node)
{
	// TODO
}

//bool UraniumCefApp::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message)
//{
//  // TODO
//  return true;
//}

//void UraniumCefApp::OnRenderThreadCreated(CefRefPtr<CefListValue> extra_info)
//{
//  // TODO
//}