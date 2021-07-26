#include "UraniumCefApp.h"

void UraniumCefApp::ParentCheckerBody(CefString pid_cla)
{
	// preventing this process to become a zombie

	const auto pid = _wtoi(pid_cla.c_str());
	const auto ue_proc = OpenProcess(SYNCHRONIZE, false, pid);
	WaitForSingleObject(ue_proc, INFINITE);
	CloseHandle(ue_proc);
	const auto currProc = GetCurrentProcess();
	TerminateProcess(currProc, 0);
}

CefRefPtr<CefRenderProcessHandler> UraniumCefApp::GetRenderProcessHandler()
{
	return CefRefPtr<CefRenderProcessHandler>(this);
}

CefRefPtr<CefBrowserProcessHandler> UraniumCefApp::GetBrowserProcessHandler()
{
	return CefRefPtr<CefBrowserProcessHandler>(this);
}

void UraniumCefApp::OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line)
{
	if(command_line->HasSwitch(L"ur-ue-pid"))
	{
		ParentCheckerThread = std::make_shared<std::thread>(
			&UraniumCefApp::ParentCheckerBody,
			command_line->GetSwitchValue(L"ur-ue-pid")
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