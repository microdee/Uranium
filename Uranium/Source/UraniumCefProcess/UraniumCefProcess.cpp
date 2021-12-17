/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#ifdef __UNREAL__

#include "UraniumCefProcess.h"
#include "CefModuleLoader.h"
#include "UraniumCefApp.h"

DEFINE_LOG_CATEGORY(LogUraniumCefProcess);

#define LOCTEXT_NAMESPACE "FUraniumCefProcessModule"

void FUraniumCefProcessModule::StartupModule()
{
#if UR_CEF_SANDBOXED
	if(!GIsEditor)
	{
		FString cmdLine = FCommandLine::Get();
		if(cmdLine.Contains(TEXT("ur-ue-pid")))
		{
			FScopedCefModuleLoader moduleLoader;
			
			auto hInst = ::GetModuleHandle(nullptr);

			int result = UraniumCefApp::Main(hInst);
	
			::ExitProcess(result);
		}
	}
#endif
}

void FUraniumCefProcessModule::ShutdownModule()
{
	
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FUraniumCefProcessModule, UraniumCefProcess)

#endif
