// MESO Digital Interiors GmbH. (C) 2020-

#include "UraniumApp.h"
#include "UrSettings.h"

void FUraniumApp::OnBeforeCommandLineProcessing(CefString const& process_type, CefRefPtr<CefCommandLine> command_line)
{
	auto settings = GetMutableDefault<UUrSettings>();

	for(auto sw : settings->ChromiumSwitches)
	{
		if (sw.IsEmpty()) continue;
		if (sw.HasValue)
			command_line->AppendSwitchWithValue(*sw.Switch, *sw.Value);
		else
			command_line->AppendSwitch(*sw.Switch);
	}
}

CefRefPtr<CefBrowserProcessHandler> FUraniumApp::GetBrowserProcessHandler()
{
	return this; 
}

void FUraniumApp::OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> command_line)
{
	auto pid = FString::FromInt(FPlatformProcess::GetCurrentProcessId());
	command_line->AppendSwitchWithValue(TEXT("ur-ue-pid"), *pid);
}