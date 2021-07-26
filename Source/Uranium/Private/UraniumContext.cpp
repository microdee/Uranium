// MESO Digital Interiors GmbH. (C) 2020-

#include "UraniumContext.h"

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

TSharedPtr<FUraniumContext> FUraniumContext::Singleton;

void FUraniumContext::MessageLoop()
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
		semaphore->Trigger();
	});
	semaphore->Wait();
	semaphore->Reset();

	// CEF/Chromium runs its own message loop from now on until shut down
	CefRunMessageLoop();
	CefShutdown();
	bIsCefRunning = false;
	UE_LOG(LogUranium, Display, TEXT("CEF is shut down"));
}

CefSettings FUraniumContext::GetCefSettings()
{
	auto settings = GetMutableDefault<UUrSettings>();

	CefSettings s;
	s.no_sandbox = true;
	s.windowless_rendering_enabled = true;
#if UE4_MESSAGE_LOOP
	s.external_message_pump = true;
	s.multi_threaded_message_loop = false;
#else
	s.external_message_pump = false;
	s.multi_threaded_message_loop = false;
#endif

	s.background_color = settings->DefaultBackgroundColor.ToPackedARGB();
	s.persist_session_cookies = settings->PersistSessionCookies;
	s.persist_user_preferences = settings->PersistUserPreferences;
	s.remote_debugging_port = settings->RemoteDebugPort;

	SET_STR_SETTING(s.user_agent, *GetUserAgent());

	FString projectDir = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
	FString projectUserDir = FPaths::ConvertRelativePathToFull(FPaths::ProjectUserDir());
	FString projectLogDir = FPaths::ConvertRelativePathToFull(FPaths::ProjectLogDir());
	FString cefPath(FPaths::Combine(*projectDir, TEXT("Binaries/Win64")));
	FString cefProc(FPaths::Combine(*cefPath, TEXT("UraniumCefProcess.exe")));
	FString cefLocales(FPaths::Combine(*cefPath, TEXT("locales")));
	FString cefCache(FPaths::Combine(*projectUserDir, TEXT("CefCache")));
	FString cefUser(FPaths::Combine(*projectUserDir, TEXT("CefUser")));
	FString cefLog(FPaths::Combine(*projectLogDir, TEXT("CefLog")));

	SET_STR_SETTING(s.resources_dir_path, *cefPath);
	SET_STR_SETTING(s.browser_subprocess_path, *cefProc);
	SET_STR_SETTING(s.locales_dir_path, *cefLocales);
	SET_STR_SETTING(s.cache_path, *cefCache);
	SET_STR_SETTING(s.user_data_path, *cefUser);
	SET_STR_SETTING(s.log_file, *cefLog);

	return s;
}

FString FUraniumContext::GetUserAgent()
{
	if(sUserAgent.IsEmpty())
	{
		std::wstringstream useragent;
		useragent
			<< L"Mozilla/5.0 (Windows NT 10.0; Win64; x64) "
			<< L"Chrome/" << *GetChromiumVersion();

		sUserAgent = FString(useragent.str().c_str());
	}
	return sUserAgent;
}

void FUraniumContext::SetUserAgent(FString userAgent)
{
	sUserAgent = userAgent;
}

FString FUraniumContext::GetChromiumVersion()
{
	std::wstringstream ver;
	ver << CHROME_VERSION_MAJOR << L"."
		<< CHROME_VERSION_MINOR << L"."
		<< CHROME_VERSION_BUILD << L"."
		<< CHROME_VERSION_PATCH;

	return FString(ver.str().c_str());
}

void FUraniumContext::Initialize()
{
	if (bIsCefInitializing || bIsCefRunning) return;
	if(!bCefModulesLoaded)
	{
		UraniumCefUtils::LoadCEF3Modules();
		bCefModulesLoaded = true;
	}
	bIsCefInitializing = true;

	// Initialize CEF
	UE_LOG(LogUranium, Display, TEXT("Initializing CEF..."));

#if UE4_MESSAGE_LOOP

	App = new FUraniumApp();
	CefMainArgs mainArgs(hInstance);
	CefInitialize(mainArgs, GetCefSettings(), App, nullptr);

	bIsCefRunning = true;
	bIsCefInitializing = false;
	OnCefInitialized.Broadcast();

	// TODO: Tick without a world context
#else
	MessageLoopThread = MakeShared<std::thread>(
		std::bind(&FUraniumContext::MessageLoop, this)
	);
#endif
}

void FUraniumContext::Shutdown()
{
	if (!bIsCefRunning || bIsCefInitializing) return;
	bIsCefRunning = false;
	CefShutdown();
	UE_LOG(LogUranium, Display, TEXT("CEF is shut down"));
	if(bCefModulesLoaded)
	{
		UraniumCefUtils::UnloadCEF3Modules();
		bCefModulesLoaded = false;
	}

#if !UE4_MESSAGE_LOOP
	CefRefPtr<CefTask> qtask(new QuitTask());
	CefPostTask(TID_UI, qtask.get());
	MessageLoopThread->join();
	MessageLoopThread.Reset();
#endif
}

bool FUraniumContext::IsCefRunning()
{
	return bIsCefRunning;
}

void UUraniumContext::GlobalTick()
{
	auto ctx = GetSingletonUraniumContext();
	if(!ctx->IsCefRunning()) return;
	CefDoMessageLoopWork();
	// TODO: Tick without a world context
}

TSharedPtr<FUraniumContext> GetSingletonUraniumContext()
{
	if (!FUraniumContext::Singleton)
		FUraniumContext::Singleton = MakeShared<FUraniumContext>();

	return FUraniumContext::Singleton;
}

void UUraniumContext::InitializeUranium(FOnCefInitializedDel onInitialized, UObject* worldContext)
{
	auto ctx = GetSingletonUraniumContext();
	if(ctx->IsCefRunning())
	{
		onInitialized.ExecuteIfBound();
	}
	else
	{
		if (onInitialized.IsBound())
			ctx->OnCefInitialized.Add(onInitialized);
		ctx->Initialize();
	}
}

void UUraniumContext::ShutdownUranium()
{
	GetSingletonUraniumContext()->Shutdown();
}

FString UUraniumContext::GetUserAgent()
{
	return GetSingletonUraniumContext()->GetUserAgent();
}

void UUraniumContext::SetUserAgent(FString userAgent)
{
	GetSingletonUraniumContext()->SetUserAgent(userAgent);
}

FString UUraniumContext::GetCefVersion()
{
	return GetSingletonUraniumContext()->GetCefVersion();
}

FString UUraniumContext::GetChromiumVersion()
{
	return GetSingletonUraniumContext()->GetChromiumVersion();
}

bool UUraniumContext::IsCefRunning()
{
	return GetSingletonUraniumContext()->IsCefRunning();
}