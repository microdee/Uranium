// MESO Digital Interiors GmbH. (C) 2020-

#include "UraniumApp.h"
#include "UrSettings.h"

void FUraniumApp::OnBeforeCommandLineProcessing(CefString const& processType, CefRefPtr<CefCommandLine> commandLine)
{
	auto settings = GetMutableDefault<UUrSettings>();

	for(FUraniumSwitch sw : settings->ChromiumSwitches)
	{
		if (sw.IsEmpty()) continue;
		if (sw.HasValue)
			commandLine->AppendSwitchWithValue(*sw.Switch, *sw.Value);
		else
			commandLine->AppendSwitch(*sw.Switch);
	}
}

CefRefPtr<CefBrowserProcessHandler> FUraniumApp::GetBrowserProcessHandler()
{
	return this; 
}

void FUraniumApp::OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> commandLine)
{
	FString pid = FString::FromInt(FPlatformProcess::GetCurrentProcessId());
	commandLine->AppendSwitchWithValue(TEXT("ur-ue-pid"), *pid);
}