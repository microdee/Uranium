/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#include "DefaultUraniumContext.h"

#include <functional>

#include "Engine/Engine.h"
#include "Uranium.h"
#include "UraniumApp.h"
#include "Misc/Paths.h"
#include "Async/Async.h"
#include "HAL/Event.h"
#include "TimerManager.h"
#include "CefUtils.h"
#include "UrCefString.h"
#include "UrSettings.h"

#if UR_CEF_SANDBOXED
#include "CefIncludesStart.h"
#include "include/cef_sandbox_win.h"
#include "CefIncludesEnd.h"
#endif

void UDefaultUraniumContext::MessageLoop()
{
	App = NewCefObject<FUraniumApp>();

	CefMainArgs mainArgs(hInstance);
	
#if UR_CEF_SANDBOXED

	CefScopedSandboxInfo sandboxInfo;
	CefInitialize(mainArgs, GetCefSettings(), App.Get(), sandboxInfo.sandbox_info());

#else
	
	CefInitialize(mainArgs, GetCefSettings(), App.Get(), nullptr);
	
#endif

	UE_LOG(LogUranium, Display, TEXT("CEF is Initialized"));

	// Tell the main thread about it
	FEvent* semaphore = FGenericPlatformProcess::GetSynchEventFromPool(true);

	AsyncTask(ENamedThreads::GameThread, [this, semaphore]()
	{
		bIsCefRunning = true;
		bIsCefInitializing = false;
		OnCefInitialized.Broadcast();

		// signal MessageLoop function that it can continue
		semaphore->Trigger();
	});

	// Wait until game thread acknowledged initialization signal
	semaphore->Wait();
	semaphore->Reset();

	// CEF/Chromium runs its own message loop from now on until shut down
	CefRunMessageLoop();
	CefShutdown();
	bIsCefRunning = false;
	if(bCefModulesLoaded)
	{
		UraniumCefUtils::UnloadCEF3Modules();
		bCefModulesLoaded = false;
	}
	UE_LOG(LogUranium, Display, TEXT("CEF is shut down"));
}

TCefPtr<FUraniumApp> UDefaultUraniumContext::GetApp()
{
	return App;
}

CefSettings UDefaultUraniumContext::GetCefSettings()
{
	auto settings = GetMutableDefault<UUrSettings>();

	CefSettings cefSettings;
	cefSettings.no_sandbox = UR_CEF_SANDBOXED ? false : true;
	cefSettings.windowless_rendering_enabled = true;
	
	cefSettings.external_message_pump = bMessageLoopOnEngineTick;
	cefSettings.multi_threaded_message_loop = false;

	cefSettings.background_color = settings->DefaultBackgroundColor.ToPackedARGB();
	cefSettings.persist_session_cookies = settings->PersistSessionCookies;
	cefSettings.persist_user_preferences = settings->PersistUserPreferences;
	cefSettings.remote_debugging_port = settings->RemoteDebugPort;

	FString projectDir = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
	FString projectUserDir = FPaths::ConvertRelativePathToFull(FPaths::ProjectUserDir());
	FString projectLogDir = FPaths::ConvertRelativePathToFull(FPaths::ProjectLogDir());
	FString cefPath(FPaths::Combine(*projectDir, TEXT("Binaries/Win64")));
	FString cefLocales(FPaths::Combine(*cefPath, TEXT("locales")));
	FString cefCache(FPaths::Combine(*projectUserDir, TEXT("CefCache")));
	FString cefUser(FPaths::Combine(*projectUserDir, TEXT("CefUser")));
	FString cefLog(FPaths::Combine(*projectLogDir, TEXT("CefLog")));

	FString cefProc;
	if(UR_CEF_SANDBOXED && !GIsEditor)
	{
		cefProc = FPlatformProcess::ExecutablePath();
	}
	else
	{
		cefProc = FPaths::Combine(*cefPath, TEXT("UraniumCefProcess.exe"));
	}
	{
		using namespace UrCefString;
		Set(&cefSettings.user_agent, GetUserAgent());
		Set(&cefSettings.resources_dir_path, cefPath);
		Set(&cefSettings.browser_subprocess_path, cefProc);
		Set(&cefSettings.locales_dir_path, cefLocales);
		Set(&cefSettings.cache_path, cefCache);
		Set(&cefSettings.user_data_path, cefUser);
		Set(&cefSettings.log_file, cefLog);
	}

	return cefSettings;
}

FString UDefaultUraniumContext::GetUserAgent()
{
	if(UserAgent.IsEmpty())
	{
		std::wstringstream useragent;
		useragent
			<< L"Mozilla/5.0 (Windows NT 10.0; Win64; x64) "
			<< L"Chrome/" << *GetChromiumVersion();

		UserAgent = FString(useragent.str().c_str());
	}
	return UserAgent;
}

void UDefaultUraniumContext::SetUserAgent(FString userAgent)
{
	UserAgent = userAgent;
}

void UDefaultUraniumContext::Initialize(UUraniumSubsystem* owner)
{
	if (bIsCefInitializing || bIsCefRunning)
	{
		return;
	}
	
	bIsCefInitializing = true;

	// Initialize CEF
	UE_LOG(LogUranium, Display, TEXT("Initializing CEF..."));
	
	if(!bCefModulesLoaded)
	{
		if(!UR_CEF_SANDBOXED || GIsEditor)
		{
			UraniumCefUtils::LoadCEF3Modules();
		}
		bCefModulesLoaded = true;
	}

	if(bMessageLoopOnEngineTick)
	{
		App = NewCefObject<FUraniumApp>();
		CefMainArgs mainArgs(hInstance);
		CefInitialize(mainArgs, GetCefSettings(), App.Get(), nullptr);

		bIsCefRunning = true;
		bIsCefInitializing = false;
		OnCefInitialized.Broadcast();

		if(FSlateApplication::IsInitialized())
		{
			FSlateApplication::Get().OnPostTick().AddUObject(this, &UDefaultUraniumContext::Tick);
		}
	}
	else
	{
		MessageLoopThread = MakeShared<std::thread, ESPMode::ThreadSafe>(
			std::bind(&UDefaultUraniumContext::MessageLoop, this)
		);
	}
}

class FCefQuitTask : public CefTask
{
public:
	FCefQuitTask() { }
	void Execute() override {
		CefQuitMessageLoop();
	}

	IMPLEMENT_REFCOUNTING(FCefQuitTask);
};

void UDefaultUraniumContext::Deinitialize()
{
	if (!bIsCefRunning || bIsCefInitializing)
	{
		return; // TODO: cancel initialization when that's in progress
	}
	
	bIsCefRunning = false;
	
	UE_LOG(LogUranium, Display, TEXT("CEF is shut down"));
	
	if(bMessageLoopOnEngineTick)
	{
		if(FSlateApplication::IsInitialized())
		{
			FSlateApplication::Get().OnPostTick().RemoveAll(this);
		}
		
		CefShutdown();
		if(bCefModulesLoaded)
		{
			if(!UR_CEF_SANDBOXED || GIsEditor)
			{
				UraniumCefUtils::UnloadCEF3Modules();
			}
			bCefModulesLoaded = false;
		}
	}
	else
	{
		CefRefPtr<CefTask> quitTask(new FCefQuitTask());
		CefPostTask(TID_UI, quitTask.get());
		MessageLoopThread->join();
		MessageLoopThread.Reset();
	}
	OnCefShutdown.Broadcast();
}

void UDefaultUraniumContext::Tick(float DeltaTime)
{
	if(IsCefRunning()) return;
	CefDoMessageLoopWork();
}

void UDefaultUraniumContext::BeginDestroy()
{
	Super::BeginDestroy();
	Deinitialize();
}