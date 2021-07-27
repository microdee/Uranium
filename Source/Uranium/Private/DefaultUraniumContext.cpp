// MESO Digital Interiors GmbH. (C) 2020-

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
#include "UrSettings.h"

#define SET_STR_SETTING(setting, value) \
	CefString(&setting).FromString16(value)

#define UE4_MESSAGE_LOOP 0

void UDefaultUraniumContext::MessageLoop()
{
	App = new FUraniumApp();
	CefMainArgs mainArgs(hInstance);
	CefInitialize(mainArgs, GetCefSettings(), App, nullptr);

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

CefSettings UDefaultUraniumContext::GetCefSettings()
{
	auto settings = GetMutableDefault<UUrSettings>();

	CefSettings cefSettings;
	cefSettings.no_sandbox = true;
	cefSettings.windowless_rendering_enabled = true;
	
	cefSettings.external_message_pump = bMessageLoopOnEngineTick;
	cefSettings.multi_threaded_message_loop = false;

	cefSettings.background_color = settings->DefaultBackgroundColor.ToPackedARGB();
	cefSettings.persist_session_cookies = settings->PersistSessionCookies;
	cefSettings.persist_user_preferences = settings->PersistUserPreferences;
	cefSettings.remote_debugging_port = settings->RemoteDebugPort;

	CefString(&cefSettings.user_agent).FromString16(*GetUserAgent());

	FString projectDir = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
	FString projectUserDir = FPaths::ConvertRelativePathToFull(FPaths::ProjectUserDir());
	FString projectLogDir = FPaths::ConvertRelativePathToFull(FPaths::ProjectLogDir());
	FString cefPath(FPaths::Combine(*projectDir, TEXT("Binaries/Win64")));
	FString cefProc(FPaths::Combine(*cefPath, TEXT("UraniumCefProcess.exe")));
	FString cefLocales(FPaths::Combine(*cefPath, TEXT("locales")));
	FString cefCache(FPaths::Combine(*projectUserDir, TEXT("CefCache")));
	FString cefUser(FPaths::Combine(*projectUserDir, TEXT("CefUser")));
	FString cefLog(FPaths::Combine(*projectLogDir, TEXT("CefLog")));

	CefString(&cefSettings.resources_dir_path).FromString16(*cefPath);
	CefString(&cefSettings.browser_subprocess_path).FromString16(*cefProc);
	CefString(&cefSettings.locales_dir_path).FromString16(*cefLocales);
	CefString(&cefSettings.cache_path).FromString16(*cefCache);
	CefString(&cefSettings.user_data_path).FromString16(*cefUser);
	CefString(&cefSettings.log_file).FromString16(*cefLog);

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
		UraniumCefUtils::LoadCEF3Modules();
		bCefModulesLoaded = true;
	}

	if(bMessageLoopOnEngineTick)
	{
		App = new FUraniumApp();
		CefMainArgs mainArgs(hInstance);
		CefInitialize(mainArgs, GetCefSettings(), App, nullptr);

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
			UraniumCefUtils::UnloadCEF3Modules();
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